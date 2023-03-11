// Author: Peng Li
// Student ID: 3012769
// MacEwan University CMPT 399: Compilers Fall 2020

%include{
#include "lexer.h"
#include "tree.h"
#include <string.h>

static TreeNode *root;
TreeNode *get_root(){
	return root;
}
}

///* function added to end of parser.c to get the pointer to
// the token names array
%code {
const char* const* get_token_names(){
	return yyTokenName;}
}
%token_type {char *}
%default_type {TreeNode *}
%syntax_error {printf("syntax error on line %d\n;", yylineno);}
%parse_failure {printf("parse failure on line %d\n;", yylineno); exit(-1);}

%left T_OR.
%left T_AND.
%left T_EQ T_NE T_LT T_LE T_GT T_GE.
%left T_PLUS T_MINUS.
%left T_ASTERISK T_SLASH T_PERCENT T_LSHIFT T_RSHIFT T_AMPERSAND.

%start_symbol program
program ::= packageClause(pkg) T_SEMICOLON topLevelDecl(top) T_SEMICOLON . {
	root = newClauseNode(PROGRAM_CLAUSE, "PROGRAM");
	root->child[0] = pkg;
	root->child[1] = top;
	root->lineno = yylineno;
}

packageClause(A) ::= T_PACKAGE identifier(id) . {
	A = newClauseNode(PACKAGE_CLAUSE, "PACKAGE");
	A->child[0] = id;
	 A->lineno = yylineno;
}
identifier(A) ::= T_ID(id). {A = newValueNode(IDENTIFIER, id); A->lineno = yylineno;}

// A topLevelDecl node has 3 children
// 1. (Optional) constDecl
// 2. (Optional) varDecl
// 3. functionDecl
// Where an optional declaration is emitted, the corresponding
// node with no children is assigned for consistency
topLevelDecl(A) ::= functionDecl(fd). {
	A = newDeclarationNode(TOP_LEVEL_DECL, "TOP LEVEL DECL");
	A->child[0] = newDeclarationNode(CONST_DECL, "CONST DECL");
	A->child[1] = newDeclarationNode(VAR_DECL, "VAR DECL");
	A->child[2] = fd;
}
topLevelDecl(A) ::= constDecl(cd) T_SEMICOLON functionDecl(fd). {
	A = newDeclarationNode(TOP_LEVEL_DECL, "TOP LEVEL DECL");
	A->child[0] = cd;
	A->child[1] = newDeclarationNode(CONST_DECL, "CONST DECL");
	A->child[2] = fd;
}
topLevelDecl(A) ::= varDecl(vd) T_SEMICOLON functionDecl(fd). {
	A = newDeclarationNode(TOP_LEVEL_DECL, "TOP LEVEL DECL");
	A->child[0] = newDeclarationNode(CONST_DECL, "CONST DECL");
	A->child[1] = vd;
	A->child[2] = fd;
}
topLevelDecl(A) ::= constDecl(cd) T_SEMICOLON  varDecl(vd) T_SEMICOLON functionDecl(fd). {
	A = newDeclarationNode(TOP_LEVEL_DECL, "TOP LEVEL DECL");
	A->child[0] = cd;
	A->child[1] = vd;
	A->child[2] = fd;
}

constDecl(A) ::= T_CONST constSpec(B). {A = B;}
constDecl(A) ::= T_CONST T_LPAREN constSpecList(list) T_RPAREN. {
	A = newDeclarationNode(CONST_DECL, "CONST");
	A->child[0] = list;
}

// 1 or more <ConstSpec>;
constSpecList(A) ::= constSpecList(list) constSpec(cd) T_SEMICOLON. {
	cd->sibling = list;
	A = cd;
}
constSpecList(A) ::= . { A = NULL;}

constSpec(A) ::= identifier(lhs) T_EQUAL T_NUM(rhs) . {
	A = newDeclarationNode(CONST_DECL, "CONST");
	TreeNode *num = newValueNode(NUM_LIT, rhs);
	A->child[0] = lhs;
	A->child[1] = num;
	A->lineno = yylineno;
}
constSpec(A) ::= identifier(lhs) T_EQUAL T_STRINGLIT(rhs) . {
	A = newDeclarationNode(CONST_DECL, "CONST");
	// remove the back quotation surrounding a string literal
	rhs[strlen(rhs)-1] = 0;
	TreeNode *str = newValueNode(STR_LIT, rhs+1);
	A->child[0] = lhs;
	A->child[1] = str;
	A->lineno = yylineno;
}

