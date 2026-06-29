/*****************************************************************************\
* RunVilli.c
*
* AUTHOR  : Pedro Italo Borges Cardoso
* DATE    : 2026-04-16
\*****************************************************************************/
#include "ift.h"
#include "iftArgs.h"
#include "VILLI.h"
#include <stdio.h>
#include <stdlib.h>

void usage();
void readImgInputs(
    iftArgs *args,
    iftImage **img,
    iftImage **labels,
    const char **path
);

bool isBorderVoxel(iftImage *label_img, int p_idx);

LIST_VILLI_PIECE_OBJ* traceBorder(iftImage *label_img, int start_idx, int label);

void villiToSVG(const char *input_path, const char *output_path);

iftColor computeMeanColor(iftImage *orig, iftImage *label_img, int label);

LIST_VILLI_PIECE_OBJ** getAllBorders(iftImage *orig, iftImage *label_img, int *nlabels_out);

void writeVilliFile(const char *path, LIST_VILLI_PIECE_OBJ **lists, int nlabels, iftImage* img);


int main(int argc, char const *argv[])
{
    iftArgs *args = iftCreateArgs(argc, argv);

    if (!iftExistArg(args, "img") || !iftExistArg(args, "labels") || !iftExistArg(args, "out")) {
        usage();
        iftDestroyArgs(&args);
        return EXIT_FAILURE;
    }

    iftImage *img = NULL, *label_img = NULL;
    const char *out_path = NULL;

    readImgInputs(args, &img, &label_img, &out_path);
    int keepScratch = iftExistArg(args, "keepScratch");
    int nlabels = 0;


    LIST_VILLI_PIECE_OBJ **lists = getAllBorders(img, label_img, &nlabels);
    char kpv_path[512];
    char svg_path[512];

    snprintf(kpv_path, sizeof(kpv_path), "%s.kpv", out_path);
    snprintf(svg_path, sizeof(svg_path), "%s.svg", out_path);

    writeVilliFile(kpv_path, lists, nlabels, img);
    villiToSVG(kpv_path, svg_path);

    if (!keepScratch) {
        remove(kpv_path);
    }

    for (int i = 0; i < nlabels; i++) {
        if (lists[i] != NULL) {
            villiDestructListPieceObj(lists[i]);
        }
    }

    free(lists);
    iftDestroyImage(&img);
    iftDestroyImage(&label_img);
    iftDestroyArgs(&args);

    return EXIT_SUCCESS;
}

void usage() {
    printf("\n--img <image>\n");
    printf("--labels <label image>\n");
    printf("--out <output txt>\n\n");
    printf("Optional\n\n");
    printf("--keepScratch\n");
}

void readImgInputs(iftArgs *args, iftImage **img, iftImage **labels, const char **path) {
    *img = iftReadImageByExt(iftGetArg(args, "img"));
    *labels = iftReadImageByExt(iftGetArg(args, "labels"));
    *path = iftGetArg(args, "out");

    iftVerifyImageDomains(*img, *labels, __func__);
}

bool isBorderVoxel(iftImage *label_img, int p_idx) {
    bool resp = false;
    iftVoxel p = iftGetVoxelCoord(label_img, p_idx);
    iftAdjRel *A = iftCircular(1.6);

    for(int i = 0; resp == false && i < A->n; i++) {
        iftVoxel q = iftGetAdjacentVoxel(A, p, i);

        if(iftValidVoxel(label_img, q)) {
            int q_idx = iftGetVoxelIndex(label_img, q);
            if(label_img->val[p_idx] != label_img->val[q_idx]) {
                resp = true;
            }
        }
    }

    iftDestroyAdjRel(&A);
    return resp;
}



