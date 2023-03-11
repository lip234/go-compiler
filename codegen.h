//
// Created by mint on 2020-11-27.
//

#ifndef CGEN_H
#define CGEN_H

#define BREAK_SYMBOL "0__break_to"
#define CONTINUE_SYMBOL "0__continue_to"
#include "tree.h"
void genOp(TreeNode *node);

void genStmt(TreeNode *node);

void genUnaryOp(TreeNode *node);

void genAssignOp(TreeNode *node);

void generateCode(FILE *f, TreeNode *root);

void genReadln(TreeNode *node);

void genExp(TreeNode *node);

void loadVariable(TreeNode *node);

void genBinaryExp(TreeNode *tree);

void genValue(TreeNode *node);

void genVarDecl(TreeNode *node);

void genCodePre(int level, TreeNode *node);

void genCodePost(int level, TreeNode *node);

void genPrintln(TreeNode *node);

void genConst(int level, TreeNode *node);

void genBooleanExp(TreeNode *node);

void genIfStmt(TreeNode *node);

void genForStmt(TreeNode *node);

void genSwitchStmt(TreeNode *Node);

#endif //CGEN_H