varDecl(A) ::= T_VAR varSpec(B). {A = B;}
varDecl(A) ::= T_VAR T_LPAREN varSpecList(list) T_RPAREN. {
	A = list;
}

varSpec(A) ::= identifier(id) type(type). {
	A = newDeclarationNode(VAR_DECL, "VAR");
	A->child[0] = id;
	A->child[1] = type;
	A->lineno = yylineno;

}

// Zero or more VarSpec;
varSpecList(A) ::= varSpecList(list) varSpec(varspec) T_SEMICOLON. {
	varspec->sibling = list;
	A = varspec;
	A->lineno = yylineno;

}
varSpecList(A) ::= . {A = NULL;}
type(A) ::= typeName(B). {A = B;}
type(A) ::= arrayType(B). {A = B;}
arrayType(A) ::= T_LBRACKET constant(c) T_RBRACKET typeName(type). {
	A = newTypeNode(ARRAY_TYPE, "ARRAY");
	A->child[0] = type;
	A->child[1] = c;
	A->lineno = yylineno;

}

typeName(A) ::= T_INT. {A = newTypeNode(INT_TYPE, "INT");}

functionDecl(A) ::= T_FUNC identifier(id) T_LPAREN T_RPAREN block(b). {
	A = newDeclarationNode(FUNC_DECL, "FUNC");
	A->child[0] = id;
	A->child[1] = b;
	A->lineno = yylineno;

}


// A topLevelDecl node has 3 children
// 1. (Optional) constDecl
// 2. (Optional) varDecl
// 3. functionDecl
// Where an optional declaration is emitted, the corresponding
// node with no children is assigned for consistency
block(A) ::= T_LBRACE statementList(stmts) T_RBRACE. {
	A = newClauseNode(BLOCK_CLAUSE, "BLOCK");
	A->child[0] = newDeclarationNode(CONST_DECL, "CONST");
	A->child[1] = newDeclarationNode(VAR_DECL, "VAR");
	A->child[2] = stmts;
	A->lineno = yylineno;

}

block(A) ::= T_LBRACE constDecl(cd) T_SEMICOLON statementList(stmts) T_RBRACE. {
	A = newClauseNode(BLOCK_CLAUSE, "BLOCK");
	A->child[0] = cd;
	A->child[1] = newDeclarationNode(VAR_DECL, "VAR DECL");
	A->child[2] = stmts;
	A->lineno = yylineno;

}
block(A) ::= T_LBRACE varDecl(vd) T_SEMICOLON statementList(stmts) T_RBRACE. {
	A = newClauseNode(BLOCK_CLAUSE, "BLOCK");
	A->child[0] = newDeclarationNode(CONST_DECL, "CONST DECL");
	A->child[1] = vd;
	A->child[2] = stmts;
	A->lineno = yylineno;

}
block(A) ::= T_LBRACE constDecl(cd) T_SEMICOLON varDecl(vd) T_SEMICOLON statementList(stmts) T_RBRACE. {
	A = newClauseNode(BLOCK_CLAUSE, "BLOCK");
	A->child[0] = cd;
	A->child[1] = vd;
	A->child[2] = stmts;
	A->lineno = yylineno;

}

// According to MacEwan Go specification, all statements (including empty stmt)
// must be followed by semicolon
// statementList derives 0 or more statements. multiple statements are linked as siblings
// Null for none
statementList(A) ::= statementList(list) statement(stmt) T_SEMICOLON. {
	stmt->sibling = list;
	A = stmt;
	A->lineno = yylineno;

}
statementList(A) ::= . {A = NULL;}

statement(A) ::= simpleStmt(B). {A = B;}
statement(A) ::= breakStmt(B). {A = B;}
statement(A) ::= continueStmt(B). {A = B;}
statement(A) ::= block(B). {A = B;}
statement(A) ::= ifStmt(B). {A = B;}
statement(A) ::= switchStmt(B). {A = B;}
statement(A) ::= forStmt(B). {A = B;}
statement(A) ::= printlnStmt(B). {A = B;}
statement(A) ::= readlnStmt(B). {A = B;}

simpleStmt(A) ::= emptyStmt(B). {A = B;}
simpleStmt(A) ::= assignmentStmt(B). {A = B;}
simpleStmt(A) ::= expression(B). {A = B;}
emptyStmt(A) ::= . {A = newStatementNode(EMPTY_STMT, "EMPTY STATEMENT");}
assignmentStmt(A) ::= variable(lhs) assignOp(op) expression(rhs). {
	op->child[0] = lhs;
	op->child[1] = rhs;
	A = op;
	A->lineno = yylineno;

}

