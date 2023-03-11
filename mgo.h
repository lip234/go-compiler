//
// Created by mint on 2020-11-23.
//

#ifndef SEMANTIC_MGO_H
#define SEMANTIC_MGO_H
#include <stdio.h>
#include "tree.h"

void *ParseAlloc(void *(*malloc)(size_t));

void ParseFree(void *pParser, void(*free)(void *));

void Parse(void *pParser, int tokenCode, char *token);

TreeNode *get_root();

const char *const *get_token_names();

typedef struct LLNode {
    char *value;
    struct LLNode *next;
} LList;

#endif //SEMANTIC_MGO_H
