#include "indexer.h"

void readall(DIR* dir, struct dirent* sd, char** path, unsigned short int counter) {	//pass in file/folder, chcks if der
	//dfs- picks a path until empty

	char* filepath = NULL;
	unsigned short int isopen = 0;

	if (sd == NULL && counter == 0) {
		mastertable = initialize(mastertable, 1000);
		filepath = (char*)malloc(strlen(*path) + 1);
		strcpy(filepath, *path);
	}
	else if (sd != NULL) {
		filepath = (char*)malloc(strlen(*path) + strlen(sd->d_name) + 2);
		strcpy(filepath, *path);
		strcat(filepath, "/");
		strcat(filepath, sd->d_name);
	}

	struct stat pathstat;
	stat(filepath, &pathstat);
	free(filepath);

	if (S_ISDIR(pathstat.st_mode)) {

		if (counter > 0) {
			*path = realloc(*path, strlen(*path) + strlen(sd->d_name) + 2);
			strcat(*path, "/");
			strcat(*path, sd->d_name);
		}

		char cpath[strlen(*path) + 1];
		strcpy(cpath, *path);

		dir = opendir(*path);
		isopen = 1;
		//printf("OPEN\n");
		if (dir == NULL) {

			if(isopen){
				closedir(dir);
				//printf("CLOSE\n");
				//printf("%s\n", *path);
				isopen = 0;
			}
			//printf("RETURN1\n");
			return;
		}

		sd = readdir(dir);

		if (sd != NULL && strcmp(sd->d_name, ".") == 0) {
			sd = readdir(dir);
			sd = readdir(dir);
		}
		else if (sd != NULL && strcmp(sd->d_name, "..") == 0) {
			sd = readdir(dir);
		}
		if (sd == NULL) {

			if(isopen){
				closedir(dir);
				//printf("CLOSE\n");
				//printf("%s\n", *path);

				isopen = 0;
			}
			//printf("RETURN2\n");

			return;
		}

		readall(dir, sd, path, ++counter);

		sd = readdir(dir);
		if (sd == NULL) {

			if(isopen){
				closedir(dir);
				//printf("CLOSE\n");
				//printf("%s\n", *path);

				isopen = 0;
			}
			//printf("RETURN3\n");

			return;
		}


		*path = realloc(*path, strlen(cpath) + 1);
		strcpy(*path, cpath);

		readall(dir, sd, path, counter);
	}

	if (S_ISREG(pathstat.st_mode)) {

		if (sd == NULL) {

			if(isopen){
				closedir(dir);
				//printf("CLOSE\n");
				//printf("%s\n", *path);

				isopen = 0;
			}
			//printf("RETURN4\n");
			readfile(*path);
			return;
		}

		char cpath[strlen(*path) + 1];
		strcpy(cpath, *path);

		filepath = (char*)malloc(strlen(*path) + strlen(sd->d_name) + 2);
		strcpy(filepath, *path);
		strcat(filepath, "/");
		strcat(filepath, sd->d_name);
		//printf("%s\n", filepath);
		
		int i;
		for(i = 0; i < strlen(sd->d_name); i++){
			if(!(isalpha(sd->d_name[i]) || isdigit(sd->d_name[i]) || sd->d_name[i] == '.')){
				printf("File: \"%s\" was not read because it contains invalid character '%c'\n", sd->d_name, sd->d_name[i]);
				free(filepath);
				return;
			}
		}

		FILE* f = fopen(filepath, "r");
		if (f == NULL) {
			perror("fopen");

			if(isopen){
				closedir(dir);
				//printf("CLOSE\n");
				//printf("%s\n", *path);

				isopen = 0;
			}
			//printf("RETURN5\n");

			return;
		}

		filetable = initialize(filetable, 100);
		char* c_line = NULL;
		size_t len = 0;
		ssize_t result;
		char* filename = NULL;;
		char* key = NULL;

		while ((result = getline(&c_line, &len, f)) != -1) {

			if(strlen(c_line) == 0){
				continue;
			}

			for (i = 0; i < strlen(c_line); i++) {

				if (!(isalpha(c_line[i]) || isdigit(c_line[i]))) {
					c_line[i] = ' ';
				}
			}

			len = 0;
			char line[strlen(c_line)+1];
			strcpy(line, c_line);
			free(c_line);
			char* tokens = strtok(line, " ");

			if(tokens == NULL){
				continue;
			}

			tokens = tolowercase(tokens);

			if(isalpha(tokens[0])){
				key = (char*)malloc(strlen(tokens) + 1);
				strcpy(key, tokens);
				filename = (char*)malloc(strlen(sd->d_name) + 1);
				strcpy(filename, sd->d_name);
				filename = tolowercase(filename);
				//printf("%s\n", key);
				filetable = insert(key, filename, filetable);

				free(key);
				free(filename);
			}

			while (tokens != NULL) {

				tokens = strtok(NULL, " ");

				if(tokens == NULL){
					continue;
				}

				tokens = tolowercase(tokens);

				if(isalpha(tokens[0])){
					key = (char*)malloc(strlen(tokens) + 1);
					strcpy(key, tokens);
					filename = (char*)malloc(strlen(sd->d_name) + 1);
					strcpy(filename, sd->d_name);
					filename = tolowercase(filename);
					//printf("%s\n", key);
					filetable = insert(key, filename, filetable);
					free(key);
					free(filename);
				}
			}
		}

		mastertable = merge(filetable, mastertable);
		free(filepath);
		free(c_line);
		fclose(f);
		sd = readdir(dir);

		if (sd == NULL) {

			if(isopen){
				closedir(dir);
				//printf("CLOSE\n");
				//printf("%s\n", *path);

				isopen = 0;
			}
			//printf("RETURN6\n");

			return;
		}

		readall(dir, sd, path, counter);
		sd = readdir(dir);

		if(sd == NULL){

			if(isopen){
				closedir(dir);
				//printf("CLOSE\n");
				//printf("%s\n", *path);

				isopen = 0;
			}
			//printf("RETURN7\n");

			return;
		}
		*path = realloc(*path, strlen(cpath) + 1);
		strcpy(*path, cpath);

		readall(dir, sd, path, counter);
	}

	if(isopen){
		closedir(dir);
		//printf("CLOSE\n");
		//printf("%s\n", *path);

		isopen = 0;
	}
	//printf("RETURN8\n");
	if(counter == 0){
		printf("File or directory name entered is invalid!\n");
	}
	return;
}

