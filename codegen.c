#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "string.h"

static FILE *output;

static SymTable *currentScope;
static int stackSize;

void *mustCalloc(size_t n, size_t size) {
    void *p = calloc(n, size);
    if (p == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(1);
    }
    return p;
}

/* Concatenate 2 strings into newly allocated memory */
char *concatStr(char *s1, char *s2) {
    char *result = mustCalloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
    sprintf(result, "%s%s", s1, s2);
    return result;
}

void generateCode(FILE *f, TreeNode *root) {
    output = f;

    // For simplicity and performance, there will be only one stack frame which grows as needed
    fprintf(output, "extern println, readln\n"
                    "global main\n"
                    "\n"
                    "section .text\n"
                    "main:\n"
                    "push ebp\n"
                    "mov ebp, esp\n");
    traverseTree(0, root, genCodePre, genCodePost);
    fprintf(output, "mov esp, ebp\n"
                    "pop ebp\n"
                    "mov eax, 0\n"
                    "ret\n"
                    "INDEX_OUT_OF_BOUNDS:\n"
                    "push RUNTIME_OUT_OF_BOUNDS\n"
                    "push dword 1\n"
                    "push dword 2\n"
                    "call println\n"
                    "add esp, 12\n"
                    "mov esp, ebp\n"
                    "pop ebp\n"
                    "mov eax, 1\n"
                    "ret\n"
                    "section .rodata\n"
                    "RUNTIME_OUT_OF_BOUNDS: db \"Runtime error: array index out of bounds\", 0\n");
    traverseTree(0, root, genConst, NULL);
}

void genConst(int level, TreeNode *node) {
    if is_const_decl(node) {
        TreeNode *IDNode, *valueNode;
        IDNode = node->child[0];
        valueNode = node->child[1];
        if (valueNode->subType.value == NUM_LIT) {
            fprintf(output, "%s: dd %s\n", IDNode->value, valueNode->value);
        } else {
            fprintf(output, "%s: db ", IDNode->value);
            for (int i = 0; valueNode->value[i] != 0; i++) {
                fprintf(output, "%d, ", valueNode->value[i]);
            }
            fprintf(output, "0\n");
        }
    }
}

void genCodePre(int level, TreeNode *node) {
    if (node->table != NULL) {
        currentScope = node->table;
    }

    if (is_var_decl(node)) {
        genVarDecl(node);
    }

    if (node->type == StatementNode) {
        genStmt(node);
    }

    if (node->type == OperatorNode) {
        genOp(node);
    }
    if (node->type == ClauseNode) {
        genClause(node);
    }
}


void genCodePost(int level, TreeNode *node) {
    if (node->type == StatementNode && node->subType.stmt == FOR_STMT) {
        // As mentioned in genForStmt, we need to reset the break-to and continue-to labels on the symbol table
        // upon exiting a for-loop node.
        symDelete(currentScope, BREAK_SYMBOL);
        symDelete(currentScope, CONTINUE_SYMBOL);
    }
    if (node->table != NULL) {
        currentScope = currentScope->parent;
    }
    if (node->postNodeAssembly != NULL) {
        fprintf(output, "%s\n", node->postNodeAssembly);
    }

}

void genClause(TreeNode *node) {
    if (node->subType.clause == BREAK_CLAUSE) {
        Symbol *breakTo = symLookup(currentScope, BREAK_SYMBOL);
        if (breakTo != NULL) {
            fprintf(output, "jmp %s\n", breakTo->value);
        }
    }
    if (node->subType.clause == CONTINUE_CLAUSE) {
        Symbol *continueTo = symLookup(currentScope, CONTINUE_SYMBOL);
        fprintf(output, "jmp %s\n", continueTo->value);
    }
    if (node->subType.clause == CASE_CLAUSE) {
        fprintf(output, "L%d_SWITCH_CASE_%s:\n", node->lineno, node->child[0]->value);
    }
    if (node->subType.clause == DEFAULT_CLAUSE) {
        fprintf(output, "L%d_SWITCH_DEFAULT:\n", node->lineno);
    }
}

void genStmt(TreeNode *node) {
    switch (node->subType.stmt) {
        case PRINTLN_STMT:
            genPrintln(node);
            break;
        case READLN_STMT:
            genReadln(node);
            break;
        case IF_STMT:
            genIfStmt(node);
            break;
        case FOR_STMT:
            genForStmt(node);
            break;
        case SWITCH_STMT:
            genSwitchStmt(node);
            break;
        default:
            break;
    }
}

/* Generate code for if statement
 * Overwrites: EAX
 * Result in: N/A
 */
