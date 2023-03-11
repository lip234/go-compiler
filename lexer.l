/* Author: Peng Li
 * Student ID: 3012769
 * MacEwan University CMPT 399: Compilers Fall 2020
 */
%option yylineno noyywrap nounput noinput
%top{
#include "parser.h"
int getToken();

#define T_ERROR -1
#define T_EOF 0
}

/* flex does not support non-greedy quantifier therefore we have to manually exclude comment terminator inside comment body
* ref: https://stackoverflow.com/a/32320759
*/
comment "/*"([^*]|(\*+[^*/]))*\*+\/
ID {letter_}({letter_}|{digit})*
letter_ [a-zA-Z_]
string_literal `[^`]*`
newline \r?\n
num ({decimal_digits}|{hex_literal})
hex_literal 0[xX]_?{hex_digits}
hex_digits {hex_digit}(_?{hex_digit})*
hex_digit [0-9a-fA-F]
decimal_digits {digit}(_?{digit})*
digits [0-9]+
digit [0-9]

white_space " "

/* Lexemes that starts with a digit and contain at least one letter
 * For example: 0abc, According to MacEwan Go specification this should produce an error token,
 * However, in Go's built-in scanner this returns a NUM token 0, and an identifier token abc
 * Ref: https://play.golang.org/p/3ZN_W-HDqc3
 */
invalid_id {digit}+[a-zA-Z0-9]+
%%

break {return T_BREAK;}
case {return T_CASE;}
const {return T_CONST;}
continue {return T_CONTINUE;}
default {return T_DEFAULT;}
else {return T_ELSE;}
for {return T_FOR;}
func {return T_FUNC;}
if {return T_IF;}
int {return T_INT;}
package {return T_PACKAGE;}
println {return T_PRINTLN;}
readln {return T_READLN;}
switch {return T_SWITCH;}
var {return T_VAR;}

"==" {return T_EQ;}
"!=" {return T_NE;}
"<=" {return T_LE;}
">=" {return T_GE;}
"<<" {return T_LSHIFT;}
">>" {return T_RSHIFT;}
"+=" {return T_ADDASSIGN;}
"-=" {return T_SUBASSIGN;}
"&&" {return T_AND;}
"||" {return T_OR;}

"<" {return T_LT;}
">" {return T_GT;}

"(" {return T_LPAREN;}
")" {return T_RPAREN;}
"{" {return T_LBRACE;}
"}" {return T_RBRACE;}
"," {return T_COMMA;}
"/" {return T_SLASH;}
"-" {return T_MINUS;}
"+" {return T_PLUS;}
"*" {return T_ASTERISK;}
"%" {return T_PERCENT;}
"=" {return T_EQUAL;}
"[" {return T_LBRACKET;}
"]" {return T_RBRACKET;}
";" {return T_SEMICOLON;}
":" {return T_COLON;}
"&" {return T_AMPERSAND;}

{comment} {}
{num} {return T_NUM;}
{string_literal} {return T_STRINGLIT;}
{ID} {return T_ID;}

\t {}
{white_space} {}
{newline} {}
<<EOF>> {return T_EOF;}
{invalid_id} {return T_ERROR;}
. {return T_ERROR;}

%%
int getToken() {
    return yylex();
}