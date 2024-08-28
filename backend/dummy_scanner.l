%{
#include "scanner.h"
#include <sstream>
#include "ast/genericType.h"
#include "runtime/runtime.h"
using namespace std;
%}

%option noyywrap c++
%option yyclass="Scanner"
%option caseless

%%

<<EOF>>                     {   yyterminate();  }

[\n]+                       { return '\n'; }

[ \t\r\v]+                /* skip whitespace */

"load"                      { return yy::parser::make_OPER(Oper::LOAD);}
"store"                     { return yy::parser::make_OPER(Oper::STORE);}
"forward"                   { return yy::parser::make_OPER(Oper::FORWARD);}
"backward"                  { return yy::parser::make_OPER(Oper::BACK);}
"left"                      { return yy::parser::make_OPER(Oper::LEFT);}
"right"                     { return yy::parser::make_OPER(Oper::RIGHT);}
"look"                      { return yy::parser::make_OPER(Oper::LOOK);}
"test"                      { return yy::parser::make_OPER(Oper::TEST);}


"var"

"ninf"                      { return yy::parser::make_NUMBER(IntBase::NEG_INF);}
"inf"                       { return yy::parser::make_NUMBER(IntBase::INF);}
"nan"                       { return yy::parser::make_NUMBER(IntBase::NAN);}

"empty"                     { return yy::parser::make_NUMBER(CellBase::EMPTY);}
"wall"                      { return yy::parser::make_NUMBER(CellBase::WALL);}
"exit"                      { return yy::parser::make_NUMBER(CellBase::EXIT);}
"box"[0-9]+                 { return yy::parser::make_NUMBER(Generic(CellBase::BOX, atoi(yytext+3)));}

t(rue)*                     { return yy::parser::make_NUMBER(BoolBase::TRUE);}
f(alse)*                    { return yy::parser::make_NUMBER(BoolBase::FALSE);}
u(ndef)*                    { return yy::parser::make_NUMBER(BoolBase::UNDEF);}

":="                        { return yy::parser::make_ASSIGN(); }

"int"                       { return yy::parser::make_TYPE(CurType::INT); }
"bool"                      { return yy::parser::make_TYPE(CurType::BOOL); }
"cell"                      { return yy::parser::make_TYPE(CurType::CELL); }

"while"                     { return yy::parser::make_WHILE(); }
"finish"                    { return yy::parser::make_FINISH(); }
"if"                        { return yy::parser::make_IF(); }
"eldef"                     { return yy::parser::make_ELDEF(); }
"elund"                     { return yy::parser::make_ELUND(); }
"return"                    { return yy::parser::make_RET(); }

"^"                         { return yy::parser::make_XOR(); }
"<"                         { return yy::parser::make_LESS(); }
">"                         { return yy::parser::make_GREATER(); }
"="                         { return yy::parser::make_EQUAL(); }

"+"                         { return yy::parser::make_PLUS(); }
"-"                         { return yy::parser::make_MINUS(); }

[0-9]+                      { return yy::parser::make_NUMBER(atoi(yytext)); }

[a-zA-Z][a-zA-Z0-9]*        { return yy::parser::make_IDENTIFIER(yytext); }

[(){}]                      { return yytext[0]; }

.                           { cerr << "Illegal character: " << yytext << endl; }

%%

