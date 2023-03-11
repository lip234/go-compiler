/* Tree.h
 * Peng Li
 * CMPT 399 (Fall 2020)
 * This work is derived from code provided in Lab #4: Introduction to lemon
 * Original author: Nicholas M. Boers
*/

#ifndef TREE_H
#define TREE_H

#include "symtab.h"

#define MAX_CHILDREN 3

#define is_bin_arith_node(x) ((x)->type == OperatorNode &&  \
    (x)->subType.operator >= MUL_OP)

#define is_const_decl(x) ((x)->type == DeclarationNode && \
    (x)->subType.decl == CONST_DECL &&                    \
    (x)->child[0] != NULL &&                              \
    (x)->child[1] != NULL)
#define is_boolean_node(x) ((x)->type == OperatorNode && \
    (x)->subType.operator >= EQ_OP &&                    \
    (x)->subType.operator <= AND_OP)
#define is_rel_op_node(x) ((x)->type == OperatorNode && \
    (x)->subType.operator >= EQ_OP &&                   \
    (x)->subType.operator <= GE_OP)
#define is_logic_op_node(x) ((x)->type == OperatorNode && \
    ((x)->subType.operator == OR_OP ||                  \
    (x)->subType.operator == AND_OP ))
#define is_unary_node(x) ((x)->type == OperatorNode && (\
    (x)->subType.operator == POSITIVE_OP ||             \
    (x)->subType.operator ==NEGATIVE_OP))

#define is_var_decl(x) ((x)->type == DeclarationNode && \
    (x)->subType.decl == VAR_DECL &&                    \
    (x)->child[0] != NULL)
#define is_id_node(x) ((x)->type == ValueNode && (x)->subType.value==IDENTIFIER)


typedef enum {
    // Assign operators
    ASSIGN_OP = 0, SUBASSIGN_OP, ADDASSIGN_OP,

    // Unary operators
    POSITIVE_OP, NEGATIVE_OP,

    // Binary relational operators
    EQ_OP, NE_OP, LT_OP, LE_OP, GT_OP, GE_OP,
    OR_OP, AND_OP,

    // Binary arithmetic operators
    MUL_OP, DIV_OP, MOD_OP, LSHIFT_OP, RSHIFT_OP,
    ADD_OP, SUBTRACT_OP
} Operator;

typedef enum {
    EMPTY_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    IF_STMT,
    SWITCH_STMT,
    FOR_STMT,
    PRINTLN_STMT,
    READLN_STMT,
} Statement;

typedef enum {
    IDENTIFIER,
    NUM_LIT,
    STR_LIT,
} ValueHolder;

typedef enum {
    FUNC_DECL,
    CONST_DECL,
    VAR_DECL,
    TOP_LEVEL_DECL
} Declaration;

typedef enum {
    INT_TYPE,
    ARRAY_TYPE
} VariableType;

typedef enum {
    PACKAGE_CLAUSE,
    DEFAULT_CLAUSE,
    CASE_CLAUSE,
    ELSE_CLAUSE,
    BREAK_CLAUSE,
    PROGRAM_CLAUSE,
    BLOCK_CLAUSE,
    CONTINUE_CLAUSE
} Clause;

typedef enum {
    OperatorNode,
    StatementNode,
    ValueNode,
    DeclarationNode,
    TypeNode,
    ClauseNode
} NodeType;

typedef struct _TreeNode {
    NodeType type;
    union {
        Operator operator;
        Statement stmt;
        ValueHolder value;
        Declaration decl;
        VariableType varType;
        Clause clause;
    } subType;
    // for terminal symbols, the line number where it appears
    // for non-terminal symbols, the line number of the first terminal symbol
    int lineno;
    char *value;
    // For code generator, text to be appended to the end of assembly code for this node
    // which can be assembly code, label, or comment
    char *postNodeAssembly;
    struct _TreeNode *child[MAX_CHILDREN + 1];
    struct _TreeNode *sibling;
    SymTable *table;
} TreeNode;

/* newNode creates a new tree node. The first argument contains the
   new nodes's value as a C-style string. It's copied to
   dynamically-allocated memory within the new node. The second
   argument contains the number of children. */
TreeNode *newNode(char *);

/* traverseTree does a tree traversal. The first argument records the
   recursion depth, and it should be initialized to 0 when calling
   this function. The second argument is the root node, and the last
   two function pointers provide pre- and post-order functions to
   execute, respectively. */
void traverseTree(int, TreeNode *,
                  void (*)(int, TreeNode *),
                  void (*)(int, TreeNode *));

void genClause(TreeNode *node);

/* destroyTree destroys a tree. Its argument is the root tree node. */
void destroyTree(TreeNode *);


/* Create a node of appropriate type, first argument refers to the subtype,
 * second argument is a human-readable description
 */
TreeNode *newOperatorNode(Operator, char *);

TreeNode *newStatementNode(Statement, char *);

TreeNode *newDeclarationNode(Declaration, char *);

TreeNode *newValueNode(ValueHolder valueHolder, char *);

TreeNode *newTypeNode(VariableType varType, char *);

TreeNode *newClauseNode(Clause, char *);

#endif /* TREE_H */
