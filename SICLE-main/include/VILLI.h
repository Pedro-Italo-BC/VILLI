#include "ift.h"
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct villiVoxel {
    double x, y , z;
} villiVoxel;

typedef struct VILLI_PIECE_OBJ {
    iftVoxel value;
    int orientation;
    struct VILLI_PIECE_OBJ* next;
} VILLI_PIECE_OBJ;

typedef struct LIST_VILLI_PIECE_OBJ {
    VILLI_PIECE_OBJ *first;
    VILLI_PIECE_OBJ *last;
    iftColor color;
    int v_length;
    int layer;
    int id;
} LIST_VILLI_PIECE_OBJ;

VILLI_PIECE_OBJ* villiCreatePieceObj(iftVoxel value, int orientation);

LIST_VILLI_PIECE_OBJ* villiCreateListPieceObj(iftColor color);

void villiDestructPieceObj(VILLI_PIECE_OBJ* vpobj);

void villiDestructListPieceObj(LIST_VILLI_PIECE_OBJ *lvpobj);

void villiAddPieceObj(LIST_VILLI_PIECE_OBJ* lvpobj, VILLI_PIECE_OBJ* vpobj);

bool isVoxelEquals(iftVoxel v1, iftVoxel v2);

void villiToSVG(const char *input_path, const char *output_path);