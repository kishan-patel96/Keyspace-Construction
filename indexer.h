#ifndef INDEXER_H_
#define INDEXER_H_

#include "hashtable.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct Node{
	char* key;
	struct Node* left;
	struct Node* right;
}Node;

struct hashtable* mastertable;
struct hashtable* filetable;
Node* root;
unsigned int BSTsize;

void readall(DIR* dir, struct dirent* sd, char** path, unsigned short int counter);
void readfile(char* filepath);
char* tolowercase(char* str);
void insertBST(Node* n);
Node* initNode(char* key);
void writeIntro(FILE* f);	
void writeBody(FILE* f, Node* root);
void writeEnd(FILE* f);
void freetree(Node* root);

#endif
