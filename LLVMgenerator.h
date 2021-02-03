/**
 * @author Dylan Perdigão   -       2018233092
 * @author Bruno Faria         -       2018295474
 */
#ifndef LLVMgenerator_h
#define LLVMgenerator_h

#include <stdio.h>
#include "ASTree.h"
#include "symbolTable.h"

#define GLOBAL 0
#define LOCAL 1
#define INDENTATION "  "

typedef struct variable{
	char* id;
	char* type;
	char* value;
	int temp;
}Variable;

void printVariables(Variable* varList);
Variable searchVar(Variable* varList, char* id);
void addLocalVar(Variable var);
int countTemp(Node* node);
char **setComparison(Node *node);
char *getLLVMOperator(char *str);
int countParams(Node* node);
int getAlign(char* type);
char* getLLVMType(char* type);
char* getValue(Node* n);
char* getIDType(char* str);
char* getID(char* str);
char* getParams(Node* n);
bool generateLoad(char* id);
void generateLabel(int temp);
void generateOperation(Node* node);
void generateReturn(Node* node);
void generateStore(Node* node, int scope);
void generateCall(Node* node);
void generateWhile(Node* node);
void generateIf(Node* node);
void generateDeclarationsAndStatements(Node* node);
void generateFuncBody(Node* node);
void generateDeclaration(Node* node, int scope);
char* allocaFuncParam(Node* node);
char* storeFuncParam(Node* node,int n);
void generateFuncParams(Node* node);
void generateFuncDefinition(Node* node, Symbol* sym);
void generate(Node* node, Symbol* sym);


#endif /* LLVMgenerator_h */
