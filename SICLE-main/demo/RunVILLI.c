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

bool isBorderVoxel(iftImage *label_img, int p_idx, iftAdjRel *A);

LIST_VILLI_PIECE_OBJ* traceBorder(
    iftImage *label_img,
    int start_idx,
    int label
);

iftColor computeMeanColor(
    iftImage *orig,
    iftImage *label_img,
    int label
);

LIST_VILLI_PIECE_OBJ** getAllBorders(
    iftImage *orig,
    iftImage *label_img,
    int *nlabels_out
);

void writeVilliFile(
    const char *path,
    LIST_VILLI_PIECE_OBJ **lists,
    int nlabels
);

int main(int argc, char const *argv[])
{
    iftArgs *args = iftCreateArgs(argc, argv);

    if (!iftExistArg(args, "img") ||
        !iftExistArg(args, "labels") ||
        !iftExistArg(args, "out")) {
        usage();
        iftDestroyArgs(&args);
        return EXIT_FAILURE;
    }

    iftImage *img = NULL, *label_img = NULL;
    const char *out_path = NULL;

    readImgInputs(args, &img, &label_img, &out_path);
    iftDestroyArgs(&args);

    int nlabels;
    LIST_VILLI_PIECE_OBJ **lists =
        getAllBorders(img, label_img, &nlabels);

    writeVilliFile(out_path, lists, nlabels);

    for(int i = 0; i < nlabels; i++) {
        if(lists[i] != NULL)
            villiDestructListPieceObj(lists[i]);
    }
    free(lists);

    iftDestroyImage(&img);
    iftDestroyImage(&label_img);

    return EXIT_SUCCESS;
}


void usage() {
    printf("\n--img <image>\n");
    printf("--labels <label image>\n");
    printf("--out <output txt>\n\n");
}

void readImgInputs(
    iftArgs *args,
    iftImage **img,
    iftImage **labels,
    const char **path
) {
    *img = iftReadImageByExt(iftGetArg(args, "img"));
    *labels = iftReadImageByExt(iftGetArg(args, "labels"));
    *path = iftGetArg(args, "out");

    iftVerifyImageDomains(*img, *labels, __func__);
}

bool isBorderVoxel(iftImage *label_img, int p_idx, iftAdjRel *A) {
    iftVoxel p = iftGetVoxelCoord(label_img, p_idx);

    for(int i = 0; i < A->n; i++) {
        iftVoxel q = iftGetAdjacentVoxel(A, p, i);

        if(iftValidVoxel(label_img, q)) {
            int q_idx = iftGetVoxelIndex(label_img, q);
            if(label_img->val[p_idx] != label_img->val[q_idx])
                return true;
        }
    }
    return false;
}


LIST_VILLI_PIECE_OBJ* traceBorder(
    iftImage *label_img,
    int start_idx,
    int label
) {
    int dx[8] = {1,1,0,-1,-1,-1,0,1};
    int dy[8] = {0,1,1,1,0,-1,-1,-1};

    LIST_VILLI_PIECE_OBJ *list =
        villiCreateListPieceObj((iftColor){0});

    iftVoxel start = iftGetVoxelCoord(label_img, start_idx);
    iftVoxel current = start;
    int dir = 0;

    do {
        villiAddPieceObj(list,
            villiCreatePieceObj(current));

        bool found = false;

        for(int i = 0; i < 8; i++) {
            int ndir = (dir + i) % 8;

            iftVoxel next = {
                current.x + dx[ndir],
                current.y + dy[ndir],
                current.z
            };

            if(iftValidVoxel(label_img, next)) {
                int idx = iftGetVoxelIndex(label_img, next);

                if(label_img->val[idx] == label) {
                    current = next;
                    dir = (ndir + 6) % 8;
                    found = true;
                    break;
                }
            }
        }

        if(!found) break;

    } while(!(current.x == start.x &&
              current.y == start.y &&
              current.z == start.z));

    return list;
}


iftColor computeMeanColor(
    iftImage *orig,
    iftImage *label_img,
    int label
) {
    long y=0, cb=0, cr=0;
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

    iftColor c = {0};
    c.val[0] = y / count;
    c.val[1] = cb / count;
    c.val[2] = cr / count;

    return c;
}


LIST_VILLI_PIECE_OBJ** getAllBorders(
    iftImage *orig,
    iftImage *label_img,
    int *nlabels_out
) {
    int min_label, max_label;
    iftMinMaxValues(label_img, &min_label, &max_label);

    int nlabels = max_label - min_label + 1;
    *nlabels_out = nlabels;

    LIST_VILLI_PIECE_OBJ **lists =
        calloc(nlabels, sizeof(LIST_VILLI_PIECE_OBJ*));

    iftAdjRel *A = iftCircular(1.6);

    int *processed = calloc(nlabels, sizeof(int));

    for(int p = 0; p < label_img->n; p++) {

        int label = label_img->val[p];
        int idx = label - min_label;

        if(processed[idx]) continue;

        if(isBorderVoxel(label_img, p, A)) {
            lists[idx] = traceBorder(label_img, p, label);
            lists[idx]->color =
                computeMeanColor(orig, label_img, label);

            processed[idx] = 1;
        }
    }

    iftDestroyAdjRel(&A);
    free(processed);

    return lists;
}

void writeVilliFile(
    const char *path,
    LIST_VILLI_PIECE_OBJ **lists,
    int nlabels
) {
    FILE *f = fopen(path, "w");

    for(int i = 0; i < nlabels; i++) {
        if(lists[i] == NULL) continue;

        LIST_VILLI_PIECE_OBJ *l = lists[i];

        fprintf(f, "1 ");
        fprintf(f, "%d,%d,%d ",
            l->color.val[0],
            l->color.val[1],
            l->color.val[2]);

        VILLI_PIECE_OBJ *cur = l->first;

        while(cur != NULL) {
            fprintf(f, "%d,%d ",
                cur->value.x,
                cur->value.y);
            cur = cur->next;
        }

        fprintf(f, "\n");
    }

    fclose(f);
}