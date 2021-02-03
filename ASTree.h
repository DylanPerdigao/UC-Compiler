/**
 * @author Dylan Perdigão     -   2018233092
 * @author Bruno Faria           -   2018295474
 */
#ifndef ASTree_h
#define ASTree_h

//INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//STRUCT
typedef struct node{
    char* message;
    char* extraMessage;
    struct node* brother;
    struct node* child;
    int line;
    int col;
}Node;

//Function Headers
Node* newNode(char* message, int line, int col);
void addBrother(Node* node1, Node* node2);
void printASTree(Node* node, int depth);
void freeASTree(Node* node);

#endif /*ASTree_h*/
