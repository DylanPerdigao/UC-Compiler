/**
 * @author Dylan Perdigão     -   2018233092
 * @author Bruno Faria           -   2018295474
 */
#include "ASTree.h"

//Functions
Node * newNode(char * message, int line, int col){
    Node * auxNode = (Node *) malloc(sizeof(Node));
    auxNode->message = message;
    auxNode->extraMessage = NULL;
    auxNode->child = NULL;
    auxNode->brother = NULL;
    auxNode->line=line;
    auxNode->col=col;

    return auxNode;
}

void addBrother(Node* node1, Node* node2){
    Node* temp;
    temp= node1;
    while(temp->brother!= NULL){
        temp=temp->brother;
    }
    temp->brother=node2;
}

void printASTree(Node *node, int depth){
    char dots[depth*2];
    for (int i = 0; i<depth*2; i++){
        dots[i]='.';
    }
    dots[2*depth]='\0';
    if (node->extraMessage){
        printf("%s%s - %s\n",dots,node->message,node->extraMessage);
    }else printf("%s%s\n",dots,node->message);

    if (node->child)    printASTree(node->child, depth+1);
    if (node->brother)  printASTree(node->brother, depth);

    //if printTree and hasError==False limpa a memória depois de printar!!
    free(node);
}

void freeASTree(Node *node){
    if (node->child)    freeASTree(node->child);
    if (node->brother)  freeASTree(node->brother);

    //if printTree and hasError==False limpa a memória depois de printar!!
    free(node);
}
