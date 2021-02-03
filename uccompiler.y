%{
    /**
    * @author Dylan Perdigão     -   2018233092
    * @author Bruno Faria           -   2018295474
    */
    #include <stdio.h>
    #include "ASTree.h"
    #include "symbolTable.h"
    #include "LLVMgenerator.h"
    #define true 1
    #define false 0
    int yylex(void);
    void yyerror (const char *s);
    extern char* yytext;
    extern int printTree;
    extern int hasError;
    extern int doSemantics;
    extern int printSymTable;
    extern int doGenerate;

    char temp[256];


    extern int line;
    extern int col;
    extern int yyleng;
%}


%type <node> Program
%type <node> FunctionsAndDeclarations
%type <node> FunctionDefinition
%type <node> FunctionBody
%type <node> DeclarationsAndStatements
%type <node> FunctionDeclaration
%type <node> FunctionDeclarator
%type <node> ParameterList
%type <node> recursionPL
%type <node> ParameterDeclaration
%type <node> Declaration
%type <node> recursionD
%type <node> TypeSpec
%type <node> Declarator
%type <node> Statement
%type <node> StatementOrError
%type <node> recursionS
%type <node> Expr
%type <node> FuncArgs

%token  <info> ID
%token  <info> INTLIT
%token  <info> CHRLIT
%token  <info> REALLIT    /* %token  <reallit> REALLIT*/
%token  CHAR
%token  ELSE
%token  WHILE
%token  IF
%token  INT
%token  SHORT
%token  DOUBLE
%token  <info> RETURN
%token  VOID

%token  <info> BITWISEAND
%token  <info> BITWISEOR
%token  <info> BITWISEXOR
%token  <info> AND
%token  <info> ASSIGN
%token  <info> MUL
%token  <info> COMMA
%token  <info> DIV
%token  <info> EQ
%token  <info> GE
%token  <info> GT
%token  LBRACE
%token  <info> LE
%token  LPAR
%token  <info> LT
%token  <info> MINUS
%token  <info> MOD
%token  <info> NE
%token  <info> NOT
%token  <info> OR
%token  <info> PLUS
%token  RBRACE
%token  RPAR
%token  SEMI
%token  RESERVED
%token  LINE_COMMENT
%token  SPACE
%token  NEWLINE



%left   COMMA
%right  ASSIGN
%left   OR
%left   AND
%left   BITWISEOR
%left   BITWISEXOR
%left   BITWISEAND
%left   EQ NE
%left   LE LT GE GT
%left   PLUS MINUS
%left   MUL DIV MOD
%right  NOT
%left   LPAR
%left   RPAR

%nonassoc LOWER
%nonassoc ELSE
%nonassoc HIGHER


%union{
    struct node* node;
    char* id;
    char chrlit;
    int intlit;
    float reallit;

    struct Info{
        char* message;
        int line;
        int col;
    }info;
};

%%
Program                     :   FunctionsAndDeclarations                                    {
                                                                                                $$ = newNode("Program", 1, 1);
                                                                                                $$->child=$1;
                                                                                                //lex and yacc
                                                                                                if (!hasError){
                                                                                                    if (doSemantics){
                                                                                                        Symbol* head = createSymbolTable($$);

                                                                                                        if(printSymTable) printSymbolTable(head);

                                                                                                        //DEBUG
                                                                                                        /*
                                                                                                        printSymbolList(head, 0);
                                                                                                        printf("\n");
                                                                                                        */

                                                                                                        if(!hasError && doGenerate) generate($$,head);
                                                                                                    }
                                                                                                    if(printTree==true) printASTree($$,0);
                                                                                                    else freeASTree($$);

                                                                                                }
                                                                                            }
                            ;
FunctionsAndDeclarations    :   FunctionDefinition                                          {$$=$1;}
                            |   FunctionDeclaration                                         {$$=$1;}
                            |   Declaration                                                 {$$=$1;}
                            |   FunctionsAndDeclarations FunctionDefinition                 {$$=$1; addBrother($$,$2);}
                            |   FunctionsAndDeclarations FunctionDeclaration                {$$=$1; addBrother($$,$2);}
                            |   FunctionsAndDeclarations Declaration                        {$$=$1; addBrother($$,$2);}
                            ;

FunctionDefinition          :   TypeSpec FunctionDeclarator FunctionBody                    {$$ = newNode("FuncDefinition", $1->line, $1->col); $$->child=$1; addBrother($1,$2); addBrother($2,$3);}
                            ;

FunctionBody                :   LBRACE DeclarationsAndStatements RBRACE                     {$$ = newNode("FuncBody", $2->line, $2->col); $$->child=$2;}
                            |   LBRACE RBRACE                                               {$$ = newNode("FuncBody", line, col-yyleng);}
                            ;

