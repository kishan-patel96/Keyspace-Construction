#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct occurrence {

	char* key;	//word
	char* filename;
	unsigned int frequency;	//#times
	unsigned int index;	//re numbers 
	struct occurrence* next;
	struct occurrence* prev;

}occurrence;

struct hashtable {

	struct occurrence** table;
	int size;
	unsigned int numkeys;
	double balancefactor;

}hashtable;

struct hashtable* initialize(struct hashtable* hashtable, unsigned int size);
void cleartable(struct hashtable* hashtable);
unsigned long hashingfunc(char* key, struct hashtable* hashtable);
struct hashtable* insert(char* key, char* filename, struct hashtable* hashtable);
struct occurrence* getlist(char* key, struct hashtable* hashtable);
struct occurrence* getnode(char* key, char* filename, struct hashtable* hashtable);
struct occurrence* getmid(struct occurrence* start, unsigned int idx);
unsigned int getsize(struct occurrence* start);
void binarysort(char* key, struct hashtable* hashtable);
void fixidx(struct occurrence* start);
unsigned int alphanumcomp(char* filepath1, char* filepath2);
struct hashtable* rehash(struct hashtable* oldtable);
struct hashtable* merge(struct hashtable* filetable, struct hashtable* mastertable);
void freetable(struct hashtable* hashtable);

#endif
