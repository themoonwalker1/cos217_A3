/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Praneeth Bhandaru                                          */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include "symtable.h"

static const size_t bucket_sizes[9] =
        {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521, 0};

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
    struct SymTableNode **ppsFirstNode;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL)
        return NULL;

    ppsFirstNode = (struct SymTableNode **)malloc(
            sizeof(struct SymTableNode *) * bucket_sizes[0]);
    if (ppsFirstNode == NULL)
    {
        free(oSymTable);
        return NULL;
    }

    oSymTable->ppsFirstNode = ppsFirstNode;
    oSymTable->buckets = bucket_sizes[0];
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

/* Expands oSymTable to the next number of buckets, to a maximum of
   65521 buckets. Returns 0 for an unsuccessful expansion (oSymTable is
   null or memory allocation failed) or Returns 1 for successful
   expansion. Returns 0 if maximum bucket size has been reached. */
static int SymTable_expand(SymTable_T oSymTable)
{
    struct SymTableNode **ppsNewBucketArray;
    struct SymTableNode *psTempOldNode, *psTempNewNode, *psTempNextNode;
    size_t *bucket_size;
    size_t i;
    size_t hash;

    assert(oSymTable != NULL);

    bucket_size = (size_t *)&(bucket_sizes[0]);
    while (*bucket_size != oSymTable->buckets)
        bucket_size++;

    bucket_size++;

    if (*bucket_size == 0)
        return 1;

    ppsNewBucketArray = (struct SymTableNode **)malloc(
            sizeof(struct SymTableNode *) * *bucket_size);

    if (ppsNewBucketArray == NULL)
        return 0;

    for (i = (size_t)0; i < *bucket_size; i++)
    {
        *(ppsNewBucketArray + i) = NULL;
    }

    for (i = (size_t)0; i < oSymTable->buckets; i++)
    {
        psTempOldNode = *(oSymTable->ppsFirstNode + i);
        while (psTempOldNode != NULL){
            hash = SymTable_hash(psTempOldNode->pcKey, *bucket_size);

            psTempNextNode = psTempOldNode->psNextNode;

            psTempNewNode = *(ppsNewBucketArray + hash);
            *(ppsNewBucketArray + hash) = psTempOldNode;
            (*(ppsNewBucketArray + hash))->psNextNode = psTempNewNode;

            psTempOldNode = psTempNextNode;
        }
    }

    free(oSymTable->ppsFirstNode);
    oSymTable->ppsFirstNode = ppsNewBucketArray;
    oSymTable->buckets = *bucket_size;

    return 1;
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

    if (oSymTable->buckets == oSymTable->symTableLength)
    {
        if (!SymTable_expand(oSymTable))
            return 0;
    }

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