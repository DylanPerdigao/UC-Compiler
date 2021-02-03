/**
 * @author Dylan Perdigão   -       2018233092
 * @author Bruno Faria         -       2018295474
 */
#include "LLVMgenerator.h"
int 	tempCount=0;
Variable localVar[1024];
Variable globalVar[1024];
bool hasReturn=FALSE;

void printVariables(Variable* varList){
	int i=0;
	while(varList[i].id){
		printf("%s\n",varList[i].id);
		i++;
	}
}

Variable searchVar(Variable* varList, char* id){
	int i=0;
	while(varList[i].id){
		if(!strcmp(varList[i].id,id)){
			return varList[i];
		}
		i++;
	}
	return varList[i];
}
int searchVarIndex(Variable* varList, char* id){
	int i=0;
	while(varList[i].id){
		if(!strcmp(varList[i].id,id)){
			return i;
		}
		i++;
	}
	return -1;
}
void addGlobalVar(Variable var){
	int i=0;
	while(globalVar[i].id){
		i++;
	}
	globalVar[i]=var;
}
void addLocalVar(Variable var){
	int i=0;
	while(localVar[i].id){
		i++;
	}
	localVar[i]=var;
}

int countTemp(Node* node){
	int count=0;
	if(node->child){
		count+=countTemp(node->child);
	}
	if(node->brother){
		count+=countTemp(node->brother);
	}
	if(!strcmp(strcut(node->message,0,3),"Id(")){//printf("--->id\n");
		count++;
	}else if(!strcmp(strcut(node->message,0,4),"Call")){//printf("--->call\n");
		count++;
	}else if(getLLVMOperator(node->message)){
		count++;
	}
	return count;
}
char **setComparison(Node *node){
	//alloca memoria
	char **comparison;
	comparison = (char**) malloc(sizeof(char*));
	*comparison = (char*) malloc(strlen(getLLVMOperator(node->message)+1));
	*(comparison+1) = (char*) malloc(strlen(getLLVMType(getIDType(node->child->brother->message)))+1);
	if(*comparison){
		*(comparison+2) = (char*) malloc(strlen(getValue(node->child->brother->brother))+1);
	}else{
		*(comparison+2) = (char*) malloc(strlen("0")+1);
		*comparison = (char*) malloc(strlen("ne")+1);
	}
	//atribuir valores
	strcpy(*comparison,getLLVMOperator(node->message));
	*(comparison+1)=getLLVMType(getIDType(node->child->brother->message));
	if(*comparison){
		*(comparison+2) = getValue(node->child->brother);
	}else{
		*(comparison+2)="0";
		*comparison="ne";
	}
	return comparison;
} 

char *getLLVMOperator(char *str){
	if(!strcmp(strcut(str,0,3),"Add")){
		return "add";
	}else if(!strcmp(strcut(str,0,5),"Minus")){
		return "sub";
	}else if(!strcmp(strcut(str,0,3),"Mul")){
		return "mul";
	}else if(!strcmp(strcut(str,0,3),"Div")){
		return "sdiv";
	}else if(!strcmp(strcut(str,0,3),"Mod")){
		return "srem";
	}else if(!strcmp(strcut(str,0,3),"Not")){
		return "fneg";
	}else if(!strcmp(strcut(str,0,3),"And")){
		return "and";
	}else if(!strcmp(strcut(str,0,2),"Or")){
		return "or";
	}else if(!strcmp(strcut(str,0,10),"BitWiseAnd")){
		return "and";
	}else if(!strcmp(strcut(str,0,10),"BitWiseOr")){
		return "or";
	}else if(!strcmp(strcut(str,0,10),"BitWiseXor")){
		return "xor";
	}if(!strcmp(strcut(str,0,2),"Eq")){
		return "eq";
	}else if(!strcmp(strcut(str,0,2),"Ne")){
		return "ne";
	}else if(!strcmp(strcut(str,0,2),"Lt")){
		return "slt";
	}else if(!strcmp(strcut(str,0,2),"Gt")){
		return "sgt";
	}else if(!strcmp(strcut(str,0,2),"Le")){
		return "sle";
	}else if(!strcmp(strcut(str,0,2),"Ge")){
		return "sge";
	}
	return NULL;
}

int countParams(Node* node){
	int i=0;
	while(node){	
		i++;
		node=node->brother;
	}
	return i;
}

int getAlign(char* type){
	if(type){
		if(!strcmp(type,"i8")){
			return 1;
		}else if(!strcmp(type,"i32")){
			return 4;
		}else if(!strcmp(type,"double")){
			return 8;
		}
	}
	return -1;
}

