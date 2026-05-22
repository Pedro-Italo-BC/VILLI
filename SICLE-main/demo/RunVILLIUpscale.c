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
#include <math.h>

void usage();

void writeExpandedKPV(const char *path, iftVoxel **pieces, int *points_per_label, iftColor *colors, int labels_amount, int width, int height);

void readInputs(iftArgs *args, const char **kpv_path, double *upscale, const char **out_path);

void villiResizePieceObj(iftVoxel *piece_obj, iftVoxel *new_piece_obj, int points_amount, double magnitude);

iftVoxel **villiReadKpvFileVec(const char *input_path, int *labels_amount, int **points_per_label, iftColor **colors, int *width, int *height);

int main(int argc, char const *argv[])
{
    iftArgs *args = iftCreateArgs(argc, argv);

    if (!iftExistArg(args, "kpv") ||
        !iftExistArg(args, "upscale") ||
        !iftExistArg(args, "out")) {

        usage();
        iftDestroyArgs(&args);

        return EXIT_FAILURE;
    }

    const char *kpv_path = NULL;
    const char *out_path = NULL;

    double upscale = 0.0;

    int labels_amount = 0;
    int width = 0;
    int height = 0;

    int *points_per_label = NULL;

    iftVoxel **pieces = NULL;
    iftVoxel **expanded = NULL;
    iftColor *colors = NULL;

    readInputs(args, &kpv_path, &upscale, &out_path);

    char kpv_path_name[512];
    char svg_path_name[512];

    snprintf(kpv_path_name, sizeof(kpv_path_name), "%s.kpv", out_path);
    snprintf(svg_path_name, sizeof(svg_path_name), "%s.svg", out_path);

    pieces = villiReadKpvFileVec(kpv_path, &labels_amount, &points_per_label, &colors, &width, &height);

    if (pieces == NULL) {
        fprintf(stderr, "Erro ao ler KPV\n");
        iftDestroyArgs(&args);

        return EXIT_FAILURE;
    }

    expanded = malloc(sizeof(iftVoxel*) * labels_amount);

    if (expanded == NULL) {
        fprintf(stderr, "Erro de memória\n");

        for (int i = 0; i < labels_amount; i++) {
            free(pieces[i]);
        }

        free(pieces);
        free(points_per_label);

        iftDestroyArgs(&args);

        return EXIT_FAILURE;
    }

    for (int i = 0; i < labels_amount; i++) {

        int n = points_per_label[i];

        expanded[i] = malloc(sizeof(iftVoxel) * n);

        if (expanded[i] == NULL) {
            fprintf(stderr, "Erro de memória\n");

            for (int j = 0; j < labels_amount; j++) {
                free(pieces[j]);
            }

            for (int j = 0; j < i; j++) {
                free(expanded[j]);
            }

            free(pieces);
            free(expanded);
            free(points_per_label);

            iftDestroyArgs(&args);

            return EXIT_FAILURE;
        }

        villiResizePieceObj(pieces[i], expanded[i], n, upscale);
    }

    writeExpandedKPV(kpv_path_name, expanded, points_per_label, colors, labels_amount, width, height);
    villiToSVG(kpv_path_name, svg_path_name);
    
    for (int i = 0; i < labels_amount; i++) {
        free(pieces[i]);
        free(expanded[i]);
    }

    free(pieces);
    free(expanded);
    free(points_per_label);
    free(colors);
    iftDestroyArgs(&args);

    return EXIT_SUCCESS;
}

void usage()
{
    printf("Usage:\n\n");
    printf("--kpv <input kpv>\n");
    printf("--upscale <expand magnitude>\n");
    printf("--out <output kpv>\n\n");
}

void readInputs(iftArgs *args, const char **kpv_path, double *upscale, const char **out_path)
{
    *kpv_path = iftGetArg(args, "kpv");
    *upscale = atof(iftGetArg(args, "upscale"));
    *out_path = iftGetArg(args, "out");
}

void villiResizePieceObj(iftVoxel *piece_obj, iftVoxel *new_piece_obj, int points_amount, double magnitude)
{
    for (int i = 0; i < points_amount; i++) {

        iftVoxel prev = piece_obj[(i - 1 + points_amount) % points_amount];
        iftVoxel curr = piece_obj[i];
        iftVoxel next = piece_obj[(i + 1) % points_amount];

        double dx1 = curr.x - prev.x;
        double dy1 = curr.y - prev.y;

        double dx2 = next.x - curr.x;
        double dy2 = next.y - curr.y;

        double nx1 = dy1;
        double ny1 = -dx1;

        double nx2 = dy2;
        double ny2 = -dx2;

        double len1 = sqrt(nx1 * nx1 + ny1 * ny1);
        double len2 = sqrt(nx2 * nx2 + ny2 * ny2);

        if (len1 < 1e-8 || len2 < 1e-8) {
            new_piece_obj[i] = curr;
            continue;
        }

        nx1 /= len1;
        ny1 /= len1;

        nx2 /= len2;
        ny2 /= len2;

        double bx = nx1 + nx2;
        double by = ny1 + ny2;

        double bisLen = sqrt(bx * bx + by * by);

        if (bisLen < 1e-8) {
            new_piece_obj[i] = curr;
            continue;
        }

        bx /= bisLen;
        by /= bisLen;

        new_piece_obj[i].x = (int)round(curr.x + bx * magnitude);
        new_piece_obj[i].y = (int)round(curr.y + by * magnitude);
        new_piece_obj[i].z = 0;
    }
}