DeclarationsAndStatements   :   Statement                                                   {$$=$1;}
                            |   Declaration                                                 {$$=$1;}
                            |   Statement DeclarationsAndStatements                         {if($1) {$$=$1; addBrother($$,$2);} else{$$=$2;}}
                            |   Declaration DeclarationsAndStatements                       {$$=$1; addBrother($$,$2);}

                            ;

FunctionDeclaration         :   TypeSpec FunctionDeclarator SEMI                            {$$ = newNode("FuncDeclaration", $1->line, $1->col); $$->child=$1; addBrother($1,$2);}
                            ;

FunctionDeclarator          :   ID LPAR ParameterList RPAR                                  {sprintf(temp, "Id(%s)", $1.message); $$ = newNode(strdup(temp), $1.line, $1.col); addBrother($$,$3);}
                            ;

ParameterList               :   ParameterDeclaration                                        {$$ = newNode("ParamList", $1->line, $1->col); $$->child=$1;}
                            |   ParameterDeclaration recursionPL                            {$$ = newNode("ParamList", $1->line, $1->col); $$->child=$1; addBrother($1,$2);}
                            ;

recursionPL                 :   COMMA ParameterDeclaration                                  {$$=$2;}
                            |   recursionPL COMMA ParameterDeclaration                      {$$=$1; addBrother($1,$3);}
                            ;

ParameterDeclaration        :   TypeSpec ID                                                 {$$ = newNode("ParamDeclaration", $1->line, $1->col); $$->child=$1;sprintf(temp, "Id(%s)", $2.message);addBrother($1,newNode(strdup(temp), $2.line, $2.col));}
                            |   TypeSpec                                                    {$$ = newNode("ParamDeclaration", $1->line, $1->col); $$->child=$1;}
                            ;

Declaration                 :   TypeSpec Declarator recursionD SEMI                         {$$=newNode("Declaration", line, col-yyleng); $$->child=$1; addBrother($1,$2); addBrother($$,$3);
                                                                                                Node *tempNode, *currentNode = $3;
                                                                                                char *type = $1->message;
                                                                                                while(currentNode){
                                                                                                    tempNode=currentNode->child;
                                                                                                    currentNode->child=newNode(type, line, col-yyleng);
                                                                                                    currentNode->child->brother=tempNode;
                                                                                                    currentNode=currentNode->brother;
                                                                                                }

                                                                                            }
                            |   TypeSpec Declarator SEMI                                    {$$=newNode("Declaration", line, col-yyleng); $$->child=$1; addBrother($1,$2);}
                            |   error SEMI                                                  {$$=newNode(NULL, line, col-yyleng); hasError=true;}
                            ;

recursionD                  :   COMMA Declarator                                            {$$=newNode("Declaration", line, col-yyleng);$$->child=$2;}
                            |   recursionD COMMA Declarator                                 {$$=$1; Node* tempNode=newNode("Declaration", line, col-yyleng);tempNode->child=$3; addBrother($$,tempNode);}
                            ;

TypeSpec                    :   CHAR                                                        {$$ = newNode("Char", line, col-yyleng);}
                            |   INT                                                         {$$ = newNode("Int", line, col-yyleng);}
                            |   VOID                                                        {$$ = newNode("Void", line, col-yyleng);}
                            |   SHORT                                                       {$$ = newNode("Short", line, col-yyleng);}
                            |   DOUBLE                                                      {$$ = newNode("Double", line, col-yyleng);}
                            ;

Declarator                  :   ID ASSIGN Expr                                              {sprintf(temp, "Id(%s)", $1.message);$$ = newNode(strdup(temp), $1.line, $1.col); addBrother($$,$3);}
                            |   ID                                                          {sprintf(temp, "Id(%s)", $1.message);$$ = newNode(strdup(temp), $1.line, $1.col);}
                            ;

