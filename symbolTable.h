/**
 * @author Dylan Perdigão     -   2018233092
 * @author Bruno Faria           -   2018295474
 */
#ifndef symbolTable
#define symbolTable

//INCLUDES
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ASTree.h"

typedef int bool;
#define TRUE 1
#define FALSE 0

extern int hasError;

//STRUCT
typedef struct Symbol{
    char* name;
    char* type;
    char* param;
    bool isParameter;
    struct Symbol* inside;
    struct Symbol* last;        //last one inside for lower Complexity!!! O(1) for insert (instead of O(N))
    struct Symbol* next;
}Symbol;


//Function Headers
char* lower(char* str);
char* strcut(char* str,unsigned long begin,unsigned long end);
char* getType(Symbol* global, Symbol* local, Node* n);
char* getOperator(char* op);
//check
int checkOperands(Symbol* global, Symbol* local, Node* n);


Symbol* searchSymbol(Symbol* t, char* name);
Symbol* createSymbolTable(Node* node);
Symbol* newSymbol(char* name,char* type,char* param, bool isParameter);
Symbol* putSymbol(Symbol* t, Symbol* sym);
void printSymbolTable(Symbol* t);

void annotateTreeNode(Node* node, Symbol* local, Symbol* global, int isFirst);
//debug
void printSymbolList(Symbol *node, int depth);

#endif /*symbolTable*/