void genIfStmt(TreeNode *node) {
    TreeNode *nodeCond, *nodeBlockTrue, *nodeBlockFalse;
    nodeCond = node->child[0];
    nodeBlockTrue = node->child[1];
    nodeBlockFalse = node->child[2];
    // 32-3=29 byte should be sufficient for the label text, if not, it'll be cut short
    char label[32];
    memset(label, 0, 32);
    // Reserve 2 bytes for the ":\n" to be added later, and one byte for NULL terminator
    snprintf(label, 29, "L%d_IF_FALSE_CASE", node->lineno);
    genBooleanExp(nodeCond);
    fprintf(output, "cmp al, 0\n");
    fprintf(output, "je %s\n", label);
    strcat(label, ":\n");
    if (nodeBlockFalse == NULL) {
        nodeBlockTrue->postNodeAssembly = strdup(label);
    } else {
        // In case there is a else block, we need to append a jmp instruction to
        // the end of true case block in order to skip the false case block
        char labelFollow[32];
        memset(labelFollow, 0, 32);
        snprintf(labelFollow, 29, "L%d_IF_ELSE_FOLLOWING", nodeBlockFalse->lineno);
        char *jmpAsm = calloc(strlen(label) + 6 + strlen(labelFollow),
                              sizeof(char));
        // append the jmp instruction and the false case label to the true case block
        sprintf(jmpAsm, "jmp %s\n%s", labelFollow, label);
        nodeBlockTrue->postNodeAssembly = jmpAsm;
        nodeBlockFalse->postNodeAssembly = concatStr(labelFollow, ":\n");
    }
}

/* Generate code for for-loop
 * Overwrites: EAX
 * Result in: N/A
 */
void genForStmt(TreeNode *node) {
    TreeNode *nodeCond = node->child[0];
    TreeNode *nodeBody = node->child[1];
    char labelBegin[32];
    char labelEnd[32];
    /* Code generator uses the symbol table to figure out where to jump to when encounters break/continue statements
     * Because side-chaining is used to resolve collision in the symbol table, it behaves like a LIFO stack when there's
     * multiple symbols with the same name. Therefore break/continue will work properly even in nested for-loop.
     */
    Symbol *breakTo = symInsert(currentScope, BREAK_SYMBOL);
    Symbol *continueTo = symInsert(currentScope, CONTINUE_SYMBOL);
    snprintf(labelBegin, 32, "L%d_FOR_LOOP_BEGIN", node->lineno);
    snprintf(labelEnd, 32, "L%d_FOR_LOOP_END", node->lineno);
    breakTo->value = strdup(labelEnd);
    continueTo->value = strdup(labelBegin);

    fprintf(output, "%s:\n", labelBegin);
    genBooleanExp(nodeCond);
    fprintf(output, "cmp al, 0\n");
    fprintf(output, "je %s\n", labelEnd);
    nodeBody->postNodeAssembly = mustCalloc(strlen(labelBegin) + strlen(labelEnd) + 8, sizeof(char));
    sprintf(nodeBody->postNodeAssembly, "jmp %s\n%s:\n", labelBegin, labelEnd);
}

/* Generate comparisons and jump statements for switch statement
 * Overwrites: EAX, EDX
 * Result in: N/A
 */
void genSwitchStmt(TreeNode *node) {
    TreeNode *nodeExp = node->child[0];
    TreeNode *nodeCase = node->child[1];
    // Label added to end of each case clause
    char labelCase[32];
    char labelDefault[32];
    // Label added to the end of switch statement
    char labelEnd[32];
    int hasDefault = 0;
    snprintf(labelEnd, 29, "L%d_SWITCH_END", node->lineno);
    fprintf(output, "; beginning of switch statement\n");
    genValue(nodeExp);
    fprintf(output, "mov edx, eax\n");
    // traverse all the cases to generate comparison and jump instructions.
    while (nodeCase != NULL) {
        if (nodeCase->subType.clause == CASE_CLAUSE) {
            // The label for each case is based on the line number and value, so that later on we can generate
            // them again in the same fashion, no need to record them
            snprintf(labelCase, 29, "L%d_SWITCH_CASE_%s", nodeCase->lineno, nodeCase->child[0]->value);
            fprintf(output, "cmp edx, %s\n", nodeCase->child[0]->value);
            fprintf(output, "je %s\n", labelCase);

            // Jump to end of switch statement at end of each case block, because by default Go
            // Does not support fallthrough
            nodeCase->postNodeAssembly = mustCalloc(strlen(labelCase) + 20, sizeof(char));
            sprintf(nodeCase->postNodeAssembly, "jmp %s\n", labelEnd);

        }
        if (nodeCase->subType.clause == DEFAULT_CLAUSE) {
            hasDefault = 1;
            nodeCase->postNodeAssembly = concatStr("jmp ", labelEnd);
            snprintf(labelDefault, 32, "L%d_SWITCH_DEFAULT", nodeCase->lineno);
        }
        nodeCase = nodeCase->sibling;
    }
    // If none of the cases match, jmp to default label if default clause exists,
    // Otherwise jmp to end
    if (hasDefault == 1) {
        fprintf(output, "jmp %s\n", labelDefault);
    } else {
        fprintf(output, "jmp %s\n", labelEnd);
    }
    node->postNodeAssembly = concatStr(labelEnd, ":");
}


