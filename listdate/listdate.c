/*
 * listdate.c
 *
 *  Created on: May 10, 2017
 *      Author: Eran Peled
 *      
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

#include "board.h"
#include "functions.h"


int main(int argc, char *argv[]){
	if(argc == 1) {perror("no date provided"); exit(1);}
	if(argc > 3) {perror("too many dates"); exit(1);}

	date_t start = convertToDate(argv[1]), end = {0,0,0};
	if(argc == 3) end = convertToDate(argv[2]);

	int pid, shmID;
	if((shmID = shmget(B_KEY, sizeof(board_t), 0644)) == -1) {perror("shared memory fail"); exit(1);}

	board_t *board = shmat(shmID, NULL, 0);
	board->flag |= B_DATE_FLAG;

	if((pid = fork()) == -1) {perror("fork fail"); exit(1);}
	if(pid == 0) { //son
		char useless[10];
		if(read(STDIN_FILENO, &useless, 10) == -1) {perror("read fail (son)"); exit(1);}
		board->flag &= ~B_DATE_FLAG;
		exit(0); /// end of son
	}

	while(board->flag & B_DATE_FLAG) {
		printf("\033[2J"); // clear screen
		printf("date:\t\tText:\n------------------------------------------------------------------------------\n");
		for(int i=0; i<B_ENT_CNT; i++) {
			if(!end.day) {
				if(isAfter(start, board->entries[i].date)) {
					printDate(board->entries[i].date); putchar('\t');
					printPartOfText(board->entries[i].text, 70);
				}
			}
			else {
				if(isAfter(start, board->entries[i].date) && !isAfter(end, board->entries[i].date)) {
					printDate(board->entries[i].date); putchar('\t');
					printPartOfText(board->entries[i].text, 70);
				}
			}
		}
		sleep(5);
	}

	if(shmdt(board) == -1) {perror("disconnect fail"); exit(1);} // disconnect from shared memory space

	printf("\n### END ###\n");
	return 0;
}
