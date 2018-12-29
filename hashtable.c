#include "hashtable.h"
#include "indexer.h"

struct hashtable* initialize(struct hashtable* hashtable, unsigned int size) {

	hashtable = (struct hashtable*)malloc(sizeof(struct hashtable));
	hashtable->table = (struct occurrence**)malloc(sizeof(struct occurrence*) * size);
	hashtable->size = size;
	cleartable(hashtable);
	hashtable->numkeys = 0;
	hashtable->balancefactor = 0.75;

	return hashtable;
}

void cleartable(struct hashtable* hashtable) {	//clears table

	int i;
	for (i = 0; i < hashtable->size; i++) {
		hashtable->table[i] = NULL;
	}
}

unsigned long hashingfunc(char* key, struct hashtable* hashtable) {

	unsigned long hash = 5381; //Arbitrary prime
	unsigned int c;
	char* k = (char*)malloc(strlen(key)+1);
	strcpy(k, key);

	while ((c = (*k)++)) { //Finds hashing value
		hash = hash * 33 + c;
	}

	unsigned int idx = hash % hashtable->size; //Finds index from hashing value

	while (hashtable->table[idx] != NULL && strcmp(hashtable->table[idx]->key, key) != 0) { //Linear probing in case of collisions
		idx = (idx + 1) % hashtable->size;
	}

	free(k);
	return idx;
}

struct hashtable* insert(char* key, char* filename, struct hashtable* hashtable) {

	unsigned int idx = hashingfunc(key, hashtable);

	if (hashtable->table[idx] == NULL) {
		struct occurrence* occ = (struct occurrence*)malloc(sizeof(struct occurrence));

		occ->key = malloc(strlen(key) + 1);
		strcpy(occ->key, key);
		occ->filename = malloc(strlen(filename) + 1);
		strcpy(occ->filename, filename);
		occ->frequency = 1;
		occ->index = 0;
		occ->next = NULL;
		occ->prev = NULL;

		hashtable->table[idx] = occ;
		hashtable->numkeys++;
	}
	else {
		(hashtable->table[idx]->frequency)++;
	}

	if ((double)hashtable->numkeys / (double)hashtable->size >= hashtable->balancefactor) {
		return rehash(hashtable);
	}

	return hashtable;
}

struct occurrence* getlist(char* key, struct hashtable* hashtable) {

	return hashtable->table[hashingfunc(key, hashtable)];
}

struct occurrence* getnode(char* key, char* filename, struct hashtable* hashtable) {

	struct occurrence*  c = getlist(key, hashtable);

	while (c != NULL) {
		if (strcmp(c->filename, filename) == 0) {
			return c;
		}
		else {
			c = c->next;
		}
	}

	return NULL;
}

struct occurrence* getmid(struct occurrence* start, unsigned int idx) {

	struct occurrence* c = start;

	if (c->index < idx) {
		while (c->index != idx) {
			c = c->next;
		}
		return c;
	}
	else {
		while (c->index != idx) {
			c = c->prev;
		}
		return c;
	}
}

unsigned int getsize(struct occurrence* start) {	//gets size

	struct occurrence* c = start;
	unsigned int s = 0;

	while (c != NULL) {
		s++;
		c = c->next;
	}

	return s;
}

void binarysort(char* key, struct hashtable* hashtable) {

	if (getsize(getlist(key, hashtable)) <= 1) {
		return;
	}

	struct occurrence* c = getlist(key, hashtable);
	struct occurrence* head = c;
	unsigned int target = head->frequency;
	c = c->next;
	short int ridx = getsize(c) - 1;
	short int lidx = 0;

	while (ridx >= lidx) {
		unsigned int midx = (lidx + ridx) / 2;
		c = getmid(c, midx);

		if (c->frequency == target) {
			unsigned int result = alphanumcomp(head->filename, c->filename);

			if (result == 1) {
				hashtable->table[hashingfunc(key, hashtable)] = head->next;
				head->next->prev = head->prev;
				head->prev = c->prev;
				if (head->prev != NULL) {
					head->prev->next = head;
				}
				head->next = c;
				c->prev = head;
				if (head->prev == NULL) {
					hashtable->table[hashingfunc(key, hashtable)] = head;
				}
				fixidx(head);
			}
			else if (result == 2) {
				hashtable->table[hashingfunc(key, hashtable)] = head->next;
				head->next->prev = head->prev;
				head->next = c->next;
				if (head->next != NULL) {
					head->next->prev = head;
				}
				head->prev = c;
				c->next = head;
				fixidx(head);
			}
			else {
				printf("Error!\n");
			}

			return;
		}
		else if (c->frequency > target) {
			lidx = midx + 1;
		}
		else {
			ridx = midx - 1;
		}
	}

	if (c->frequency > target) {
		hashtable->table[hashingfunc(key, hashtable)] = head->next;
		head->next->prev = head->prev;
		head->next = c->next;
		if (head->next != NULL) {
			head->next->prev = head;
		}
		head->prev = c;
		c->next = head;
		fixidx(head);
	}
	else {
		hashtable->table[hashingfunc(key, hashtable)] = head->next;
		head->next->prev = head->prev;
		head->prev = c->prev;
		if (head->prev != NULL) {
			head->prev->next = head;
		}
		head->next = c;
		c->prev = head;
		if (head->prev == NULL) {
			hashtable->table[hashingfunc(key, hashtable)] = head;
		}
		fixidx(head);
	}
}

