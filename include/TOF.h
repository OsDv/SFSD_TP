#ifndef _TOF_HEADER_
#define _TOF_HEADER_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
/*
 * declaration of types and constants
 */
// structure contain student personal informations

#define MAX_NAME_SIZE  20
#define MAX_CITY_NAME  20
#define DATE_SIZE 10
#define StudentSize  sizeof(Student)
#define MAX_RECORDS  100
#define LOADING_FACTOR 0.60
#define MAX_LINE_SIZE 250
#define TOF_PRINT_LINE_NUMBER_WIDTH 3
#define TOF_PRINT_N_RW_WIDTH 3
#define NUM_FIELDS 5
#define MAX_FIELD_LENGTH 64


extern int TOF_NUMBER_OF_READS;
extern int TOF_NUMBER_OF_WRITES;

typedef struct {
	int id;
	char firstName[MAX_NAME_SIZE];
	char lastName[MAX_NAME_SIZE];
	char birthDate[DATE_SIZE];
	char birthCity[MAX_CITY_NAME];
} Student;
#define StudentSize  sizeof(Student)


typedef struct {
	Student data[MAX_RECORDS];
	bool del[MAX_RECORDS];
	int NR;
	int ND;
} TOF_Block,TOF_Buffer;
#define TOF_blockSize  sizeof(TOF_Block)

typedef struct {
	int NB;// number of blocks in the file
	int NR;//number of records in the file
	int ND;// number of deleted records in the file
} TOF_Header;
#define TOF_HEADER_SIZE sizeof(TOF_Header)
enum TOF_LINE_STATUS {
	EMPTY_LINE , MISSING_ID , MISSING_FIRST_NAME , MISSING_LAST_NAME , MISSING_BIRTH_DATE , MISSING_BIRTH_CITY , VALID_LINE
};

typedef struct {
	FILE *file;
	TOF_Header header;
} TOF_FILE;
enum INSERT_STATUS {INSERT_SUCCUSFUL , RECORD_EXISTS , NOT_INSERTED};
typedef struct {
    FILE *file;
    TOF_Header header;
} TOF_FILE;
int TOF_setHeader(TOF_FILE *f , TOF_Header *header);

int TOF_getHeader(TOF_FILE *f , TOF_Header *header);


int TOF_readBlock(TOF_FILE * f , int n , TOF_Buffer *buffer) ;

int TOF_writeBlock(TOF_FILE * f , int n , TOF_Buffer *buffer);


int TOF_search(TOF_FILE *f , int key , bool *found , int *i , int *j , Student *student);

void TOF_LineToRecord(char* line,Student* student,enum INSERT_STATUS *LineStatus);



#endif
/*


parcourir les line 
transforme lines to rec"TOF_LineToRec" and indicate its status
if ValidLine insert the rec in file "TOF_Insertion"
 WriteLineToLog





*/