LIST_VILLI_PIECE_OBJ* traceBorder(iftImage *label_img, int start_pixel, int label)
{
    int dx[8] = {1, 1, 0,-1,-1,-1, 0, 1};
    int dy[8] = {0, 1, 1, 1, 0,-1,-1,-1};

    LIST_VILLI_PIECE_OBJ *list = villiCreateListPieceObj((iftColor){0});

    iftVoxel start = iftGetVoxelCoord(label_img, start_pixel);
    iftVoxel current = start;

    int direction = 0;
    list->layer = start.z;

    bool neighbor_found = false;
    int start_direction = -1;
    int start_visit_count = 0;
    int security_max_steps = label_img->xsize * label_img->ysize;
    int steps = 0;

    do {

        neighbor_found = false;
        steps++;

        int ndir = 0;

        for (int i = 0; !neighbor_found && i < 8; i++) {

            ndir = (direction + i) % 8;

            iftVoxel next = {
                current.x + dx[ndir],
                current.y + dy[ndir],
                current.z
            };

            if (iftValidVoxel(label_img, next)) {

                int idx = iftGetVoxelIndex(label_img, next);

                if (label_img->val[idx] == label) {

                    villiAddPieceObj(
                        list,
                        villiCreatePieceObj(current, ndir)
                    );

                    current = next;

                    direction = (ndir + 6) % 8;

                    neighbor_found = true;

                    if (current.x == start.x &&
                        current.y == start.y &&
                        current.z == start.z)
                    {
                        if (start_direction == -1) {
                            start_direction = direction;
                        } else if (direction == start_direction) {
                            start_visit_count = 2;
                        }
                    }
                }
            }
        }

    } while (
        neighbor_found &&
        start_visit_count < 2 &&
        steps < security_max_steps
    );

    return list;
}

iftColor computeMeanColor(iftImage *orig, iftImage *label_img, int label) {
    iftColor c = {0};
    c.val[0] = 0;
    c.val[1] = 0;
    c.val[2] = 0;
    
    long y = 0, cb = 0, cr = 0;
    int count = 0;

    for(int i = 0; i < orig->n; i++) {
        if(label_img->val[i] == label) {
            y += orig->val[i];
            if(iftIsColorImage(orig)) {
                cb += orig->Cb[i];
                cr += orig->Cr[i];
            }
            count++;
        }
    }


    if(count > 0) {
        c.val[0] = y / (count * 1.0);
        c.val[1] = cb / (count * 1.0);
        c.val[2] = cr / (count * 1.0);
    }

    return iftYCbCrtoRGB(c,255); 
}


LIST_VILLI_PIECE_OBJ** getAllBorders(iftImage *orig, iftImage *label_img, int *nlabels) {
    int min_label = 0;
    int max_label = 0;
   
    iftMinMaxValues(label_img, &min_label, &max_label);

    *nlabels = max_label - min_label + 1;

    LIST_VILLI_PIECE_OBJ **lists = calloc((*nlabels), sizeof(LIST_VILLI_PIECE_OBJ*));

    int *processed = calloc((*nlabels), sizeof(int));
    int index = 0;

    for(int p = 0; p < label_img->n; p++) {
        int label = label_img->val[p];
        int id = label - min_label;

        if(processed[id] == 0 && isBorderVoxel(label_img, p)) {
            lists[index] = traceBorder(label_img, p, label);
            lists[index]->id = id;
            lists[index]->color = computeMeanColor(orig, label_img, label);
 
            processed[id] = 1;
            index++;
        }
    }

    free(processed);

    return lists;
}

void writeVilliFile(const char *path, LIST_VILLI_PIECE_OBJ **lists, int nlabels, iftImage *img) {
    FILE *f = fopen(path, "w");
    
    fprintf(f, "%d,%d,%d\n", img->xsize, img->ysize, (nlabels));

    for(int i = 0; i < (nlabels); i++) {
        if(lists[i] == NULL) continue;

        LIST_VILLI_PIECE_OBJ *l = lists[i];

        fprintf(f, "1 ");
        fprintf(f, "%d,%d,%d %d %d ",
            l->color.val[0],
            l->color.val[1],
            l->color.val[2],
            l->v_length,
            l->id
        );

        VILLI_PIECE_OBJ *cur = l->first;

        while(cur != NULL) {
            fprintf(f, "%d,%d,%d ", cur->value.x, cur->value.y, cur->orientation);
            cur = cur->next;
        }

        fprintf(f, "\n");
    }

    fclose(f);
}