char* getLLVMType(char* type){
	char* aux = lower(type);
	if(!strcmp(aux,"int") || !strcmp(aux,"short") || !strcmp(aux,"char") ){
		return "i32";
	}else if(!strcmp(aux,"double")){
		return "double";
	}else{
		return NULL;
	}
}

char getEscapeChar(char c){
	switch(c){
		case 'n':	return '\n';
		case 't':	return '\t';
	}
	return 0;
}

char* getValue(Node* n){
	char* str;
	if(n){
		if(!strcmp(strcut(n->message,0,6),"ChrLit")){
				char* aux = malloc(strlen(n->message)+1);
				strcpy(aux,n->message);
				strtok(aux,"'");
				aux = strtok(NULL, "'");
				str = (char*) malloc(atoi(aux)*sizeof(char)+1);
				if(*aux=='\\'){
					sprintf(str,"%d",getEscapeChar(*(aux+1)));
				}else{
					sprintf(str,"%d",*aux);
				}
				
		}else if(!strcmp(strcut(n->message,0,6),"IntLit")){
			str = (char*) malloc(strlen(strtok(strcut(n->message,7,strlen(n->message)),")"))+1);
			sprintf(str,"%s",strtok(strcut(n->message,7,strlen(n->message)),")"));
		}else{
			str=(char*)malloc(2);
			str="0";
		}
	}else{
		str=(char*)malloc(2);
		str="0";
	}
	return str;
}

char* getIDType(char* str){
	char* aux = malloc(strlen(str)+1);
	strcpy(aux,str);
	strtok(aux," - ");
	aux = strtok(NULL, " - ");
	//printf("§§§>%s\n",aux);
	return aux;
}

char* getID(char* str){
	return strtok(strcut(str,3,strlen(str)),")");
}

char* getParams(Node* n){
	char* params = (char*) malloc(sizeof(char));
	while(n){
		char*aux = lower(n->child->message);
		if(!strcmp(aux,"int") || !strcmp(aux,"short") || !strcmp(aux,"char") || !strcmp(aux,"double")){
			params = (char*) realloc(params,strlen(params)+strlen(aux)+3);
			strcat(params, getLLVMType(aux));
			strcat(params,", ");
		}
		n=n->brother;
	}
	//dá segmentation fault de fazer diretamente -2
	char* str = strcut(params,0,strlen(params)-1);
	return strcut(str,0,strlen(str)-1);
}
bool generateLoad(char* id){
	Variable var = searchVar(localVar,id);
	if(var.id){
		tempCount++;
		printf("%s%%%i = load %s, %s* %%%i, align %i\n",INDENTATION,tempCount,var.type, var.type, var.temp,getAlign(var.type));
		return TRUE;
	}
	return FALSE;
}

void generateLabel(int temp){
	printf("%i:\n",temp);
}
void generateOperation(Node* node){
	//node=Add/Minus/Mul/Div/...
	char *op=getLLVMOperator(node->message);
	char *type=getLLVMType(getIDType(node->message));
	generateLoad(getID(node->child->message));
	bool isVariable = generateLoad(getID(node->child->brother->message));
	tempCount++;
	if(isVariable){
		printf("%s%%%i = %s %s %%%i, %%%i\n",INDENTATION,tempCount,op,type,tempCount-2,tempCount-1);
	}else{
		printf("%s%%%i = %s %s %%%i, %s\n",INDENTATION,tempCount,op,type,tempCount-1,getValue(node->child->brother));
	}
	
}

void generateReturn(Node* node){
	//	node=Return
	/*
	tempCount++;
	char* idType = getLLVMType(getIDType(node->child->message));
	char* id = getID(node->child->message);
	printf("%s%%%i = load %s, %s* @%s, align %i\n",INDENTATION,tempCount,idType, idType, id,getAlign(idType));
	*/
	printf("%sret %s %s\n",INDENTATION,getLLVMType(getIDType(node->child->message)), getValue(node->child));
}

void generateStore(Node* node, int scope){
	//	node=store
	char* type = getLLVMType(strcut(node->message,8,strlen(node->message)+1));
	char* val = getValue(node->child->brother);
	char* idType = getLLVMType(getIDType(node->child->message));
	char* id = getID(node->child->message);
	char* op = getLLVMOperator(node->child->brother->message);
	if(scope==GLOBAL){
		printf("%sstore %s %s, %s* @%s, align %i\n",INDENTATION,type,val, idType, id,getAlign(idType));
	}else{
		Variable var = searchVar(localVar,id);
		if(op){
			//caso de ter uma operacao
			generateOperation(node->child->brother);
			printf("%sstore %s %%%i, %s* %%%i, align %i\n",INDENTATION,var.type,tempCount, var.type, var.temp,getAlign(var.type));
		}else{
			if(!var.id){
				var = searchVar(globalVar,id);
				printf("%sstore %s %s, %s* @%s, align %i\n",INDENTATION,var.type,val, var.type, var.id,getAlign(var.type));
			}else{
				printf("%sstore %s %s, %s* %%%i, align %i\n",INDENTATION,var.type,val, var.type, var.temp,getAlign(var.type));
			}
		}
		
	}
}

