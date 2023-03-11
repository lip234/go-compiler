/*
 * Semantic Analyzer
 * Author: Peng Li
 * Instructor: Nicholas M. Boers
 * CMPT 399: Compilers Fall 2020
 */
#ifndef SEMANTIC_ANALYZE_H
#define SEMANTIC_ANALYZE_H

#include "tree.h"
#include "symtab.h"


void symCheckPost(int level, TreeNode *node);

void symCheckPre(int level, TreeNode *node);

void addConst(TreeNode *node);

void addVariable(TreeNode *node);

void addFunction(TreeNode *node);

void processDeclNode(TreeNode *node);

void processClauseNode(TreeNode *node);

void processOperatorNode(TreeNode *node);

void processStatementNode(TreeNode *node);

void verifyBooleanExpression(TreeNode *node);

void verifyIntegerVariable(TreeNode *node);

void verifyIntegerExpression(TreeNode *node);

int strtoi(char *s);
#endif //SEMANTIC_ANALYZE_H
