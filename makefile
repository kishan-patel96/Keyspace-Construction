Wall: hashtable.c indexer.c main.c
	gcc -Wall -Werror -g hashtable.c indexer.c main.c -o invertedIndex
	./invertedIndex test.txt testfolder
	
