#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ift.h"

typedef struct VILLI_PIECE_OBJ {
    iftVoxel value;
    struct VILLI_PIECE_OBJ* next;
} VILLI_PIECE_OBJ;

typedef struct LIST_VILLI_PIECE_OBJ {
    VILLI_PIECE_OBJ *first;
    VILLI_PIECE_OBJ *last;
    int v_length;
    iftColor color;
} LIST_VILLI_PIECE_OBJ;


VILLI_PIECE_OBJ* villiCreatePieceObj(iftVoxel value);

LIST_VILLI_PIECE_OBJ* villiCreateListPieceObj(iftColor color);

void villiDestructPieceObj(VILLI_PIECE_OBJ* vpobj);

void villiDestructListPieceObj(LIST_VILLI_PIECE_OBJ *lvpobj);

void villiAddPieceObj(LIST_VILLI_PIECE_OBJ* lvpobj, VILLI_PIECE_OBJ* vpobj);

bool isVoxelEquals(iftVoxel v1, iftVoxel v2);