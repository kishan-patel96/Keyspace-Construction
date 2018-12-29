#include "indexer.h"
#include <unistd.h>

int main(int argc, char** argv){

	if(argc != 3){
		printf("This program requires 3 inputs: invertedIndex <inverted-index file name> <directory or file name>\n");
		return 0;
	}
	if(access(argv[2], F_OK) == -1){
		printf("File or directory name entered does not exist!\n");
		return 0;
	}		

	if(access(argv[1], F_OK) != -1){
		char decision;
		printf("Would you like to overwrite the existing file: \"%s\"? y/n\n", argv[1]);
		scanf(" %c", &decision);
		while(!(decision == 'y' || decision == 'n')){
			decision = 0;
			fflush(stdin);
			printf("Invalid decision. Enter again: y/n\n");
			scanf(" %c", &decision);	
		}
		if(decision == 'n'){
			return 0;
		}
	}

	DIR * dir = NULL;	//dir pointer
	struct dirent* sd = NULL;
	char* path = malloc(strlen(argv[2])+1);
	strcpy(path, argv[2]);
	unsigned short counter = 0;

	readall(dir, sd, &path, counter);
	free(path);
	int i;
	FILE* f = fopen(argv[1], "w");
	if(mastertable != NULL){
		for (i = 0; i < mastertable->size; i++) {
			if (mastertable->table[i] != NULL) {
				struct occurrence* curr = mastertable->table[i];
				//printf("%s\n", curr->key);
				insertBST(initNode(curr->key));
			}
		}
	writeIntro(f);
	Node* temp = root;
	writeBody(f, temp);
	writeEnd(f);
	fclose(f);
	//printf("%d\n", mastertable->numkeys);
	freetable(mastertable);	//frees memory
	temp = root;
	freetree(temp);
	printf("Done!\n");
	}

	return 0;
	//valgrind- check mem leaks
}

