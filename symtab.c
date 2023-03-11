/*
 * Semantic Analyzer
 * Author: Peng Li
 * Instructor: Nicholas M. Boers
 * CMPT 399: Compilers Fall 2020
 */

#include "symtab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * CRC32 checksum calculation derived from crc32.c authored by Gary S. Brown
 * Original work may be accessed at https://web.mit.edu/freebsd/head/sys/libkern/crc32.c
 */
int hash(char *s, int size) {
    uint32_t crc32 = 0xFFFFFFFF;
    for (int i = 0; s[i] != '\0'; i++) {
        crc32 = (crc32 >> 8) ^ crc32Table[(crc32 ^ s[i]) & 0xff];
    }
    crc32 = crc32 ^ 0xffffffff;
    return crc32 & (size - 1);
}

SymTable *symInit() {
    return calloc(1, sizeof(SymTable));
}

SymTable *symNew(SymTable *parent) {
    // unique ID for each symbol table we create
    static int symID = 0;
    SymTable *table = calloc(1, sizeof(SymTable));
    table->ID = ++symID;
    if (table == NULL) {
        fprintf(stderr, "Unable to allocate memory for new symble table.");
        exit(1);
    }
    table->parent = parent;
    return table;
}

Symbol *symInsert(SymTable *table, char *name) {
    Symbol *symbol = calloc(1, sizeof(Symbol));
    int index = hash(name, TABLE_SIZE);
    symbol->name = strdup(name);
    symbol->next = table->symbols[index];
    table->symbols[index] = symbol;
    return symbol;
}

Symbol *symLookup(SymTable *table, char *name) {
    int h = hash(name, TABLE_SIZE);
    Symbol *symbol = table->symbols[h];
    while (symbol != NULL) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    if (table->parent != NULL) {
        return symLookup(table->parent, name);
    }
    return NULL;
}

void freeBucket(Symbol *symbol) {
    free(symbol->name);
    if (symbol->value != NULL) {
        free(symbol->value);
    }
    free(symbol);
}

void symDestroy(SymTable *table) {
    Symbol *symbol, *next;
    for (int i = 0; i < TABLE_SIZE; i++) {
        // skip empty slots
        if (table->symbols[i] == NULL) {
            continue;
        }

        symbol = table->symbols[i];
        while (symbol != NULL) {
            next = symbol->next;
            freeBucket(symbol);
            symbol = next;
        }
    }
    free(table);
}

void symDelete(SymTable *table, char *name) {
    int h = hash(name, TABLE_SIZE);
    Symbol *symbol = table->symbols[h];
    if (symbol == NULL) {
        return;
    }
    if (strcmp(symbol->name, name) == 0) {
        table->symbols[h] = symbol->next;
        freeBucket(symbol);
    } else {
        // there might be collision, try to find the symbol who's next is a match
        while (symbol->next != NULL && strcmp(symbol->next->name, name) != 0) {
            symbol = symbol->next;
        }
        // Confirm found
        if (symbol->next != NULL && strcmp(symbol->next->name, name) == 0) {
            Symbol *next = symbol->next->next;
            freeBucket(symbol->next);
            symbol->next = next;
        }
    }
}

