/*
 * functions.h
 *
 *  Created on: May 10, 2017
 *      Author: Eran Peled
 *      
 */

#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "board.h"


char *trimEndWhitespaces(char *str)
{
	  char *end;
	// Trim trailing space
	  end = str + strlen(str) - 1;
	  while(end > str && isspace((unsigned char)*end)) end--;

	  // Write new null terminator
	  *(end+1) = 0;
	  return str;
}


/** listdate functions ****************************************************************************************/
date_t convertToDate(const char buff[]) {
	date_t date;
	char day[3] = "", month[3] = "", year[5] = "";
	strncpy(day, buff, 2);		date.day = atoi(day);
	strncpy(month, buff+3, 2);	date.month = atoi(month);
	strncpy(year, buff+6, 4);	date.year = atoi(year);
	if(date.day < 0 || date.day > 31 || date.month < 0 || date.month > 12 || date.year < 0 || date.year > 9999)
		{perror("invalid date"); exit(1);}
	return date;
}

int isAfter(const date_t start, const date_t date) { // return: 1 date is equal or after start, 0 date is earlier then start
	return start.year > date.year? 0 :
			start.month > date.month? 0 :
					start.day > date.day? 0: 1;
}

void printDate(const date_t date) {
	// [dd/mm/yyyy]
	if(date.day < 10) printf("0");
	printf("%d/" ,date.day);
	if(date.month < 10) printf("0");
	printf("%d/%d", date.month, date.year);
}


/** listcat functions ****************************************************************************************/
int isValidCategory(const char category[]) {
	char categories[11][B_CAT_LEN] = {"‪Appliances", "Art Crafts", "Cameras", "Computers",
			"Electronics", "Home Audio Video", "Jewelry", "Motorcycles", "Music", "Televisions","Tools"};
	for(int i=0; i<11; i++)
		if(!strcmp(category,categories[i])) return 1;
	return 0; // end of for, no match
}
int exist(int argc, char *argv[], const char *category) {
	for(int i=1; i<argc; i++) // skip first parameter
		if(!strcmp(category,argv[i])) return 1;
	return 0;
}


/** board functions ****************************************************************************************/
board_ent_t convertToEntry(char buff[]) {
	board_ent_t ent;

	//parse DATE
	char date[B_DATE_LEN];
	strncpy(date, buff , B_DATE_LEN);
	ent.date = convertToDate(date);

	//parse Category
	char category[B_CAT_LEN];
	char *parsed_category;

	for(int j=0, i=B_DATE_LEN; i<B_DATE_LEN+B_CAT_LEN; j++, i++){
			category[j] = buff[i];
	}

			category[B_CAT_LEN] = '\0';

	parsed_category = trimEndWhitespaces(category);

	//parse Text
	strcpy(ent.category, category);
	strcpy(ent.text, buff+B_DATE_LEN+B_CAT_LEN);
	if(ent.text[strlen(ent.text)-1] =='\n') // in case the text is shorter then B_TEXT_LEN
		ent.text[strlen(ent.text)-1] = '\0'; // avoid \n\n

	return ent;
}

int getLineFromFile(int fd, char *buff) {
	int i=0, readBytes;
	char smallBuff = ' ';

	while(smallBuff != '\n' && strlen(buff) < B_BUFF_LEN) {
		if((readBytes = read(fd, &smallBuff, 1)) == -1) {perror("read fail"); exit(1);}
		if(!readBytes) return 0; //EOF
		buff[i++] = smallBuff;
	}
	buff[i] = '\0';

	return 1;
}


void printPartOfText(const char *text, int length) {
	int actualLength = strlen(text)<length? strlen(text):length;
	for(int i=0; i<actualLength; i++)
		putchar(text[i]);
	putchar('\n');
}


#endif
