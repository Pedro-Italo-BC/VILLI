#include "VILLI.h"
#include <assert.h>
#include <stdlib.h>

VILLI_PIECE_OBJ* villiCreatePieceObj(iftVoxel value) {
    VILLI_PIECE_OBJ* nvpobj =
        (VILLI_PIECE_OBJ*) malloc(sizeof(VILLI_PIECE_OBJ));

    assert(nvpobj != NULL);
    if (nvpobj == NULL) return NULL;

    nvpobj->value = value;
    nvpobj->next = NULL;

    return nvpobj;
}

void villiDestructPieceObj(VILLI_PIECE_OBJ* vpobj) {
    if (vpobj != NULL) {
        free(vpobj);
    }
}

LIST_VILLI_PIECE_OBJ* villiCreateListPieceObj(iftColor color) {
    LIST_VILLI_PIECE_OBJ *nlvpobj =
        (LIST_VILLI_PIECE_OBJ*) malloc(sizeof(LIST_VILLI_PIECE_OBJ));

    assert(nlvpobj != NULL);
    if (nlvpobj == NULL) return NULL;

    nlvpobj->first = NULL;
    nlvpobj->last  = NULL;
    nlvpobj->v_length = 0;
    nlvpobj->color = color;

    return nlvpobj;
}

void villiDestructListPieceObj(LIST_VILLI_PIECE_OBJ *lvpobj) {
    if (lvpobj == NULL) return;

    VILLI_PIECE_OBJ* current = lvpobj->first;

    while (current != NULL) {
        VILLI_PIECE_OBJ* next = current->next;
        free(current);
        current = next;
    }

    free(lvpobj);
}

void villiAddPieceObj(
    LIST_VILLI_PIECE_OBJ* lvpobj,
    VILLI_PIECE_OBJ* vpobj
) {
    if (lvpobj == NULL || vpobj == NULL) return;

    vpobj->next = NULL;

    if (lvpobj->first == NULL) {
        lvpobj->first = vpobj;
        lvpobj->last  = vpobj;
    } else {
        lvpobj->last->next = vpobj;
        lvpobj->last = vpobj;
    }

    lvpobj->v_length++;
}

bool isVoxelEquals(iftVoxel v1, iftVoxel v2) {
    return (v1.x == v2.x &&
            v1.y == v2.y &&
            v1.z == v2.z);
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

        int id = 0;

        if (sscanf(ptr, "%d %d,%d,%d %d %d%n", &thickness, &r, &g, &b, &pointsAmount, &id, &consumed) != 6) {
            continue;
        }

        ptr += consumed;

        fprintf(out, "<!-- %d -->", id);

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