/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Praneeth Bhandaru                                          */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <string.h>
#include <stdlib.h>

/*--------------------------------------------------------------------*/

/* A SymTable is a collection of unique key value pairs (bindings). */
typedef struct SymTable *SymTable_T;

/* Create, initialize, and return a new and empty SymTable_T object, or
   return NULL if insufficient memory is available. */
SymTable_T SymTable_new(void);

/* Frees all memory occupied by oSymTable.
   Precondition: oSymTable is non-null. */
void SymTable_free(SymTable_T oSymTable);

/* Return the number of bindings in oSymTable.
   Precondition: oSymTable is non-null. */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Adds new binding to oSymTable with the key pcKey and value pvValue.
   If key already exists, oSymTable is unchanged and returns 0. If
   insufficient memory is available, oSymTable is unchanged and returns
   0. If a new binding is successfully added, the function returns 1.
   Precondition: oSymTable and pcKey are non-null. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
const void *pvValue);

/* Replaces the value of a binding in oSymTable that has the key pcKey
   with pvValue. Returns the previous value of the binding. If a
   binding with key pcKey does not exist, oSymTable is unchanged and
   returns NULL.
   Precondition: oSymTable and pcKey are non-null. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue);

/* Returns 1 if binding with key pcKey exists in oSymTable. Returns 0
   if such binding does not exist.
   Precondition: oSymtable and pcKey are non-null. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Returns the value of a binding in oSymTable the has the key pcKey.
   If no such binding exists, returns NULL.
   Precondition: oSymtable and pcKey are non-null. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Removes the binding in oSymTable with key pcKey. Returns the value of
   that binding, and frees allocated memory. If no such binding exists,
   oSymTable is unchanged and NULL is returned.
   Precondition: oSymtable and pcKey are non-null.  */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Applies function *pfApply to each binding in oSymTable, with pvExtra
   as an extra parameter for the function.
   Precondition: oSymtable and pfApply are non-null. */
void SymTable_map(SymTable_T oSymTable,
void (*pfApply) (const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra);

#endif

/*--------------------------------------------------------------------*/