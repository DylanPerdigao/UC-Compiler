/**
 * @author Dylan Perdigão   -       2018233092
 * @author Bruno Faria         -       2018295474
 */
#include "symbolTable.h"

/** Converte as maiusculas para minusculas de uma strng
 * @param str      String de entrada
 * @return      Retorna a string inicial convertida
 */
char* lower(char* str){
    char* aux = (char*) malloc(strlen(str) + 1);
    for(int i = 0; i<strlen(str); i++){
      aux[i] = tolower(str[i]);
    }
    aux[strlen(str)]='\0';
    return aux;
}
/** Cria  uma sub-string de um indice inicial para um indide final exclusivo [begin;end[
 * @param str          String para ser fateada
 * @param begin     Indice de inicio INCLUSIVO
 * @param end          Indice de fim EXCLUSIVO
 * @return       String cortada
 */
char* strcut(char* str,unsigned long begin,unsigned long end){
    if(begin<end){
        char* aux = (char*) malloc(end-begin+1);
        for (int i=0; i<end-begin+1; i++){
            aux[i]= str[i+begin];
        }
        aux[end-begin]='\0';
        return aux;
    }else{
        return NULL;
    }
}
/** Converte um operador em letras para um operador em simbolo
 * @param op           String do operador em letras
 * @return       String do operador em simbolo(s)
 */
char* getOperator(char* op){
    if(strcmp(op,"Add")==0){
        return "+";
    }else if(strcmp(op,"Sub")==0){
        return "-";
    }else if(strcmp(op,"Mul")==0){
        return "*";
    }else if(strcmp(op,"Div")==0){
        return "/";
    }else if(strcmp(op,"Mod")==0){
        return "%";
    }else if(strcmp(op,"And")==0){
        return "&&";
    }else if(strcmp(op,"Or")==0){
        return "||";
    }else if(strcmp(op,"BitWiseAnd")==0){
        return "&";
    }else if(strcmp(op,"BitWiseOr")==0){
        return "|";
    }else if(strcmp(op,"BitWiseXor")==0){
        return "^";
    }else if(strcmp(op,"Eq")==0){
        return "==";
    }else if(strcmp(op,"Ne")==0){
        return "!=";
    }else if(strcmp(op,"Lt")==0){
        return "<";
    }else if(strcmp(op,"Gt")==0){
        return ">";
    }else if(strcmp(op,"Le")==0){
        return "<=";
    }else if(strcmp(op,"Ge")==0){
        return ">=";
    }
    //printf("%s\n",op);
    return NULL;
}
char* getType(Symbol* global, Symbol* local, Node* n){
    Symbol* sym = NULL;
    char* str = n->message;
    char* aux = lower(strcut(str,0,3));
    ///CHAR
    if(strcmp(aux,"cha")==0){
        return "char";
    ///CHRLIT
    }else if(strcmp(aux,"chr")==0){
        return "int";
    ///CALL
    }else if(strcmp(aux,"cal")==0){
        sym = searchSymbol(global,strcut(n->child->message,3,strlen(n->child->message)-1));
        if(!sym) sym = searchSymbol(local,strcut(n->child->message,3,strlen(n->child->message)-1));
        if(sym){
            return sym->type;
        }else{
            return NULL;
        }
    ///INTLIT
    }else if(strcmp(aux,"int")==0){
        return "int";
    ///ID
    }else if(strcmp(aux,"id(")==0){
        sym = searchSymbol(local,strcut(str,3,strlen(str)-1));
        if (!sym) sym = searchSymbol(global,strcut(str,3,strlen(str)-1));
        if(sym){
            //printf("%s\n",sym->name);
            if(sym->param){
                char* funcStr = malloc(strlen(sym->type)+strlen(sym->param)+3);
                strcat(funcStr,sym->type);
                strcat(funcStr,"(");
                strcat(funcStr,sym->param);
                strcat(funcStr,")");
                return funcStr;
            }else{
                return sym->type;
            }
        }else{
           return NULL;
        }
    ///NULL
    }else if(strcmp(aux,"nul")==0){
        return "void";
    ///REALLIT
    }else if(strcmp(aux,"rea")==0){
        return "double";
    ///SHORT
    }else if(strcmp(aux,"sho")==0){
        return "short";
    ///STORE
    }else if(strcmp(aux,"sto")==0){
        sym = searchSymbol(local,strcut(n->child->message,3,strlen(n->child->message)-1));
        if(!sym) sym = searchSymbol(global,strcut(n->child->message,3,strlen(n->child->message)-1));
        if(sym){
            return sym->type;
        }else{

            return getType(global, local ,n->child);
        }
    ///COMMA
    }else if(strcmp(aux,"com")==0){
        if(n->child && n->child->brother)   return getType(global, local, n->child->brother);
        else return "ERRO!!";
    ///LOGIC
    }else if(   !strcmp(str,"Mod")          ||
                !strcmp(str,"Not")          ||
                !strcmp(str,"And")          ||
                !strcmp(str,"Or")           ||
                !strcmp(str,"BitWiseAnd")   ||
                !strcmp(str,"BitWiseOr")    ||
                !strcmp(str,"BitWiseXor")   ||
                !strcmp(str,"Eq")           ||
                !strcmp(str,"Ne")           ||
                !strcmp(str,"Lt")           ||
                !strcmp(str,"Gt")           ||
                !strcmp(str,"Le")           ||
                !strcmp(str,"Ge")           ){
        return "int";
    ///OPERADORES
    }else{
        if(n->child){
            if(n->child->brother){
                char* first = getType(global, local, n->child);
                char* second = getType(global, local, n->child->brother);
                if (!first || !second) return NULL;
                if (!strcmp(first, second)) return first;
                else if(!strcmp(first, "undef") || !strcmp(second, "undef") || !strcmp(first, "void") || !strcmp(second, "void"))    return "undef";
                else if(!strcmp(first, "double")|| !strcmp(second, "double"))   return "double";
                else if(!strcmp(first, "int")   || !strcmp(second, "int"))      return "int";
                else if(!strcmp(first, "short") || !strcmp(second, "short"))    return "short";
                else if(!strcmp(first, "char")  || !strcmp(second, "char"))     return "char";
                //else
                return "ERRO!!";
            }else if(strcmp(aux,"plu")==0 || strcmp(aux,"min")==0){
                char* first = getType(global, local, n->child);
                if (first) return first;
            }
        return NULL;
        }
    }
    return NULL;
}
/*
 ***********************
 ****** ERROS **********
 ***********************
 */
