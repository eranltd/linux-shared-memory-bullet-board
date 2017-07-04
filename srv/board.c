/*
 * board.c
 *
 *  Created on: May 10, 2017
 *      Author: Eran Peled
 *      
 */


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "board.h"
#include "functions.h"

int main(int argc, char *argv[]) {
	int i=0, pid, fd, shmID; // process ID, file directory, shared memory ID
	char buff[B_BUFF_LEN+1];

	if((shmID = shmget(B_KEY, sizeof(board_t), 0777 | IPC_CREAT | IPC_EXCL)) == -1) {
		perror("shared memory fail");
		exit(1);
	}
	board_t *board = shmat(shmID, NULL, 0);
	board->flag |= B_BOARD_FLAG;

	if((fd = open("classified.txt",O_RDONLY)) == -1) {perror("open fail"); exit(1);}

	if((pid = fork()) == -1)
	{perror("fork fail"); exit(1);}
	if(pid == 0) { //son
		char useless[10];
		if(read(STDIN_FILENO, useless, 10) == -1) {perror("read fail (son)"); exit(1);}
		board->flag &= ~B_BOARD_FLAG; //??
		exit(0); /// end of son
	}

	while(board->flag & B_BOARD_FLAG) {
		if(!getLineFromFile(fd, buff)) { // return 0 in case of EOF
			if(lseek(fd, 0 , SEEK_SET) == -1) {perror("seek fail"); exit(1);} // start from beginning
			continue;
		}
		board->entries[i] = convertToEntry(buff);

//		[ii]	dd/mm/yyyy	<category>		part_of_text
		printf("%d\t" ,i);
		printDate(board->entries[i].date);
		//printf("%40s\n", ptr);
		//print fixed structure for the category
		printf("\t%-18s\t", board->entries[i].category);
		printPartOfText(board->entries[i].text, 70);

		sleep(1);

		i = (i+1 < B_ENT_CNT? i+1 : 0); // basically just i++ (if the end is reached, start over)
	} // end of endless while


	if(shmdt(board) == -1) {perror("disconnect fail"); exit(1);} // disconnect from shared memory space
	if(shmctl(shmID, IPC_RMID, NULL) == -1) {perror("remove fail"); exit(1);} // remove shared memory space


	if(close(fd) == -1) {perror("close fail"); exit(1);}

	printf("\n\n### END ###\n");
	return 0;
}