void generateCall(Node* node){
	//	node=Call
	bool isVariable = generateLoad(getID(node->child->brother->message));
	tempCount++;
	if(!strcmp(getID(node->child->message),"putchar")){
		if(isVariable){
			printf("%s%%%i = call i32 (i32, ...) bitcast (i32 (...)* @putchar to i32 (i32, ...)*)(i32 %%%i)\n",INDENTATION,tempCount,tempCount-1);
		}else{
			printf("%s%%%i = call i32 (i32, ...) bitcast (i32 (...)* @putchar to i32 (i32, ...)*)(i32 %s)\n",INDENTATION,tempCount,getValue(node->child->brother));
		}
	}else{
		//TODO
	}
}


void generateWhile(Node* node){
	//	node=While
	char **comparison = setComparison(node->child);
	int label_cond=++tempCount;
	int label_exit=tempCount+countTemp(node);
	printf("%sbr label %%%i\n\n",INDENTATION,label_cond);
	//LABEL PARA A CONDICAO DE PARAGEM DO CICLO
	generateLabel(tempCount);
	generateLoad(getID(node->child->child->message));
	
	tempCount++;
	printf("%s%%%i = icmp %s %s %%%i, %s\n",INDENTATION,tempCount,comparison[0],getLLVMType(getIDType(node->child->message)), tempCount-1,comparison[2]);
	int label_loop=++tempCount;
	printf("%sbr i1 %%%i, label %%%i, label %%%i\n\n",INDENTATION,label_loop-1, label_loop, label_exit);
	//LABEL PARA A STATLIST
	generateLabel(label_loop);
	generateDeclarationsAndStatements(node->child->brother);
	printf("%sbr label %%%i\n\n",INDENTATION,label_cond);
	//LABEL PARA CONTINUAR
	tempCount++;
	generateLabel(tempCount);
}

void generateIf(Node* node){
	//	node=If
	char **comparison = setComparison(node->child);
	//LABEL PARA A CONDICAO DO IF
	char* op = getLLVMOperator(node->child->message);
	if(op){
		generateOperation(node->child);
		strcpy(comparison[0],"ne\0");
	}
	int label_block2=tempCount+countTemp(node->child->brother);
	tempCount++;
	printf("%s%%%i = icmp %s %s %%%i, %s\n",INDENTATION,tempCount,comparison[0],getLLVMType(getIDType(node->child->message)), tempCount-1,comparison[2]);
	int label_block1=++tempCount;	
	int label_exit=tempCount+countTemp(node->child->brother);				
	printf("%sbr i1 %%%i, label %%%i, label %%%i\n\n",INDENTATION,label_block1-1, label_block1, label_block2);
	//LABEL PARA O BLOCO 1 - IF
	generateLabel(label_block1);
	generateDeclarationsAndStatements(node->child->brother);
	printf("%sbr label %%%i\n\n",INDENTATION,label_exit);
	//LABEL PARA O BLOCO 2 - ELSE
	if(node->child->brother->brother){
		generateLabel(label_block2);
		tempCount++;
		generateDeclarationsAndStatements(node->child->brother->brother);
		printf("%sbr label %%%i\n\n",INDENTATION,label_exit);
	}
	generateLabel(label_exit);
	tempCount++;
}
void generateDeclarationsAndStatements(Node* node){
	//printf("-->%s\n",node->message);
	//STATLIST
	if(!strcmp(strcut(node->message,0,8),"StatList")){
		Node* statlist = node->child;
		while(statlist){
			generateDeclarationsAndStatements(statlist);
			statlist=statlist->brother;
		}
	}else if(!strcmp(node->message,"Declaration")){
		generateDeclaration(node, LOCAL);
	//STORE
	}else if(!strcmp(strcut(node->message,0,5),"Store")){
		generateStore(node, LOCAL);
	//CALL
	}else if(!strcmp(strcut(node->message,0,4),"Call")){
		generateCall(node);
	//IF
	}else if(!strcmp(node->message,"If")){
		generateIf(node);
	//WHILE
	}else if(!strcmp(node->message,"While")){
		generateWhile(node);
	//RETURN
	}else if(!strcmp(node->message,"Return")){
		hasReturn=TRUE;
		generateReturn(node);
	}
}
void generateFuncBody(Node* node){
	//	node=FuncBody
	if(node->child){
		//	node=FuncBody->childs
		node=node->child;
		while(node){
			generateDeclarationsAndStatements(node);
			node=node->brother;
		}		
	}
}