void readfile(char* filepath){	//reads file

		FILE* f = fopen(filepath, "r");

		if (f == NULL) {
			perror("fopen");
			return;
		}

		int i;
		for(i = 0; i < strlen(filepath); i++){
			if(!(isalpha(filepath[i]) || isdigit(filepath[i]) || filepath[i] == '.')){
				printf("File: \"%s\" was not read because it contains invalid character '%c'\n", filepath, filepath[i]);
				return;
			}
		}

		filetable = initialize(filetable, 100);
		char* c_line = NULL;
		size_t len = 0;
		ssize_t result;
		char* key = NULL;

		while ((result = getline(&c_line, &len, f)) != -1) {

			if(strlen(c_line) == 0){
				continue;
			}

			for (i = 0; i < strlen(c_line); i++) {

				if (!(isalpha(c_line[i]) || isdigit(c_line[i]))) {
					c_line[i] = ' ';
				}
			}

			len = 0;
			char line[strlen(c_line)+1];
			strcpy(line, c_line);
			free(c_line);
			char* tokens = strtok(line, " ");

			if(tokens == NULL){
				continue;
			}

			tokens = tolowercase(tokens);

			if(isalpha(tokens[0])){
				key = (char*)malloc(strlen(tokens) + 1);
				strcpy(key, tokens);
				//printf("%s\n", key);
				filetable = insert(key, tolowercase(filepath), filetable);
				free(key);
			}

			while (tokens != NULL) {

				tokens = strtok(NULL, " ");

				if(tokens == NULL){
					continue;
				}

				tokens = tolowercase(tokens);
				if(isalpha(tokens[0])){
					key = (char*)malloc(strlen(tokens) + 1);
					strcpy(key, tokens);
					//printf("%s\n", key);
					filetable = insert(key, tolowercase(filepath), filetable);
	
					free(key);
				}
			}
		}

		mastertable = merge(filetable, mastertable);
		free(c_line);
		fclose(f);

}

char* tolowercase(char* str) {

	int i;
	for (i = 0; i < strlen(str); i++) {
		str[i] = tolower(str[i]);
	}

	return str;
}

void insertBST(Node* n){

	if(root == NULL){
		root = n;
		BSTsize++;
		return;
	}
	Node* temp = root;
	unsigned short int result;
	while(1){

		result = alphanumcomp(temp->key, n->key);
		if(result == 1){
			if(temp->left != NULL){
				temp = temp->left;
			}
			else{
				temp->left = n;
				BSTsize++;
				return;
			}
		}
		else if(result == 2){
			if(temp->right != NULL){
				temp = temp->right;
			}
			else{
				temp->right = n;
				BSTsize++;
				return;
			}
		}
		else{
			printf("Error2!\n");
			return;
		}
	}
}

Node* initNode(char* key){
	Node* n = malloc(sizeof(Node));
	n->key = key;
	n->left = NULL;
	n->right = NULL;
	return n;
}

void writeIntro(FILE* f){	//prints intro
	fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(f, "<fileIndex>\n");
}

void writeBody(FILE* f, Node* root){	//prints info

	if(root == NULL){
		return;
	}

	writeBody(f, root->right);
	fprintf(f, "\t<word text=\"%s\">\n", root->key);

	struct occurrence* c = mastertable->table[hashingfunc(root->key, mastertable)];

	while(c != NULL){
		fprintf(f, "\t\t<filename=\"%s\">%d</file>\n", c->filename, c->frequency);
		c = c->next;
	}

	fprintf(f, "\t</word>\n");

	writeBody(f, root->left);
}

void writeEnd(FILE* f){	//writes fileIndex
	fprintf(f, "<fileIndex>");
}

void freetree(Node* root){	//frees
	if(root == NULL){
		return;
	}
	freetree(root->right);
	freetree(root->left);
	free(root);
}