void genOp(TreeNode *node) {
    switch (node->subType.operator) {
        case ADDASSIGN_OP:
        case SUBASSIGN_OP:
        case ASSIGN_OP:
            genAssignOp(node);
            break;
        default:
            break;
    }
}

/* Generate code for unary operations
 * Overwrites: EAX
 * Result in: EAX
 */
void genUnaryOp(TreeNode *node) {
    genExp(node->child[0]);
    if (node->subType.operator == NEGATIVE_OP) {
        fprintf(output, "neg eax\n");
    }
}

void genAssignOp(TreeNode *node) {
    TreeNode *left = node->child[0];
    TreeNode *right = node->child[1];
    fprintf(output, "; Assignment to %s at line %d\n", left->value, left->lineno);
    genExp(right);
    fprintf(output, "push eax\n");
    loadVariable(left);
    fprintf(output, "pop eax\n");

    switch (node->subType.operator) {
        case ASSIGN_OP:
            fprintf(output, "mov [esi], eax\n");
            break;
        case ADDASSIGN_OP:
            fprintf(output, "add [esi], eax\n");
            break;
        case SUBASSIGN_OP:
            fprintf(output, "sub [esi], eax\n");
            break;
        default:
            break;
    }
    fprintf(output, ";end of assignment\n");
}

void genVarDecl(TreeNode *node) {
    TreeNode *IDNode = node->child[0];

    int h = hash(node->child[0]->value, TABLE_SIZE);
    Symbol *symbol = currentScope->symbols[h];
    while (strcmp(IDNode->value, symbol->name) != 0) {
        symbol = symbol->next;
    }
    /* this code should never run because variable references are already checked in syntax analyzer */
    if (strcmp(IDNode->value, symbol->name) != 0) {
        printf("Oops. Cannot find variable %s in the symbol table\n", IDNode->value);
        exit(1);
    }

    int size = 0;
    switch (symbol->type) {
        case VARIABLE_INTEGER:
            size = 4;
            break;
        case VARIABLE_INTEGER_ARRAY:
            size = 4 * symbol->array_length;
        default:
            break;
    }
    fprintf(output, "; Allocating %d bytes for `%s`\n", size, symbol->name);
    fprintf(output, "sub esp, %d\n", size);
    stackSize += size;
    symbol->offset = stackSize;
}


/* load the address of a variable,
 * In case of array, the address of indexed cell
 * Overwrites: ESI, EDX
 * Result in: ESI
 */
void loadVariable(TreeNode *node) {
    Symbol *symbol = symLookup(currentScope, node->value);
    if (symbol->type == VARIABLE_INTEGER) {
        fprintf(output, "lea esi, [ebp-%d]\n", symbol->offset);
    } else if (symbol->type == VARIABLE_INTEGER_ARRAY) {
        genExp(node->child[0]);
        /* Check array index at runtime */
        fprintf(output, "cmp eax, %d\n", symbol->array_length - 1);
        fprintf(output, "jg INDEX_OUT_OF_BOUNDS\n");
        fprintf(output, "mov edx, 4\n");

        //fprintf(output, "add eax, 1\n");
        fprintf(output, "imul edx\n");
        fprintf(output, "lea esi, [ebp-%d]\n", symbol->offset);
        fprintf(output, "add esi, eax\n");
    }

}

void genExp(TreeNode *node) {
    if (node->type == ValueNode) {
        genValue(node);
    }
    if (is_bin_arith_node(node)) {
        genBinaryExp(node);
    }
    if (is_unary_node(node)) {
        genUnaryOp(node);
    }
}

/* if an boolean expression evaluates to be true, EAX will be set to 1
 * Overwrite: EAX, EBX, ECX, EDX
 * Result in: EAX
 */
