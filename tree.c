/* symtab.h
 * Peng Li
 * CMPT 399 (Fall 2020)
 * This work is derived from code provided in Lab #4: Introduction to lemon
 * Original author: Nicholas M. Boers
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tree.h"

TreeNode *newNode(char *s) {
    char *new_s = malloc(strlen(s) + 1);
    if (new_s == NULL) {
        return NULL;
    }
    strcpy(new_s, s);
    TreeNode *node = calloc(1, sizeof(TreeNode));
    if (node == NULL) {
        free(new_s);
        return NULL;
    }
    node->value = new_s;
    return node;
}

TreeNode *mustNewNode(char *value) {
    TreeNode *node = newNode(value);
    if (node == NULL) {
        fprintf(stderr, "Failed to allocate memory for new node.\n");
        exit(1);
    }
    return node;
}

TreeNode *newOperatorNode(Operator op, char *value) {
    TreeNode *node = mustNewNode(value);
    node->type = OperatorNode;
    node->subType.operator = op;
    return node;
}

TreeNode *newStatementNode(Statement stmt, char *value) {
    TreeNode *node = mustNewNode(value);
    node->type = StatementNode;
    node->subType.stmt = stmt;
    return node;
}

TreeNode *newValueNode(ValueHolder valueHolder, char *value) {
    TreeNode *node = mustNewNode(value);
    node->type = ValueNode;
    node->subType.value = valueHolder;
    return node;
}

TreeNode *newDeclarationNode(Declaration decl, char *value) {
    TreeNode *node = mustNewNode(value);
    node->type = DeclarationNode;
    node->subType.decl = decl;
    return node;
}

TreeNode *newTypeNode(VariableType varType, char *value) {
    TreeNode *node = mustNewNode(value);
    node->type = TypeNode;
    node->subType.varType = varType;
    return node;
}

TreeNode *newClauseNode(Clause clause, char *value) {
    TreeNode *node = mustNewNode(value);
    node->type = ClauseNode;
    node->subType.clause = clause;
    return node;
}

void traverseTree(int level, TreeNode *node,
                  void (*fEnter)(int, TreeNode *),
                  void (*fExit)(int, TreeNode *)) {
    if (node->sibling != NULL) {
        traverseTree(level, node->sibling, fEnter, fExit);
    }

    if (fEnter != NULL)
        fEnter(level, node);

    for (int i = 0; node->child[i] != NULL; i++) {
        traverseTree(level + 1, node->child[i], fEnter, fExit);
    }

    if (fExit != NULL)
        fExit(level, node);
}

static void freeNode(int n, TreeNode *node) {
    free(node->value);
    node->value = NULL;
    if (node->postNodeAssembly != NULL) {
        free(node->postNodeAssembly);
    }
    if (node->table != NULL) {
        symDestroy(node->table);
    }
    node->table = NULL;
    free(node);
}

void destroyTree(TreeNode *root) {
    traverseTree(0, root, NULL, freeNode);
}
