/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Praneeth Bhandaru                                          */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

struct SymTableNode
{
    const char *pcKey;

    void *pvValue;

    struct SymTableNode *psNextNode;
};

/*--------------------------------------------------------------------*/

struct SymTable
{
    struct SymTableNode *psFirstNode;
    size_t symTableLength;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL)
        return NULL;

    oSymTable->psFirstNode = NULL;
    oSymTable->symTableLength = 0;
    return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable)
{
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;

    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        free(psCurrentNode);
    }

    free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable)
{
    return oSymTable->symTableLength;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
const void *pvValue)
{
    struct SymTableNode *psNewNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
    if (psNewNode == NULL)
        return 0;

    if (SymTable_contains(oSymTable, pcKey)) {
        free(psNewNode);
        return 0;
    }

    // Allocate memory for the new key and copy the contents
    psNewNode->pcKey = (char*)malloc(strlen(pcKey) + 1);
    if (psNewNode->pcKey == NULL) {
        free(psNewNode);
        return 0;
    }
    strcpy((char*)psNewNode->pcKey, pcKey);

    psNewNode->pvValue = (void *)pvValue;
    psNewNode->psNextNode = oSymTable->psFirstNode;
    oSymTable->psFirstNode = psNewNode;
    oSymTable->symTableLength++;

    return 1;
}


/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue)
{
    struct SymTableNode *psTempNode;
    void *pvPrevValue;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /* assert(pvValue != NULL); */

    psTempNode = oSymTable->psFirstNode;

    while (psTempNode != NULL) {
        if (!strcmp(psTempNode->pcKey, pcKey)) {
            pvPrevValue = psTempNode->pvValue;
            psTempNode->pvValue = (void *)pvValue;
            return pvPrevValue;
        }
        psTempNode = psTempNode->psNextNode;
    }

    return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
    struct SymTableNode *psTempNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    psTempNode = oSymTable->psFirstNode;

    while (psTempNode != NULL) {
        if (!strcmp(psTempNode->pcKey, pcKey)) {
            return 1;
        }
        psTempNode = psTempNode->psNextNode;
    }

    return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    struct SymTableNode *psTempNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    psTempNode = oSymTable->psFirstNode;

    while (psTempNode != NULL) {
        if (!strcmp(psTempNode->pcKey, pcKey)) {
            return psTempNode->pvValue;
        }
        psTempNode = psTempNode->psNextNode;
    }

    return NULL;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
    struct SymTableNode *psTempNode, *psPrevNode;
    void *pvPrevValue;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    psTempNode = oSymTable->psFirstNode;
    psPrevNode = NULL;  // Initialize psPrevNode to NULL

    while (psTempNode != NULL) {
        if (!strcmp(psTempNode->pcKey, pcKey)) {
            pvPrevValue = psTempNode->pvValue;

            if (psPrevNode == NULL) {
                oSymTable->psFirstNode = psTempNode->psNextNode;
            } else {
                psPrevNode->psNextNode = psTempNode->psNextNode;
            }

            free((void *)psTempNode->pcKey);
            free(psTempNode);

            oSymTable->symTableLength--;
            return pvPrevValue;
        }
        psPrevNode = psTempNode;
        psTempNode = psTempNode->psNextNode;
    }

    return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra)
{
    struct SymTableNode *psCurrentNode;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
    psCurrentNode != NULL;
    psCurrentNode = psCurrentNode->psNextNode)
        (*pfApply)((void*)psCurrentNode->pcKey,
                (void *)psCurrentNode->pvValue, (void*)pvExtra);
}

/*--------------------------------------------------------------------*/