void writeExpandedKPV(const char *path, iftVoxel **pieces, int *points_per_label, iftColor *colors, int labels_amount, int width, int height)
{
    FILE *f = fopen(path, "w");

    if (f == NULL) {
        fprintf(stderr, "Erro ao abrir saída\n");
        return;
    }

    fprintf(f, "%d,%d,%d\n", width, height, labels_amount);

    for (int i = 0; i < labels_amount; i++) {

        fprintf(f, "1 %d,%d,%d %d ",
            colors[i].val[0],
            colors[i].val[1],
            colors[i].val[2],
            points_per_label[i]);

        for (int j = 0; j < points_per_label[i]; j++) {
            fprintf(f, "%d,%d ", pieces[i][j].x, pieces[i][j].y);
        }

        fprintf(f, "\n");
    }

    fclose(f);
}

iftVoxel **villiReadKpvFileVec(const char *input_path, int *labels_amount, int **points_per_label, iftColor **colors, int *width, int *height)
{
    FILE *in = fopen(input_path, "r");

    if (in == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo\n");
        return NULL;
    }

    if (fscanf(in, "%d,%d,%d\n", width, height, labels_amount) != 3) {
        fprintf(stderr, "Erro ao ler dimensões\n");
        fclose(in);

        return NULL;
    }

    *points_per_label = malloc(sizeof(int) * (*labels_amount));
    *colors = malloc(sizeof(iftColor) * (*labels_amount));

    if (*points_per_label == NULL || *colors == NULL) {
        fclose(in);

        return NULL;
    }

    iftVoxel **vListPiece = malloc(sizeof(iftVoxel*) * (*labels_amount));

    if (vListPiece == NULL) {
        free(*points_per_label);
        free(*colors);

        fclose(in);

        return NULL;
    }

    char line[65536];

    int index = 0;

    while (fgets(line, sizeof(line), in) != NULL) {

        if (index >= *labels_amount) {
            break;
        }

        int thickness = 0;
        int r = 0;
        int g = 0;
        int b = 0;
        int pointsAmount = 0;
        int consumed = 0;

        char *ptr = line;

        if (sscanf(ptr, "%d %d,%d,%d %d%n", &thickness, &r, &g, &b, &pointsAmount, &consumed) != 5) {
            continue;
        }

        ptr += consumed;

        (*points_per_label)[index] = pointsAmount;

        (*colors)[index].val[0] = r;
        (*colors)[index].val[1] = g;
        (*colors)[index].val[2] = b;

        iftVoxel *curr_voxel = malloc(sizeof(iftVoxel) * pointsAmount);

        if (curr_voxel == NULL) {

            for (int i = 0; i < index; i++) {
                free(vListPiece[i]);
            }

            free(vListPiece);
            free(*points_per_label);
            free(*colors);

            fclose(in);

            return NULL;
        }

        int inner_index = 0;

        while (inner_index < pointsAmount) {

            int x = 0;
            int y = 0;

            if (sscanf(ptr, "%d,%d%n", &x, &y, &consumed) != 2) {
                break;
            }

            curr_voxel[inner_index].x = x;
            curr_voxel[inner_index].y = y;
            curr_voxel[inner_index].z = 0;

            inner_index++;

            ptr += consumed;

            while (*ptr == ' ') {
                ptr++;
            }
        }

        if (inner_index != pointsAmount) {
            free(curr_voxel);
            continue;
        }

        vListPiece[index] = curr_voxel;

        index++;
    }

    fclose(in);

    return vListPiece;
}

void villiToSVG(const char *input_path, const char *output_path)
{
    FILE *in = fopen(input_path, "r");
    FILE *out = fopen(output_path, "w");

    if (!in || !out) {
        fprintf(stderr, "Erro ao abrir arquivos\n");

        if (in) fclose(in);
        if (out) fclose(out);

        return;
    }

    int width, height, nLabels;

    if (fscanf(in, "%d,%d,%d\n",
               &width,
               &height,
               &nLabels) != 3) {

        fprintf(stderr, "Erro ao ler cabeçalho\n");

        fclose(in);
        fclose(out);

        return;
    }

    fprintf(out,
        "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        "width=\"%d\" height=\"%d\" "
        "viewBox=\"0 0 %d %d\">\n",
        width, height,
        width, height);

    char line[65536];

    while (fgets(line, sizeof(line), in)) {

        int thickness;
        int r, g, b;
        int pointsAmount;

        char *ptr = line;

        int consumed = 0;

        if (sscanf(ptr, "%d %d,%d,%d %d%n", &thickness, &r, &g, &b, &pointsAmount, &consumed) != 5) {
            continue;
        }

        ptr += consumed;

        fprintf(out, "<path d=\"");

        int x, y;
        int first = 1;
        int validPoints = 0;

        while (sscanf(ptr, "%d,%d%n", &x, &y,&consumed) == 2) {
            if (first) {
                fprintf(out, "M %d %d ", x, y);
                first = 0;
            } else {
                fprintf(out, "L %d %d ", x, y);
            }

            validPoints++;

            ptr += consumed;

            while (*ptr == ' ')
                ptr++;
        }

        if (validPoints >= 2) {

            fprintf(out,
                "Z\" "
                "stroke=\"rgb(%d,%d,%d)\" "
                "stroke-width=\"1\" "
                "fill=\"rgb(%d,%d,%d)\"/>\n",
                r, g, b,
                r, g, b);

        } else {
            fprintf(out, "\"/>\n");
        }
    }

    fprintf(out, "</svg>\n");

    fclose(in);
    fclose(out);
}