Statement                   :   Expr SEMI                                                   {$$=$1;}
                            |   SEMI                                                        {$$=NULL;}
                            |   LBRACE RBRACE                                               {$$=NULL;}
                            |   LBRACE recursionS RBRACE                                    {if($2 && $2->brother){$$=newNode("StatList", line, col-yyleng); $$->child=$2;} else{$$=$2;}}
                            |   IF LPAR Expr RPAR StatementOrError      %prec LOWER         {$$ = newNode("If", line, col-yyleng); $$->child=$3; if(!$5){$5=newNode("Null", line, col-yyleng);} addBrother($3,$5); addBrother($5,newNode("Null", line, col-yyleng));}
                            |   IF LPAR Expr RPAR StatementOrError ELSE StatementOrError    {$$ = newNode("If", line, col-yyleng); $$->child=$3; if(!$5){$5=newNode("Null", line, col-yyleng);} if(!$7){$7=newNode("Null", line, col-yyleng);} addBrother($3,$5);addBrother($5,$7);}
                            |   WHILE LPAR Expr RPAR StatementOrError                       {$$ = newNode("While", line, col-yyleng); $$->child=$3; if(!$5){$5=newNode("Null", line, col-yyleng);} addBrother($3,$5);}
                            |   RETURN Expr SEMI                                            {$$ = newNode("Return", $1.line, $1.col); $$->child=$2;}
                            |   RETURN SEMI                                                 {$$ = newNode("Return", $1.line, $1.col); $$->child=newNode("Null", line, col-yyleng);}
                            |   LBRACE error RBRACE                                         {$$=newNode(NULL, line, col-yyleng); hasError=true;}
                            ;

StatementOrError            :   Statement                                                   {$$=$1;}
                            |   error SEMI                                                  {$$=newNode(NULL, line, col-yyleng); hasError=true;}
                            ;

recursionS                  :   StatementOrError                                            {$$=$1;}
                            |   recursionS StatementOrError                                 {if($1){$$=$1; addBrother($1,$2);} else{$$=$2;}}
                            ;

Expr                        :   Expr ASSIGN Expr                                            {$$ = newNode("Store", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr COMMA Expr                                             {$$ = newNode("Comma", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr PLUS Expr                                              {$$ = newNode("Add", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr MINUS Expr                                             {$$ = newNode("Sub", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr MUL Expr                                               {$$ = newNode("Mul", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr DIV Expr                                               {$$ = newNode("Div", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr MOD Expr                                               {$$ = newNode("Mod", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr OR Expr                                                {$$ = newNode("Or", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr AND Expr                                               {$$ = newNode("And", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr BITWISEAND Expr                                        {$$ = newNode("BitWiseAnd", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr BITWISEOR Expr                                         {$$ = newNode("BitWiseOr", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr BITWISEXOR Expr                                        {$$ = newNode("BitWiseXor", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr EQ Expr                                                {$$ = newNode("Eq", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr NE Expr                                                {$$ = newNode("Ne", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr LE Expr                                                {$$ = newNode("Le", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr GE Expr                                                {$$ = newNode("Ge", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr LT Expr                                                {$$ = newNode("Lt", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   Expr GT Expr                                                {$$ = newNode("Gt", $2.line, $2.col); $$->child=$1; addBrother($1,$3);}
                            |   PLUS Expr                               %prec NOT           {$$ = newNode("Plus", $1.line, $1.col); $$->child=$2;}
                            |   MINUS Expr                              %prec NOT           {$$ = newNode("Minus", $1.line, $1.col); $$->child=$2;}
                            |   NOT Expr                                                    {$$ = newNode("Not", $1.line, $1.col); $$->child=$2;}
                            |   ID LPAR FuncArgs RPAR                                       {$$ = newNode("Call", $1.line, $1.col); sprintf(temp, "Id(%s)", $1.message); $$->child = newNode(strdup(temp), $1.line, $1.col); addBrother($$->child,$3);}
                            |   ID LPAR RPAR                                                {$$ = newNode("Call", $1.line, $1.col); sprintf(temp, "Id(%s)", $1.message); $$->child = newNode(strdup(temp), $1.line, $1.col);}
                            |   ID                                                          {sprintf(temp, "Id(%s)", $1.message);$$ = newNode(strdup(temp), $1.line, $1.col);}
                            |   INTLIT                                                      {sprintf(temp, "IntLit(%s)", $1.message);$$ = newNode(strdup(temp), $1.line, $1.col);}
                            |   CHRLIT                                                      {sprintf(temp, "ChrLit(%s)", $1.message);$$ = newNode(strdup(temp), $1.line, $1.col);}
                            |   REALLIT                                                     {sprintf(temp, "RealLit(%s)", $1.message);$$ = newNode(strdup(temp), $1.line, $1.col);}
                            |   LPAR Expr RPAR                                              {$$ = $2;}
                            |   ID LPAR error RPAR                                          {$$=newNode(NULL, 0, 0); hasError=true;}
                            |   LPAR error RPAR                                             {$$=newNode(NULL, 0, 0); hasError=true;}
                            ;

FuncArgs                    :   Expr                                    %prec LOWER                                  {$$ = $1;}
                            |   FuncArgs COMMA Expr                     %prec HIGHER                                      {$$ = $1; addBrother($$, $3);}

%%
