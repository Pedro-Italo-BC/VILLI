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