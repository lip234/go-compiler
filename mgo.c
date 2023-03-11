/*
 * Author: Peng Li
 * Student ID: 3012769
 * MacEwan University CMPT 399: Compilers Fall 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "parser.h"
#include "lexer.h"
#include "tree.h"
#include "analyze.h"
#include "mgo.h"
#include "codegen.h"

void print(int level, TreeNode *t) {
    printf("%*s%s\n", level * 4, " ", t->value);
    if (t->table != NULL) {
        printf("%*sSym Table: %d\n", level * 4, " ", t->table->ID);
    }
}

void printSymTable(int level, TreeNode *node) {
    Symbol *symbol;
    if (node->table != NULL) {
        printf("Symol Table: %d\n", node->table->ID);
        printf("Variable\t\tType\t\tDefined at line\n");
        for (int i = 0; i < TABLE_SIZE; i++) {
            symbol = node->table->symbols[i];
            while (symbol != NULL) {
                printf("%s\t\t", symbol->name);
                switch (symbol->type) {
                    case CONST_STRING:
                        printf("String const");
                        break;
                    case CONST_INTEGER:
                        printf("Integer const");
                        break;
                    case VARIABLE_INTEGER:
                        printf("Integer");
                        break;
                    case VARIABLE_INTEGER_ARRAY:
                        printf("Integer Array");
                        break;
                    case FUNCTION:
                        printf("Function");
                        break;
                }
                printf("\t\t%d\n", symbol->lineno);
                symbol = symbol->next;
            }
        }
        printf("\n");
    }
}

/* Prints the help message then exits */
void print_help(char *argv0) {
    printf("usage: %s [options] file\n\n"
           "Compile programs written in the MacEwan Go programming language.\n\n"
           "Options: \n"
           "\t-h     display this help and exit\n"
           "\t-v     display extra (verbose) debugging information\n", argv0);
    exit(0);
}

int main(int argc, char *argv[]) {
    LList *head = NULL;
    int opt, token, verbose = 0;
    char *filename = NULL;
    char *out_filename = NULL;
    FILE *fp;
    const char *const *token_names = get_token_names();

    while ((opt = getopt(argc, argv, "hvo:")) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
            case 'v':
                verbose++;
                break;
            case 'o':
                out_filename = strdup(optarg);
                break;
            default:
                print_help(argv[0]);
                exit(1);
        }
    }
    /* There needs to be one positional argument for filename */
    if (optind + 1 == argc) {
        filename = argv[optind];
    } else {
        print_help(argv[0]);
        exit(1);
    }

    if (out_filename == NULL) {
        char *dot_pos = strrchr(filename, '.');
        if (dot_pos == NULL || dot_pos == filename) {
            // if the input file name does not contain dot, simply add .s to the end
            out_filename = calloc(strlen(filename) + 3, sizeof(char));
            if (out_filename == NULL) {
                fprintf(stderr, "Unable to allocate memory.\n");
                exit(1);
            }
            sprintf(out_filename, "%s.s", filename);
        } else {
            // extra 3 bytes just to be safe
            out_filename = calloc(strlen(filename) + 3, sizeof(char));
            if (out_filename == NULL) {
                fprintf(stderr, "Unable to allocate memory.\n");
                exit(1);
            }
            snprintf(out_filename, dot_pos - filename +1, "%s", filename);
            strcat(out_filename, ".s");
        }
    }

    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Unable to open file %s, error: %s\n", filename,
                strerror(errno));
        return 1;
    }
    yyin = fp;
    void *parser = ParseAlloc(malloc);

    do {
        token = getToken();
        if (token == T_ERROR) {
            printf("Illegal token %s at line %d\n", yytext, yylineno);
            exit(-1);
        }
        /* As suggested in the parser feedback, it is possible to free the duplicated strings
         * in parser.y, however, I personally prefer having alloc and free in the same file
         * therefore all the duplicates are still kept in the linked-list to be freed later
         */
        char *yytextCopy = strdup(yytext);
        LList *new_head = calloc(1, sizeof(LList));
        new_head->next = head;
        new_head->value = yytextCopy;
        head = new_head;

        if (verbose > 2) {
            printf("%d, %s, %s\n", yylineno, token_names[token], yytext);
        }
        Parse(parser, token, yytextCopy);


    } while (token != 0);
    TreeNode *root = get_root();
    // free the duplicated strings mentioned earlier
    while (head != NULL) {
        LList *tmp = head->next;
        free(head->value);
        free(head);
        head = tmp;
    }

    if (verbose > 1) {
        traverseTree(0, root, print, NULL);
    }

    traverseTree(0, root, symCheckPre, symCheckPost);

    if (verbose > 0) {
        traverseTree(0, root, print, NULL);
        traverseTree(0, root, printSymTable, NULL);
    }

    FILE *output = fopen(out_filename, "w");
    generateCode(output, root);
    yylex_destroy();
    ParseFree(parser, free);
    destroyTree(root);
    fclose(fp);
    fclose(output);
    printf("Compiled successfully\n"
           "\n To build binary executable, plaese run the following commands:\n"
           "make io.o\n"
           "nasm -f elf32 -o program.o %s\n"
           "gcc -m32 program.o io.o\n", out_filename);

    free(out_filename);
    return 0;
}
