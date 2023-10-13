# Macros
CC = gcc
# CC = gcc217
# CC = gcc217m

# Dependency rules for non-file targets
all: testsymtablelist testsymtablehash
clean:
	rm -f testsymtablelist testsymtablehash *.o meminfo*

# Dependency rules for file targets
testsymtablelist: testsymtable.o symtablelist.o
	$(CC) testsymtable.o symtablelist.o -o testsymtablelist

testsymtablehash: testsymtable.o symtablehash.o
	$(CC) testsymtable.o symtablehash.o -o testsymtablehash

testsymtable.o: testsymtable.c
	$(CC) -c testsymtable.c

symtablelist.o: symtablelist.c
	$(CC) -c symtablelist.c

symtablehash.o: symtablehash.c
	$(CC) -c symtablehash.c