void fixidx(struct occurrence* start) {	//fix ordering

	struct occurrence* c = start;

	if (c->prev == NULL) {
		c = c->next;

		while (c != NULL) {
			(c->index)++;
			c = c->next;
		}
	}
	else if (c->next == NULL) {
		c->index = c->prev->index + 1;
	}
	else {
		c->index = c->prev->index + 1;
		c = c->next;

		while (c != NULL) {
			(c->index)++;
			c = c->next;
		}
	}
}

unsigned int alphanumcomp(char* filepath1, char* filepath2) {	//sort

	if (strcmp(filepath1, filepath2) == 0) {
		return 0;
	}

	unsigned int len1 = strlen(filepath1);
	unsigned int len2 = strlen(filepath2);
	unsigned short int shorter;

	unsigned int len;
	if (len1 > len2) {
		len = len2;
		shorter = 2;
	}
	else {
		len = len1;
		shorter = 1;
	}

	int i;
	len1 = 0;
	len2 = 0;
	for (i = 0; i < len; i++) {

		while (len1 < strlen(filepath1) && !(isalpha(filepath1[len1]) || isdigit(filepath1[len1]) || filepath1[len1] == '.')) {
			len1++;
		}
		while (len2 < strlen(filepath2) && !(isalpha(filepath2[len2]) || isdigit(filepath2[len2]) || filepath2[len2] == '.')) {
			len2++;
		}

		if (len1 >= len || len2 >= len) {
			return shorter;
		}
		
		unsigned short int val1 = (int)filepath1[len1];
		unsigned short int val2 = (int)filepath2[len2];
	
		if(isdigit(filepath1[len1])){
			val1 *= 5;
		}
		else if(filepath1[len1] == '.'){
			val1 *= 10;
		}
		
		if(isdigit(filepath2[len2])){
			val2 *= 5;
		}
		else if(filepath2[len2] == '.'){
			val2 *= 10;
		}

		if(val1 > val2){
			return 2;
		}
		else if(val1 < val2){
			return 1;
		}

		len1++;
		len2++;
	}

	return shorter;
}

struct hashtable* rehash(struct hashtable* oldtable) {	//adds if table is full

	struct hashtable* newtable = (struct hashtable*)malloc(sizeof(struct hashtable));
	newtable->size = oldtable->size * 2;
	newtable->table = (struct occurrence**)malloc(sizeof(struct occurrence*) * newtable->size);
	cleartable(newtable);
	newtable->numkeys = oldtable->numkeys;
	newtable->balancefactor = oldtable->balancefactor;

	int i;
	for (i = 0; i < oldtable->size; i++) {
		if (oldtable->table[i] != NULL) {
			newtable->table[hashingfunc(oldtable->table[i]->key, newtable)] = oldtable->table[i];
		}
	}

	free(oldtable->table);
	free(oldtable);

	return newtable;
}

struct hashtable* merge(struct hashtable* filetable, struct hashtable* mastertable) {	//adds to mastertable then sorts

	int i;
	for (i = 0; i < filetable->size; i++) {

		if (filetable->table[i] != NULL) {
			unsigned int idx = hashingfunc(filetable->table[i]->key, mastertable);
			struct occurrence* c = mastertable->table[idx];
			unsigned short int boolean = 0;

			while (c != NULL) {
				if (strcmp(c->filename, filetable->table[i]->filename) == 0) {
					boolean = 1;
					c->frequency += filetable->table[i]->frequency;
					free(filetable->table[i]->key);
					free(filetable->table[i]->filename);
					free(filetable->table[i]);
					break;
				}
				c = c->next;
			}
			if (boolean) {
				continue;
			}
			filetable->table[i]->next = mastertable->table[idx];
			mastertable->table[idx] = filetable->table[i];

			if (mastertable->table[idx]->next != NULL) {
				mastertable->table[idx]->next->prev = filetable->table[i];
				binarysort(mastertable->table[idx]->key, mastertable);
			}
			(mastertable->numkeys)++;

			if ((double)mastertable->numkeys / (double)mastertable->size >= mastertable->balancefactor) {
				mastertable = rehash(mastertable);
			}
		}
	}

	free(filetable->table);
	free(filetable);

	return mastertable;
}

void freetable(struct hashtable* hashtable) {

	int i;
	for (i = 0; i < hashtable->size; i++) {

		struct occurrence* c = hashtable->table[i];

		while(c != NULL){
			free(c->key);
			free(c->filename);
			struct occurrence* prev = c;
			c = c->next;
			free(prev);
		}
	}

	free(hashtable->table);
	free(hashtable);
}