breakStmt(A) ::= T_BREAK. {A = newClauseNode(BREAK_CLAUSE, "BREAK");}
continueStmt(A) ::= T_CONTINUE. {A = newClauseNode(CONTINUE_CLAUSE, "CONTINUE");}

// If statement node has 3 children, from left to right:
// 1. Condition clause
// 2. Block
// 3. (Optional) else clause
ifStmt(A) ::= T_IF condition(con) block(blk) elseClause(ec) . {
	A = newStatementNode(IF_STMT, "IF");
	A->child[0] = con;
	A->child[1] = blk;
	A->child[2] = ec;
	A->lineno = yylineno;
}

elseClause(A) ::= T_ELSE ifStmt(B). {
	A = newClauseNode(ELSE_CLAUSE, "ELSE");
	A->child[0] = B;
	A->lineno = yylineno;
}

elseClause(A) ::= T_ELSE block(B). {
	A = newClauseNode(ELSE_CLAUSE, "ELSE");
	A->child[0] = B;
	A->lineno = yylineno;

}
elseClause(A) ::= . {A = NULL;}

condition(A) ::= expression(B). {A = B;}
switchStmt(A) ::= T_SWITCH expression(expr) T_LBRACE caseClauseList(cclist) T_RBRACE. {
	A = newStatementNode(SWITCH_STMT, "SWITCH");
	A->child[0] = expr;
	A->child[1] = cclist;
	A->lineno = yylineno;

}
caseClauseList(A) ::= caseClauseList(cclist) caseClause(cc). {
	cc->sibling = cclist;
	A = cc;
	A->lineno = yylineno;

}
caseClauseList(A) ::= . {A = NULL;}
caseClause(A) ::= switchCase(sc) T_COLON statementList(stmts). {
	if (sc->subType.clause == DEFAULT_CLAUSE) {
	sc->child[0] = stmts;
	} else {
	sc->child[1] = stmts;
	}
	A = sc;
	A->lineno = yylineno;

}
switchCase(A) ::= T_CASE constList(list). {
	A = newClauseNode(CASE_CLAUSE, "CASE");
	A->child[0] = list;
	A->lineno = yylineno;

}
switchCase(A) ::= T_DEFAULT. {
	A = newClauseNode(DEFAULT_CLAUSE, "DEFAULT");
	A->lineno = yylineno;

}
constList(A) ::= constant(c) T_COMMA constList(list). {
	c->sibling = list;
	A = c;
	A->lineno = yylineno;
}
constList(A) ::= constant(c) . {A = c;}
constant(A) ::= T_ID(id) . {A = newValueNode(IDENTIFIER, id);	A->lineno = yylineno;}
constant(A) ::= T_NUM(num) . {A = newValueNode(NUM_LIT, num);	A->lineno = yylineno;}

unaryExpr(A) ::= primaryExpr(B). {A = B;}
unaryExpr(A) ::= T_PLUS unaryExpr(B). {
	A = newOperatorNode(POSITIVE_OP, "+");
	A->child[0] = B;
	A->lineno = yylineno;
}
unaryExpr(A) ::= T_MINUS unaryExpr(B). {
	A = newOperatorNode(NEGATIVE_OP, "-");
	A->child[0] = B;
	A->lineno = yylineno;
}

primaryExpr(A) ::= T_NUM(num). {A = newValueNode(NUM_LIT, num);	A->lineno = yylineno;}
primaryExpr(A) ::= variable(B). {A = B;}
primaryExpr(A) ::= T_LPAREN expression(B) T_RPAREN. {A = B;}


// expression is broken down into several non-terminals,
// the higher number means higher precedence
// expression itself has the lowest precedence
// In each expression, left-recursion is allowed becuase operators of the same
// precedence are parsed left-to-right order
expression(A) ::= expression1(B). {A = B;}
expression(A) ::= expression(lhs) T_OR expression1(rhs). {
	A = newOperatorNode(OR_OP, "||");
	A->child[0] = lhs;
	A->child[1]=rhs;
	A->lineno = yylineno;
}

expression1(A) ::= expression1(lhs) T_AND expression2(rhs). {
	A = newOperatorNode(AND_OP, "&&");
	A->child[0] = lhs;
	A->child[1]=rhs;
	A->lineno = yylineno;
}
expression1(A) ::= expression2(B). {A = B;}
expression2(A) ::= expression2(lhs) relOp(op) expression3(rhs). {op->child[0] = lhs; op->child[1]=rhs;A = op;}
expression2(A) ::= expression3(B) . {A = B;}
expression3(A) ::= expression3(lhs) addOp(op) expression4(rhs). {op->child[0] = lhs; op->child[1]=rhs;A = op;}
expression3(A) ::= expression4(B) . {A = B;}
expression4(A) ::= expression4(lhs) mulOp(op) factor(rhs). {op->child[0] = lhs; op->child[1]=rhs;A = op;}
expression4(A) ::= factor(B) . {A = B;}

