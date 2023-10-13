/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Praneeth Bhandaru                                          */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each binding in a Symtable is stored as a SymTableNode. SymTableNodes
   are linked to each other to form a linked list structure. */
struct SymTableNode
{
    /* Unique String Key */
    const char *pcKey;

    /* Binding's Value */
    void *pvValue;

    /* Pointed to the next SymTableNode in linked list */
    struct SymTableNode *psNextNode;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a hash table implementation of a symbol table that
   points to hash buckets containing bindings and stores the number of
   bindings and buckets . */
struct SymTable
{
    /* Pointer to the first hash bucket */
    struct SymTableNode **ppsFirstNode;

    /* Number of Bindings */
    size_t symTableLength;

    /* Number of Buckets */
    size_t buckets;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
    SymTable_T oSymTable;
    size_t i;
    const int INITIALIZATION_BUCKETS = 509;
    struct SymTableNode **ppsFirstNode;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL)
        return NULL;

    *ppsFirstNode =
        malloc(sizeof(struct SymTableNode *)*INITIALIZATION_BUCKETS);
    if (ppsFirstNode == NULL)
    {
        free(oSymTable);
        return NULL;
    }

    oSymTable->ppsFirstNode = ppsFirstNode;
    oSymTable->buckets = INITIALIZATION_BUCKETS;
    oSymTable->symTableLength = 0;

    for (i = (size_t)0; i < oSymTable->buckets; i++)
    {
        *(oSymTable->ppsFirstNode + i) = NULL;
    }

    return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable)
{
    size_t i;

    assert(oSymTable != NULL);

    for (i = (size_t)0; i < oSymTable->buckets; i++) {
        struct SymTableNode *psCurrentNode = oSymTable->ppsFirstNode[i];
        struct SymTableNode *psNextNode;

        while (psCurrentNode != NULL) {
            psNextNode = psCurrentNode->psNextNode;
            free((void *)psCurrentNode->pcKey);
            free(psCurrentNode);
            psCurrentNode = psNextNode;
        }
    }

    free(oSymTable->ppsFirstNode);
    free(oSymTable);
}


/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable)
{
    return oSymTable->symTableLength;
}

/*--------------------------------------------------------------------*/


/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
const void *pvValue)
{
    struct SymTableNode *psNewNode;
    size_t hash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (SymTable_contains(oSymTable, pcKey))
        return 0;

    hash = SymTable_hash(pcKey, oSymTable->buckets);

    psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
    if (psNewNode == NULL)
        return 0;

    psNewNode->pcKey = (char*)malloc(strlen(pcKey) + 1);
    if (psNewNode->pcKey == NULL) {
        free(psNewNode);
        return 0;
    }

    strcpy((char*)psNewNode->pcKey, pcKey);

    psNewNode->pvValue = (void *)pvValue;

    psNewNode->psNextNode = *(oSymTable->ppsFirstNode + hash);
    *(oSymTable->ppsFirstNode + hash) = psNewNode;

    oSymTable->symTableLength++;

    return 1;
}



/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue)
{
    struct SymTableNode *psTempNode;
    void *pvPrevValue;
    size_t hash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /* assert(pvValue != NULL); */

    hash = SymTable_hash(pcKey, oSymTable->buckets);
    psTempNode = *(oSymTable->ppsFirstNode + hash);

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
    size_t hash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    hash = SymTable_hash(pcKey, oSymTable->buckets);
    psTempNode = *(oSymTable->ppsFirstNode + hash);

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
    size_t hash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    hash = SymTable_hash(pcKey, oSymTable->buckets);
    psTempNode = *(oSymTable->ppsFirstNode + hash);

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
    size_t hash;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    hash = SymTable_hash(pcKey, oSymTable->buckets);
    psTempNode = *(oSymTable->ppsFirstNode + hash);

    psPrevNode = NULL;

    while (psTempNode != NULL) {
        if (!strcmp(psTempNode->pcKey, pcKey)) {
            pvPrevValue = psTempNode->pvValue;

            if (psPrevNode == NULL) {
                *(oSymTable->ppsFirstNode + hash)
                    = psTempNode->psNextNode;
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
    size_t i;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    for (i = (size_t)0; i < oSymTable->buckets; i++)
    {
        for (psCurrentNode = *(oSymTable->ppsFirstNode + i);
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
        {
            (*pfApply)((void*)psCurrentNode->pcKey,
            (void *)psCurrentNode->pvValue, (void*)pvExtra);
        }
    }
}

/*--------------------------------------------------------------------*/