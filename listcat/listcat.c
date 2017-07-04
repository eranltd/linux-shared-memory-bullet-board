/*
 *	listcat.c
 *
 *  Created on: May 10, 2017
 *      Author: Eran Peled
 *      
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

#include "board.h"
#include "functions.h"


int main(int argc, char *argv[]) {
	if(argc == 1) {perror("no category provided"); exit(1);}
	for(int i=1; i<argc; i++)
		if(!isValidCategory(argv[i])) {
			char msg[strlen(argv[i])+21];
			strcpy(msg, "category "); strcat(msg, argv[i]); strcat(msg, " is invalid");
			perror(msg); exit(1);}

	int pid, shmID;
	if((shmID = shmget(B_KEY, sizeof(board_t), 0644)) == -1) {perror("shared memory fail"); exit(1);}

	board_t *board = shmat(shmID, NULL, 0);
	board->flag |= B_CAT_FLAG;

	if((pid = fork()) == -1) {perror("fork fail"); exit(1);}
	if(pid == 0) { //son
		char useless[10];
		if(read(STDIN_FILENO, useless, 10) == -1) {perror("read fail (son)"); exit(1);}
		board->flag &= ~B_CAT_FLAG;
		exit(0); /// end of son
	}

	while(board->flag & B_CAT_FLAG) {
		printf("\033[2J"); // clear screen
		printf("Category:\tText:\n------------------------------------------------------------------------------\n");
		for(int i=0; i<B_ENT_CNT; i++) {
			if(exist(argc, argv, board->entries[i].category)) { // chosen category
				printf("%s\t", board->entries[i].category);
				printPartOfText(board->entries[i].text, 70);
			}
		}
		sleep(5);
	}

	if(shmdt(board) == -1) {perror("disconnect fail"); exit(1);} // disconnect from shared memory space

	printf("\n### END ###\n");
	return 0;
}