factor(A) ::= T_LPAREN expression(expr) T_LPAREN. {A=expr;}
factor(A) ::= unaryExpr(uexpr). {A = uexpr;}

assignOp(A) ::= T_EQUAL. {A = newOperatorNode(ASSIGN_OP, "=");A->lineno = yylineno;}
assignOp(A) ::= T_ADDASSIGN. {A = newOperatorNode(ADDASSIGN_OP, "+=");A->lineno = yylineno;}
assignOp(A) ::= T_SUBASSIGN. {A = newOperatorNode(SUBASSIGN_OP, "-=");A->lineno = yylineno;}


mulOp(A) ::= T_ASTERISK. {A = newOperatorNode(MUL_OP, "*");}
mulOp(A) ::= T_SLASH. {A = newOperatorNode(DIV_OP, "/");}
mulOp(A) ::= T_PERCENT. {A = newOperatorNode(MOD_OP, "%");}
mulOp(A) ::= T_LSHIFT. {A = newOperatorNode(LSHIFT_OP, "<<");}
mulOp(A) ::= T_RSHIFT. {A = newOperatorNode(RSHIFT_OP, ">>");}
addOp(A) ::= T_PLUS. {A = newOperatorNode(ADD_OP, "+");}
addOp(A) ::= T_MINUS. {A = newOperatorNode(SUBTRACT_OP, "-");}
relOp(A) ::= T_EQ. {A = newOperatorNode(EQ_OP, "==");}
relOp(A) ::= T_NE. {A = newOperatorNode(NE_OP, "!=");}
relOp(A) ::= T_LT. {A = newOperatorNode(LT_OP, "<");}
relOp(A) ::= T_LE.  {A = newOperatorNode(LE_OP, "<=");}
relOp(A) ::= T_GT.  {A = newOperatorNode(GT_OP, ">");}
relOp(A) ::= T_GE.  {A = newOperatorNode(GE_OP, ">=");}

variable(A) ::= identifier(B).  {A = B;}
variable(A) ::= identifier(id) T_LBRACKET expression(expr) T_RBRACKET. {
	id->child[0] = expr;
	A = id;
	A->lineno = yylineno;
}

// Condition may be emitted, in such case
// A dummy boolean node 1==1 is assigned as condition,
// this is for consistency.
forStmt(A) ::= T_FOR block(b). {
	A = newStatementNode(FOR_STMT, "forStmt");
	A->child[0] = newOperatorNode(EQ_OP, "==");
	A->child[0]->child[0] = newValueNode(NUM_LIT, "1");
	A->child[0]->child[1] = newValueNode(NUM_LIT, "1");
	A->child[1] = b;
	A->lineno = yylineno;
}
forStmt(A) ::= T_FOR condition(cond) block(b). {
	A = newStatementNode(FOR_STMT, "forStmt");
	A->child[0] = cond;
	A->child[1] = b;
	A->lineno = yylineno;
}
printlnStmt(A) ::= T_PRINTLN T_LPAREN printlnStmtPrime(prime) T_RPAREN. {
	A = newStatementNode(PRINTLN_STMT, "printlnStmt");
	A->child[0] = prime;
	A->lineno = yylineno;
}
// printlnStmtPrime derives 1 or more expressions. multiple expressions are linked as sibligns
printlnStmtPrime(A) ::= printlnStmtPrime(prime) T_COMMA expression (expr). {
	expr->sibling = prime;
	A = expr;
	A->lineno = yylineno;
}
printlnStmtPrime(A) ::=  expression (expr). {A = expr;}

readlnStmt(A) ::= T_READLN T_LPAREN readlnStmtPrime(prime) T_RPAREN. {
	A = newStatementNode(READLN_STMT, "readlnStmt");
	A->child[0] = prime;
	A->lineno = yylineno;
}

// readlnStmtPrime drives 1 or more variables. multiple variables are linked as siblings.
readlnStmtPrime(A) ::= readlnStmtPrime(prime) T_COMMA T_AMPERSAND variable(var). {
	var->sibling = prime;
	A = var;
	A->lineno = yylineno;
}
readlnStmtPrime(A) ::= T_AMPERSAND variable(var) .  {A = var;A->lineno = yylineno;}
