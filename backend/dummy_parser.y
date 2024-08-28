%defines
%define parse.error detailed
%define api.token.constructor
%define api.value.type variant

%language "c++"

%code requires {
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "ast/ast.h"
#include "runtime/runtime.h"
class Scanner;
}

%code {
    #include "scanner.h"

    extern "C" int yyerror(const char *s) { 
        std::cout << s << std::endl;
        return 0;
    };

    void yy::parser::error (const std::string& msg) {
        std::cout << msg << std::endl;
    };

    static yy::parser::symbol_type yylex(Scanner &scanner) {
        return scanner.ScanToken();
    }

    funcTable global_table;
    Field field("/home/cracky/MakeRzhakaGreatAgain/backend/rt.field");
}


%lex-param { Scanner &scanner }
%parse-param { Scanner &scanner }

%token <Generic> NUMBER
%token <std::string> IDENTIFIER
%token ASSIGN
%token <CurType> TYPE
%token WHILE FINISH IF ELDEF ELUND RET
%token XOR LESS GREATER EQUAL
%token PLUS MINUS
%token <Oper> OPER 
%left XOR
%left LESS GREATER EQUAL
%left PLUS MINUS

%type <ASTNode*> expr
%type <ASTNode*> assign_stmt
%type <ASTNode*> while_stmt
%type <ASTNode*> if_stmt
%type <ASTNode*> decl_stmt
%type <ASTNode*> statement
%type <nodeList> statements
%type <std::string> func_start


%%

program:
    function_defs {
        varTable main_table;
        auto main_node = new FunctionCallNode("main", new NumberNode(GenericType(), "void"), global_table);
        main_node->eval(main_table);
};

function_defs:
    function_defs function_def '\n'
    | function_def '\n'
    ;

function_def:
    func_start statements '}'
    {
        // Storing lambda to be called 
        FuncLambda func 
        = [=](GenericType arg, nodeList& body, Signature sign) {
            global_table.print();
            varTable localVars;
            // Assign the parameter to the local variable
            localVars.setVar(sign.second, arg); 
            // Evaluate the function body
            GenericType result;
            for (auto stmt : body) {
                if (stmt->eval(localVars).getRet()) break;
                localVars.print();
            }

            result = localVars.getVar(sign.second);
            result.setType(sign.first);
            localVars.setVar(sign.second, result);

            std::cout << "Result: ";
            std::cout << result;

            std::cout << std::endl;
            return result;
        };
        
        global_table.setFuncBody($1, std::make_pair(func, $2));
    }
;

func_start: TYPE IDENTIFIER '(' IDENTIFIER ')' '{' '\n' {
    global_table.setFuncSign($2, std::make_pair($1, $4));
    $$ = $2;
}

statements:
    statements statement '\n' { $1.push_back($2); $$=$1; }
    | statement '\n' { $$ = nodeList({$1}); }
    ;

statement:
    assign_stmt         { $$ = $1; }
    | expr              { $$ = $1; }
    | while_stmt        { $$ = $1; }
    | if_stmt           { $$ = $1; }
    | decl_stmt         { $$ = $1; }
    | RET               { $$ = new RetNode(); }
;

while_stmt:
    WHILE '(' expr ')' '{' '\n' statements '}' {
        $$ = new WhileNode($3, $7);
    }
    //| WHILE '(' expr ')' '{' '\n' statements '}' '\n' FINISH '{' statements '}' 
    //{
    //    $$ = new WhileNode($3, $7, $12);
    //}
;

if_stmt:
    IF '(' expr ')' '{' '\n' statements '}' '\n' ELDEF '{' '\n' statements '}' '\n' ELUND '{' '\n' statements '}' {
        $$ = new IfNode($3, $7, $13, $19);
    }
    | IF '(' expr ')' '{' '\n' statements '}' '\n' ELDEF '{' '\n' statements '}' '\n' {
        $$ = new IfNode($3, $7, $13, {});
    }
    | IF '(' expr ')' '{' '\n' statements '}' '\n' ELUND '{' '\n' statements '}' {
        $$ = new IfNode($3, $7, {}, $13);
    }
    | IF '(' expr ')' '{' '\n' statements '}' '\n'
    {
        $$ = new IfNode($3, $7);
    }
;

assign_stmt
: IDENTIFIER ASSIGN expr {
    $$ = new AssignNode($1, $3);
}

decl_stmt
: TYPE IDENTIFIER {
    $$ = new NumberNode(GenericType($1), $2);
} 

expr:
    OPER              { $$ = new OperNode($1, field);  }
    |
    IDENTIFIER '(' expr ')'
    {
        $$ = new FunctionCallNode($1, $3, global_table);
    }
    | expr PLUS expr
    {
        $$ = new BinaryOpNode('+', $1, $3);
    }
    | expr MINUS expr
    {
        $$ = new BinaryOpNode('-', $1, $3);
    }
    | expr XOR expr
    {
        $$ = new BinaryOpNode('^', $1, $3);
    }
    | expr LESS expr
    {
        $$ = new BinaryOpNode('<', $1, $3);
    }
    | expr GREATER expr
    {
        $$ = new BinaryOpNode('>', $1, $3);
    }
    | expr EQUAL expr
    {
        $$ = new BinaryOpNode('=', $1, $3);
    }
    |   NUMBER
    {
        $$ = new NumberNode($1);
    }
    | IDENTIFIER
    {
        $$ = new NumberNode(GenericType(), $1);
    }
;



%%