void generateDeclaration(Node* node, int scope){
	//	node=Declaration
	char* id = strcut(node->child->brother->message,3,strlen(node->child->brother->message)-1);
	if(scope==GLOBAL){
		Variable var;
		var.id = id;
		var.type = getLLVMType(node->child->message);
		var.value = getValue(node->child->brother->brother);
		addGlobalVar(var);
		printf("@%s = common global %s %s, align %i\n",var.id,var.type,var.value,getAlign(var.type));
	}else{
		int index = searchVarIndex(localVar,id);
		if(node->child->brother->brother){
			localVar[index].value = getValue(node->child->brother->brother);
		}else{
			localVar[index].value=NULL;
		}
		Variable var = searchVar(localVar,id);
		if(localVar[index].value){
			printf("%sstore %s %s, %s* %%%i, align %i\n",INDENTATION,var.type,var.value,var.type,var.temp,getAlign(var.type));
		}
	}
}

char* allocaFuncParam(Node* node){
	//	node=ParamDeclaration
	char* string="";
	while(node){
		char* type = getLLVMType(node->child->message);
		//porque pode ser void e nao é preciso printar nada
		if(type){
			char aux[1000];
			int align = getAlign(type);
			tempCount++;
			sprintf(aux,"%s%s%%%i = alloca %s, align %i\n",string,INDENTATION,tempCount,type,align);
			string=(char*) malloc(strlen(string)+strlen(aux)+1);
			strcpy(string,aux);
		}
		node=node->brother;
	}
	return string;
}
char* storeFuncParam(Node* node,int n){
	//	node=ParamDeclaration
	char* string="";
	while(node){
		char* type = getLLVMType(node->child->message);
		//porque pode ser void e nao é preciso printar nada
		if(type){
			int align = getAlign(type);
			tempCount++;
			char aux[1000];
			sprintf(aux,"%s%sstore %s %%%i, %s* %%%i, align %i\n",string,INDENTATION,type,tempCount-n,type,tempCount+n-1,align);
			string=(char*) malloc(strlen(string)+strlen(aux)+1);
			strcpy(string,aux);
		}
		node=node->brother;
	}
	return string;
}
void generateFuncParams(Node* node){
		//	node=ParamList
		if(node){
			//O store tem de ser executado antes de alloca mas no print é ao contrario para ter os bons temporarios
			int n=countParams(node->child);
			int aux = tempCount;
			char *aux1 = (char*) malloc(strlen(storeFuncParam(node->child,n)));
			char *aux2 = (char*) malloc(strlen(allocaFuncParam(node->child)));
			tempCount = aux;
			aux1=storeFuncParam(node->child,n);
			tempCount--;
			aux2=allocaFuncParam(node->child);
			printf("%s%s",aux2,aux1);
		}
}

void generateAlloca(Node* node, Symbol* sym){
	//node=FuncDefinition
	Symbol* s=sym->inside;
	while(s){
		//verificar se diferente de return
		if(strcmp(s->name,"return")){
			Variable var;
			var.id = s->name;
			var.type = getLLVMType(s->type);
			var.value = NULL;
			var.temp = ++tempCount;
			addLocalVar(var);
			printf("%s%%%i = alloca %s, align %i\n",INDENTATION,var.temp,var.type,getAlign(var.type));
		}
		s=s->next;
	}
}

void generateFuncDefinition(Node* node, Symbol* sym){
	hasReturn=FALSE;
	//	node=FuncDefinition
	char* funcType = getLLVMType(node->child->message);
	char* funcName = strcut(node->child->brother->message,3,strlen(node->child->brother->message)-1);
	char* funcParams = getParams(node->child->brother->brother->child);
	printf("define %s @%s(%s) #0 {\n",funcType,funcName,funcParams);
	generateAlloca(node,sym);
	//generateFuncParams(node->child->brother->brother);
	generateFuncBody(node->child->brother->brother->brother);
	if(!hasReturn){
		printf("%sret i32 0\n",INDENTATION);
	}
	printf("}\n\n");
}


void generate(Node* node, Symbol* sym){
	//	node=Program
	if(node->child){
		node=node->child;
		//node=Program->childs
		while(node){
			//DECLARATION
			if(!strcmp(node->message,"Declaration")){
				generateDeclaration(node, GLOBAL);
			//FUNCTION DEFINITION
			}else if(!strcmp(node->message,"FuncDefinition")){
				generateFuncDefinition(node,searchSymbol(sym,getID(node->child->brother->message)));
			}
			//printVariables(localVar);
			node=node->brother;
		}
	}
	printf("declare i32 @putchar(...) #1\n");
}