int checkOperands(Symbol* global, Symbol* local, Node* n){
    char* operator = getOperator(n->message);
    char* op1 = getType(global, local, n->child);
    if(!op1){op1="BUG_OP1_NULL";}
    char* op2 = getType(global, local, n->child->brother);
    if(!op2){op2="BUG_OP2_NULL";}
    if(operator){
        if (strcmp(op1,"int")!=0 && strcmp(op2,"int")!=0){
            printf("Line %d, col: %d : Operator %s cannot be applied to types %s, %s\n",1,1,operator,op1,op2);
            return TRUE;
        }else if (strcmp(op1,"int")!=0){
            printf("Line %d, col: %d : Operator %s cannot be applied to type %s\n",1,1,operator,op1);
            return TRUE;
        }else if (strcmp(op2,"int")!=0){
            printf("Line %d, col: %d : Operator %s cannot be applied to type %s\n",1,1,operator,op2);
            return TRUE;
        }
    }
    return FALSE;
}

int checkExistingSymbol(Symbol* t, Symbol* sym){
   if(searchSymbol(t, sym->name)){
        printf("Line %d, col: %d : Symbol %s already defined\n",1,1,sym->name);
        return TRUE;
    }
    return FALSE;
}
int checkVoidUseInDeclaration(char* type){
    if(strcmp(type, "void")==0){
        printf("Line %d, col: %d : Invalid use of void type in declaration\n",1,1);
        return TRUE;
    }
    return FALSE;
}
int checkLValue(Node* n){
    if(strcmp(n->message, "Store")==0){
        //printf("%s\n",strcut(n->child->message,0,3));
        if(strcmp(strcut(n->child->message,0,3), "Id(")!=0){
            printf("Line %d, col: %d : Lvalue required\n",1,1);
            return TRUE;
        }
    }
    return FALSE;
}
int checkUnknownSymbol(Symbol* global,Symbol* local,Node* n){
    if(n){
        char* str = n->message;
        char* id = strcut(str,3,strlen(str)-1);
        if(!strcmp(strcut(str,0,3), "Id(")){
            printf("%s\n",id);
            if(!searchSymbol(global, id)){
                if(!(local && searchSymbol(local->inside, id))){
                    printf("Line %d, col: %d : Unknown symbol %s\n",1,1,id);
                    return TRUE;
                }
            }
        }
        //checkUnknownSymbol(global,local,n->child);
        //checkUnknownSymbol(global,local,n->brother);
    }
    return FALSE;
}


