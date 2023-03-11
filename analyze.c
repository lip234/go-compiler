/*
 * Semantic Analyzer
 * Author: Peng Li
 * Instructor: Nicholas M. Boers
 * CMPT 399: Compilers Fall 2020
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "analyze.h"
#include "tree.h"

#define output stderr

static SymTable *currentScope;

void symCheckPre(int level, TreeNode *node) {
    switch (node->type) {
        case DeclarationNode:
            processDeclNode(node);
            break;
        case OperatorNode:
            processOperatorNode(node);
            break;
        case StatementNode:
            processStatementNode(node);
            break;
        case ClauseNode:
            processClauseNode(node);
            break;
        default:
            break;
    }
}

void symCheckPost(int level, TreeNode *node) {
    // reset the current scope upon exiting a block node
    // Note that in processClauseNode function, symbol table
    // is only created for blocks with variable declarations.
    if (node->type == ClauseNode && node->subType.clause == BLOCK_CLAUSE
        && is_var_decl(node)) {
        currentScope = currentScope->parent;
    }
}

void processClauseNode(TreeNode *node) {
    TreeNode *constSpec;
    Symbol *symbol;
    switch (node->subType.clause) {
        case CASE_CLAUSE:
            constSpec = node->child[0];
            while (constSpec != NULL) {
                if (constSpec->type == ValueNode && constSpec->subType.value == NUM_LIT) {
                    constSpec = constSpec->sibling;
                    continue;
                }
                symbol = symLookup(currentScope, constSpec->value);
                if (symbol == NULL) {
                    fprintf(stderr, "syntax error: undefined variable `%s` at line %d\n",
                            constSpec->value, constSpec->lineno);
                    exit(1);
                }
                if (symbol->type == CONST_STRING) {
                    fprintf(stderr, "syntax error: wrong type expected integer, got string at line %d\n",
                            constSpec->lineno);
                    exit(1);
                }
                if (symbol->type == VARIABLE_INTEGER_ARRAY) {
                    verifyIntegerExpression(node->child[0]);
                }
                constSpec = constSpec->sibling;
            }

        case BLOCK_CLAUSE:
            // Create symbol table only for blocks with variable declarations,
            // in such case, the constant declarations, if exists will be added
            // to the parent scope, declarations are processed in processDeclNode function
            if (is_var_decl(node->child[1])) {
                currentScope = symNew(currentScope);
                node->table = currentScope;
            }
        default:
            break;
    }
}

void processStatementNode(TreeNode *node) {
    switch (node->subType.stmt) {
        case IF_STMT:
        case FOR_STMT:
            verifyBooleanExpression(node->child[0]);
            break;
        case SWITCH_STMT:
            verifyIntegerExpression(node->child[0]);
            break;
        case READLN_STMT: {
            TreeNode *arg = node->child[0];
            while (arg != NULL) {
                verifyIntegerVariable(arg);
                arg = arg->sibling;
            }
            break;
        }
        case PRINTLN_STMT: {
            TreeNode *arg = node->child[0];
            Symbol *symbol;
            while (arg != NULL) {
                if (is_bin_arith_node(arg)) {
                    verifyIntegerExpression(arg);
                } else if (is_id_node(arg)) {
                    symbol = symLookup(currentScope, arg->value);
                    if (symbol == NULL || symbol->type == FUNCTION) {
                        fprintf(output, "syntax error: reference to undefined variable `%s` at line %d\n",
                                arg->value, arg->lineno);
                        exit(1);
                    }
                }
                arg = arg->sibling;
            }
        }
        default:
            break;
    }
}

void processDeclNode(TreeNode *node) {
    switch (node->subType.decl) {
        case TOP_LEVEL_DECL:
            currentScope = symNew(currentScope);
            node->table = currentScope;
            break;
        case CONST_DECL:
            addConst(node);
            break;
        case VAR_DECL:
            addVariable(node);
            break;
        case FUNC_DECL:
            addFunction(node);
            break;
    }
}

void addConst(TreeNode *node) {
    static int nameID = 0;
    if (node == NULL) {
        return;
    }
    if (!is_const_decl(node)) {
        return;
    }

    Symbol *symbol;
    TreeNode *lhs, *rhs;
    lhs = node->child[0];
    rhs = node->child[1];
    if (currentScope == NULL) {
        fprintf(output, "syntax error: no scope found for constant `%s` at line %d\n",
                node->value, node->lineno);
        return;
    }
    // Keep renaming constants until no conflict
    while (symLookup(currentScope, lhs->value) != NULL) {
        char newName[strlen(lhs->value) + 20];
        sprintf(newName, "%s_RENAMED_%d", lhs->value, ++nameID);
        free(lhs->value);
        lhs->value = strdup(newName);
    }

    symbol = symInsert(currentScope, lhs->value);
    symbol->lineno = lhs->lineno;
    // parser ensures the rhs is a ValueHolder node defined in tree.h
    // so we only need to check what type of ValueHolder it is
    if (rhs->subType.value == STR_LIT) {
        symbol->type = CONST_STRING;
    } else {
        // rhs is a integer literal
        symbol->type = CONST_INTEGER;
        symbol->ivalue = strtoi(rhs->value);
    }
}

void addVariable(TreeNode *node) {
    if (node == NULL) {
        return;
    }

    if (node->type != DeclarationNode || node->subType.decl != VAR_DECL ||
        node->child[0] == NULL) {
        return;
    }

    Symbol *symbol;
    TreeNode *lhs, *rhs;
    lhs = node->child[0];
    rhs = node->child[1];
    if (currentScope == NULL) {
        fprintf(output, "syntax error: no scope found for variable `%s` at line %d\n",
                node->value, node->lineno);
        exit(1);
    }
    symbol = symLookup(currentScope, lhs->value);
    if (symbol != NULL) {
        fprintf(output, "syntax error: redefinition of symbol `%s` at line %d. "
                        "Prevously defined at line %d.\n",
                lhs->value, lhs->lineno, lhs->lineno);
        exit(1);
    }

    symbol = symInsert(currentScope, lhs->value);
    symbol->lineno = lhs->lineno;
    if (rhs->subType.varType == INT_TYPE) {
        symbol->type = VARIABLE_INTEGER;
    } else if (rhs->subType.varType == ARRAY_TYPE) {
        Symbol *symLen;
        if (rhs->child[1]->subType.value == IDENTIFIER) {
            symLen = symLookup(currentScope, rhs->child[1]->value);
            if (symLen == NULL) {
                fprintf(output, "syntax error: array length referenced to undefined constant `%s`\n",
                        rhs->child[1]->value);
                exit(1);
            }
            if (symLen->type == CONST_STRING) {
                fprintf(output, "syntax error: array length referenced to string constant `%s`\n",
                        rhs->child[1]->value);
                exit(1);
            }
            if (symLen->type == VARIABLE_INTEGER || symLen->type == VARIABLE_INTEGER_ARRAY) {
                fprintf(output, "syntax error: array length referenced to variable `%s`\n",
                        rhs->child[1]->value);
                exit(1);
            }
            symbol->array_length = symLen->ivalue;
        } else if (rhs->child[1]->subType.value == NUM_LIT) {
            symbol->array_length = strtoi(rhs->child[1]->value);
        } else {
            fprintf(output, "syntax error: illegal array length `%s`\n",
                    rhs->child[1]->value);
            exit(1);
        }
        symbol->type = VARIABLE_INTEGER_ARRAY;
    } else {
        fprintf(output, "syntax error: invalid type in variable declaration `%s` at line %d\n",
                lhs->value, node->lineno);
        exit(1);
    }
}

void addFunction(TreeNode *node) {
    Symbol *symbol;
    TreeNode *lhs;
    lhs = node->child[0];
    if (currentScope == NULL) {
        fprintf(output, "syntax error: no scope found for function `%s` at line %d\n",
                lhs->value, node->lineno);
        exit(1);
    }

    // function must be declared at top level scope
    if (currentScope->parent != NULL) {
        fprintf(output, "syntax error: invalid scope for function `%s` at line %d\n",
                lhs->value, node->lineno);
        exit(1);
    }
    symbol = symLookup(currentScope, lhs->value);
    if (symbol != NULL) {
        fprintf(output, "syntax error: redefinition of symbol `%s` at line %d. "
                        "Prevously defined at line %d.\n",
                node->value, node->lineno, symbol->lineno);
        exit(1);
    }
    symbol = symInsert(currentScope, lhs->value);
    symbol->type = FUNCTION;
    symbol->lineno = lhs->lineno;
}

void verifyAssignment(TreeNode *node) {
    // we can only assign integer value to an integer variable or to a cell of an integer array
    // also note that the parser already ensures the lhs is a variable
    TreeNode *lhs = node->child[0];
    TreeNode *rhs = node->child[1];
    verifyIntegerVariable(lhs);
    verifyIntegerExpression(rhs);
}


void verifyIntegerVariable(TreeNode *node) {
    Symbol *symbol = symLookup(currentScope, node->value);
    if (symbol == NULL) {
        fprintf(output, "syntax error: reference to undefined variable `%s` at line %d.\n",
                node->value, node->lineno);
        exit(1);
    }
    if (symbol->type == CONST_INTEGER || symbol->type == CONST_STRING) {
        fprintf(output, "syntax error: assignment to constant `%s` at line %d\n",
                node->value, node->lineno);
        exit(1);
    }
    switch (symbol->type) {
        case VARIABLE_INTEGER:
            if (node->child[0] != NULL) {
                fprintf(output, "syntax error: cannot index on integer variable `%s` at line %d\n",
                        node->value, node->lineno);
                exit(1);
            }
            return;
        case VARIABLE_INTEGER_ARRAY:
            if (node->child[0] == NULL) {
                fprintf(output, "syntax error: cannot assign array directly, an index must be specified line %d",
                        node->lineno);
                exit(1);
            }
            verifyIntegerExpression(node->child[0]);
            return;
        default:
            break;
    }
}

void verifyIntegerExpression(TreeNode *node) {
    switch (node->type) {
        case ValueNode:
            if (node->subType.value == IDENTIFIER) {
                // Check if an identifier can produce integer
                Symbol *symbol = symLookup(currentScope, node->value);
                if (symbol == NULL) {
                    fprintf(output, "syntax error: reference to undefined variable `%s` at line %d.\n",
                            node->value, node->lineno);
                    exit(1);
                }

                if (symbol->type == VARIABLE_INTEGER ||
                    symbol->type == CONST_INTEGER) {
                    return;
                }
                // for array, its index must be a valid integer
                if (symbol->type == VARIABLE_INTEGER_ARRAY) {
                    verifyIntegerExpression(node->child[0]);
                    return;
                }
            }
            if (node->subType.value == NUM_LIT) {
                return;
            }
        case OperatorNode:
            switch (node->subType.operator) {
                // for unary operators, its sole operand must be integer
                case POSITIVE_OP:
                case NEGATIVE_OP:
                    verifyIntegerExpression(node->child[0]);
                    return;
                    // for binary operators, both operands must be integers
                case ADD_OP:
                case SUBTRACT_OP:
                case MUL_OP:
                case DIV_OP:
                case MOD_OP:
                case LSHIFT_OP:
                case RSHIFT_OP:
                    verifyIntegerExpression(node->child[0]);
                    verifyIntegerExpression(node->child[1]);
                    return;
                default:
                    break;
            }
        default:
            break;
    }
    fprintf(output, "syntax error: invalid Integer expression at line %d\n", node->lineno);
}

void verifyBooleanExpression(TreeNode *node) {
    if (node->type == OperatorNode) {
        switch (node->subType.operator) {
            case EQ_OP:
            case NE_OP:
            case LT_OP:
            case LE_OP:
            case GT_OP:
            case GE_OP:
                verifyIntegerExpression(node->child[0]);
                verifyIntegerExpression(node->child[1]);
                return;

            case OR_OP:
            case AND_OP:
                verifyBooleanExpression(node->child[0]);
                verifyBooleanExpression(node->child[1]);
                return;
            default:
                break;
        }
    }
    fprintf(output, "syntax error: invalid boolean expression near at line %d\n", node->lineno);
    exit(1);
}

void processOperatorNode(TreeNode *node) {
    switch (node->subType.operator) {
        case ASSIGN_OP:
        case SUBASSIGN_OP:
        case ADDASSIGN_OP:
            verifyAssignment(node);
            break;

        case POSITIVE_OP:
        case NEGATIVE_OP:
            verifyIntegerExpression(node->child[0]);
            break;

        case OR_OP:
        case AND_OP:
            verifyBooleanExpression(node->child[0]);
            verifyBooleanExpression(node->child[1]);
            break;

        default:
            verifyIntegerExpression(node->child[0]);
            verifyIntegerExpression(node->child[1]);
            break;
    }
}

int strtoi(char *s) {
    return (int) strtol(s, NULL, 10);
}