void genBooleanExp(TreeNode *node) {
    TreeNode *left, *right;
    left = node->child[0];
    right = node->child[1];
    if (is_logic_op_node(node)) {
        genBooleanExp(left);
        fprintf(output, "push eax\n");
        genBooleanExp(right);
        switch (node->subType.operator) {
            case AND_OP:
                fprintf(output, "and eax, dword [esp]\n");
                break;
            case OR_OP:
                fprintf(output, "or eax, dword [esp]\n");
                break;
            default:
                break;
        }
        fprintf(output, "add esp, 4\n");
    } else {
        genExp(left);
        fprintf(output, "mov ebx, eax\n");
        genExp(right);
        fprintf(output, "mov ecx, eax\n");
        // zero eax with xor for performance reason
        // ref: https://stackoverflow.com/questions/33666617/what-is-the-best-way-to-set-a-register-to-zero-in-x86-assembly-xor-mov-or-and/33668295#33668295
        fprintf(output, "xor eax, eax\n");
        fprintf(output, "cmp ebx, ecx\n");
        switch (node->subType.operator) {
            case EQ_OP:
                fprintf(output, "sete al\n");
                break;
            case NE_OP:
                fprintf(output, "setne al\n");
                break;
            case LT_OP:
                fprintf(output, "setl al\n");
                break;
            case LE_OP:
                fprintf(output, "setle al\n");
                break;
            case GT_OP:
                fprintf(output, "setg al\n");
                break;
            case GE_OP:
                fprintf(output, "setge al\n");
                break;
            default:
                break;
        }
    }
}

/* Evaluates a binary arithmetic expression
 * Overwrites: EAX, ECX, EDX
 * Result in: EAX
 */
void genBinaryExp(TreeNode *tree) {
    TreeNode *left, *right;
    left = tree->child[0];
    right = tree->child[1];

    genExp(right);
    fprintf(output, "push eax\n");
    genExp(left);
    // put the result of rhs to ecx
    fprintf(output, "pop ecx\n");
    switch (tree->subType.operator) {
        case ADD_OP:
            fprintf(output, "add eax, ecx\n");
            break;
        case MUL_OP:
            fprintf(output, "imul ecx\n");
            break;
        case SUBTRACT_OP:
            fprintf(output, "sub eax, ecx\n");
            break;
        case DIV_OP:
            /* The First operand(dividend) for idiv is EDX:EAX, therefore EDX needs to be zeroed out */
            fprintf(output, "xor edx, edx\n");
            fprintf(output, "idiv ecx\n");
            break;
        case LSHIFT_OP:
            /* second operand for ehl/shr must be either cl or imm8 */
            fprintf(output, "shl eax, cl\n");
            break;
        case RSHIFT_OP:
            fprintf(output, "shr eax, cl\n");
            break;
        case MOD_OP:
            fprintf(output, "xor edx, edx\n");
            fprintf(output, "idiv ecx\n");
            fprintf(output, "mov eax, edx\n");
        default:
            break;
    }
}

/* load value of an numerical literal, variable, or constant
 * In case of a string constant, the address
 * Overwrites: EAX
 * Result in: EAX
 */
void genValue(TreeNode *node) {
    if (node->subType.value == IDENTIFIER) {
        Symbol *symbol = symLookup(currentScope, node->value);
        switch (symbol->type) {
            case VARIABLE_INTEGER:
            case VARIABLE_INTEGER_ARRAY:
                loadVariable(node);
                fprintf(output, "mov eax,[esi]\n");
                break;
            case CONST_INTEGER:
                fprintf(output, "mov eax, [%s]\n", node->value);
                break;
            case CONST_STRING:
                fprintf(output, "mov eax, %s\n", node->value);
                break;
            default:
                break;
        }
    } else {
        fprintf(output, "mov eax, %s\n", node->value);
    }
}

/* call println with appropriate arguments
 * Overwrites: EAX, ECX, EDX
 * Result in: N/A
 */
void genPrintln(TreeNode *node) {
    int count = 0;
    TreeNode *arg = node->child[0];
    while (arg != NULL) {
        genExp(arg);
        fprintf(output, "push eax\n");

        // if an identifier is passed as an argument,
        // we need to figure out whether it's a string constant by looking up the symbol table
        // This is sort of ugly but it avoids introducing unnecessary temp variables
        if (is_id_node(arg) && symLookup(currentScope, arg->value)->type == CONST_STRING) {
            fprintf(output, "push dword 1\n");
        } else {
            fprintf(output, "push dword 0\n");
        }
        arg = arg->sibling;
        count++;
    }
    fprintf(output, "push %d\n", count * 2);
    fprintf(output, "call println\n");
    fprintf(output, "add esp, %d\n", 4 * (count * 2 + 1));
}

/* call readln with appropriate arguments
 * Overwrites: EAX
 * Result in: N/A
 */
void genReadln(TreeNode *node) {
    fprintf(output, "; line %d: readln\n", node->lineno);
    int count = 0;
    TreeNode *arg = node->child[0];
    while (arg != NULL) {
        loadVariable(arg);
        fprintf(output, "push esi\n");
        arg = arg->sibling;
        count++;
    }
    fprintf(output, "push dword %d\n", count);
    fprintf(output, "call readln\n");
    fprintf(output, "add esp, %d\n", 4 * (count + 1));
    fprintf(output, "; readln ends\n");
}