Symbol* searchSymbol(Symbol* t, char* name){
    while(t && name){
        if(t->name && strcmp(t->name,name)==0){
            return t;
        }
        t=t->next;
    }
    return NULL;
}



/** Alloca memoria para a criação do simbolo novo
 * @param name      Nome do simbolo
 * @param type      Tipo de retorno do simbolo
 * @param param     Lista dos tipos dos parametros do simbolo
 * @return          Retorna o simbolo
 */
Symbol* newSymbol(char* name,char* type,char* param, bool isParameter){
    Symbol* aux = (Symbol*) malloc(sizeof(Symbol));

    aux->name = name;
    aux->type = type;
    if(param){
        aux->param = param;
    }
    aux->isParameter=isParameter;
    aux->inside = NULL;
    aux->last = NULL;
    aux->next = NULL;

    return aux;
}


/** Meter um simbolo no fim da lista
 * @param last     Lista
 * @param sym       Novo simbolo para meter no fim da lista
 * @return      Lista com o novo simbolo
 */
Symbol* insertSymbol(Symbol* last, Symbol* sym){
    //checkExistingSymbol(last,sym);
    last->next = sym;
    return sym;
}


Symbol* createSymbolTable(Node* AST){
    Symbol* head = newSymbol("putchar", "int", "int", FALSE);
    Symbol* last = head;
    last = insertSymbol(last, newSymbol("getchar", "int", "void", FALSE));
    if (AST && AST->child){
        Node* FuncDecl = AST->child;
        while(FuncDecl){
            if(strcmp(FuncDecl->message,"FuncDefinition")==0){
                Node* funcDef = FuncDecl->child;
                char* type = lower(funcDef->message);
                char* name = strcut(funcDef->brother->message,3,strlen(funcDef->brother->message)-1);
                char* param = (char*) malloc(sizeof(char));
                Symbol* current = searchSymbol(head, name);
                if (current){   //já declarada
                    while(funcDef){
                        if(strcmp(funcDef->message,"ParamList")==0){
                            Node* params = funcDef->child;
                            current->inside=newSymbol("return", type, NULL, FALSE);
                            current->last=current->inside;
                            //Percorre os argumentos todos para meter na string param
                            while(params){

                                if(params->child->brother){
                                    Node* declaration = params->child;
                                    char* declarationType = lower(declaration->message);
                                    char* declarationName = strcut(declaration->brother->message,3,strlen(declaration->brother->message)-1);
                                    current->last = insertSymbol(current->last, newSymbol(declarationName, declarationType, NULL, TRUE));

                                }

                                param = (char*) realloc(param,strlen(param)+strlen(params->child->message)+2);
                                strcat(param, lower(params->child->message));
                                strcat(param,",");

                                params=params->brother;
                            }
                            param = strcut(param,0,strlen(param)-1);
                            current->param = param;
                            //last = insertSymbol(last, newSymbol(name, type, param));
                        }else if(strcmp(funcDef->message,"FuncBody")==0){
                            Node* funcBody = funcDef->child;
                            while(funcBody){
                                //checkUnknownSymbol(head,current->inside,funcBody);
                                if(strcmp(funcBody->message,"Declaration")==0){
                                    Node* declaration = funcBody->child;
                                    char* declarationType = lower(declaration->message);
                                    char* declarationName = strcut(declaration->brother->message,3,strlen(declaration->brother->message)-1);
                                    //checkVoidUseInDeclaration(declarationType);
                                    if (!searchSymbol(current->inside, declarationName)){
                                        current->last = insertSymbol(current->last, newSymbol(declarationName, declarationType, NULL, FALSE));
                                    }//else //já existe

                                    if (declaration->brother->brother){
                                        annotateTreeNode(declaration->brother->brother, current->inside, head,1);
                                    }

                                }else if(strcmp(funcBody->message,"Return")==0){
                                    //char* returnType = getType(head, current->inside,funcBody->child);

                                    //checkOperands(current->inside, funcBody->child);

                                    //DEVE SER ERRO/WARNING!!!
                                    //TODO: verificar se o returnType == last->inside type
                                    annotateTreeNode(funcBody->child, current->inside, head, 1);
                                }
                                else{
                                    //checkLValue(funcBody);
                                    annotateTreeNode(funcBody, current->inside, head, 1);
                                }
                                funcBody=funcBody->brother;
                            }
                        }
                        funcDef=funcDef->brother;
                    }
                }
                else{
                    while(funcDef){
                        if(strcmp(funcDef->message,"ParamList")==0){
                            Node* params = funcDef->child;
                            last = insertSymbol(last, newSymbol(name, type, NULL, FALSE));
                            last->inside=newSymbol("return", type, NULL, FALSE);
                            last->last=last->inside;
                            //Percorre os argumentos todos para meter na string param
                            while(params){
                                if(params->child->brother){
                                    Node* declaration = params->child;
                                    char* declarationType = lower(declaration->message);
                                    char* declarationName = strcut(declaration->brother->message,3,strlen(declaration->brother->message)-1);

                                    last->last = insertSymbol(last->last, newSymbol(declarationName, declarationType, NULL, TRUE));

                                }

                                param = (char*) realloc(param,strlen(param)+strlen(params->child->message)+2);
                                strcat(param, lower(params->child->message));
                                strcat(param,",");

                                params=params->brother;
                            }
                            param = strcut(param,0,strlen(param)-1);
                            last->param = param;

                        }else if(strcmp(funcDef->message,"FuncBody")==0){
                            Node* funcBody = funcDef->child;
                            while(funcBody){
                                //checkUnknownSymbol(head,last->inside,funcBody);
                                if(strcmp(funcBody->message,"Declaration")==0){
                                    Node* declaration = funcBody->child;
                                    char* declarationType = lower(declaration->message);
                                    char* declarationName = strcut(declaration->brother->message,3,strlen(declaration->brother->message)-1);
                                    //checkVoidUseInDeclaration(declarationType);
                                    if (!searchSymbol(last->inside, declarationName)){
                                        last->last = insertSymbol(last->last, newSymbol(declarationName, declarationType, NULL, FALSE));
                                    }//else //já existe

                                    if (declaration->brother->brother){
                                        annotateTreeNode(declaration->brother->brother, last->inside, head,1);
                                    }

                                }else if(strcmp(funcBody->message,"Return")==0){
                                    //char* returnType = getType(head, last->inside,funcBody->child);

                                    //checkOperands(last->inside, funcBody->child);

                                    //DEVE SER ERRO/WARNING!!!
                                    //TODO: verificar se o returnType == last->inside type
                                    annotateTreeNode(funcBody->child, last->inside, head, 1);
                                }
                                else{
                                    //checkLValue(funcBody);
                                    annotateTreeNode(funcBody, last->inside, head, 1);
                                }
                                funcBody=funcBody->brother;
                            }
                        }
                        funcDef=funcDef->brother;
                    }
                }
            }else if(strcmp(FuncDecl->message,"Declaration")==0){
                Node* declaration = FuncDecl->child;
                char* declarationType = lower(declaration->message);
                char* declarationName = strcut(declaration->brother->message,3,strlen(declaration->brother->message)-1);
                //checkVoidUseInDeclaration(declarationType);
                //checkUnknownSymbol(head,NULL,FuncDecl);
                Symbol* current = searchSymbol(head, declarationName);

                if (!current){
                    last = insertSymbol(last, newSymbol(declarationName, declarationType, NULL, FALSE));
                }else {
                    //redeclaração!!!
                    current->type = declarationType;
                }

                if (declaration->brother->brother){
                    annotateTreeNode(declaration->brother->brother, last->inside, head,1);
                }
            }
            else if(strcmp(FuncDecl->message,"FuncDeclaration")==0){
                Node* funcDef = FuncDecl->child;
                char* type = lower(funcDef->message);
                char* name = strcut(funcDef->brother->message,3,strlen(funcDef->brother->message)-1);
                char* param = (char*) malloc(sizeof(char));

                Node* params = funcDef->brother->brother->child;
                //Percorre os argumentos todos para meter na string param


                while(params){

                    /*
                    if(params->child->brother){
                        Node* declaration = params->child;
                        char* declarationType = lower(declaration->message);
                        char* declarationName = strcut(lower(declaration->brother->message),3,strlen(declaration->brother->message)-1);
                        last->last = insertSymbol(last->last, newSymbol(declarationName, declarationType, ".."));

                    }
                    */

                    param = (char*) realloc(param,strlen(param)+strlen(params->child->message)+2);
                    strcat(param, lower(params->child->message));
                    strcat(param,",");
                    //checkVoidUseInDeclaration(lower(params->child->message));
                    params=params->brother;
                }


                param = strcut(param,0,strlen(param)-1);
                if (!searchSymbol(head, name)){

                    last = insertSymbol(last, newSymbol(name, type, param, FALSE));
                    last->inside=newSymbol(NULL, NULL, NULL, FALSE);
                }//else já existe
            }
            FuncDecl=FuncDecl->brother;
        }
    }
    return head;
}


void printSymbolTable(Symbol* t){
    Symbol* copy = t;
    printf("===== Global Symbol Table =====\n");
    //Percorrer simbolos
    while(t){
        //Print diferente caso tenha ou nao parametros
        if(t->param){
            printf("%s\t%s(%s)\n",t->name,t->type,t->param);
        }else{
            printf("%s\t%s\n",t->name,t->type);
        }
        t=t->next;
    }
    //Percorrer Funcoes
     while(copy){
        if(copy->inside && copy->inside->name){
            printf("\n===== Function %s Symbol Table =====\n",copy->name);
            Symbol* sym = copy->inside;
            while(sym){
                //Print diferente caso tenha ou nao parametros
                if(sym->isParameter){
                    printf("%s\t%s\tparam\n",sym->name,sym->type);
                }else{
                    printf("%s\t%s\n",sym->name,sym->type);
                }
                sym=sym->next;
            }
        }
        copy=copy->next;
    }
    printf("\n");
}

void annotateTreeNode(Node* node, Symbol* local, Symbol* global, int isFirst){
    if (strcmp(node->message, "While")      &&
        strcmp(node->message, "StatList")   &&
        strcmp(node->message, "If")         &&
        strcmp(node->message, "Else")       &&
        strcmp(node->message, "Return")     &&
        strcmp(node->message, "Null")       ){

        char* separator = " - ";
        char* type = getType(global, local, node);

        if(type!=NULL){
            char* newMessage = (char*) malloc(strlen(node->message) + strlen(type) + 4);
            strcat(newMessage,node->message);
            strcat(newMessage,separator);
            strcat(newMessage,type);
            node->message= newMessage;
        }else{
            char* newMessage = (char*) malloc(strlen(node->message) + 9);
            strcat(newMessage,node->message);
            strcat(newMessage,separator);
            strcat(newMessage,"undef");
            node->message= newMessage;
        }
    }

    if (node->child)    annotateTreeNode(node->child,local, global, 0);
    if(isFirst==0){
        if (node->brother)  annotateTreeNode(node->brother,local, global, 0);
    }
}


//DEBUG
void printSymbolList(Symbol *node, int depth){
    char dots[depth*2];
    for (int i = 0; i<depth*2; i++){
        dots[i]='.';
    }
    dots[2*depth]='\0';

    printf("%s%s - %s - %s\n",dots,node->name, node->type, node->param);
    if (node->inside)   printSymbolList(node->inside, depth+1);
    if (node->next)     printSymbolList(node->next, depth);

}
