#ifndef _TOF_HEADER_
#define _TOF_HEADER_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <lib.h>
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
#define TOF_PRINT_LINE_NUMBER_WIDTH 3
#define TOF_PRINT_N_RW_WIDTH 3
#define NUM_FIELDS 5
#define MAX_FIELD_LENGTH 64


#define TOF_FILE_NAME "./result/Students_Infos.TOF\0"
#define TOF_LOG_FILE "./result/TOF_LOG.txt\0"
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
	TOF_EMPTY_LINE , TOF_MISSING_ID , TOF_MISSING_FIRST_NAME , TOF_MISSING_LAST_NAME , TOF_MISSING_BIRTH_DATE , TOF_MISSING_BIRTH_CITY , TOF_VALID_LINE
};
#define TOF_N_LINE_STATUS 7

enum TOF_INSERT_STATUS {TOF_INSERT_SUCCUSFUL , TOF_RECORD_EXISTS , TOF_NOT_INSERTED};
#define TOF_N_INSERT_STATUS 3
typedef struct {
    FILE *file;
    TOF_Header header;
} TOF_FILE;
int TOF_setHeader(TOF_FILE *f , TOF_Header *header);
int TOF_getHeader(TOF_FILE *f , TOF_Header *header);
int TOF_readBlock(TOF_FILE * f , int n , TOF_Buffer *buffer) ;
int TOF_writeBlock(TOF_FILE * f , int n , TOF_Buffer *buffer);
int TOF_search(TOF_FILE *f , int key , bool *found , int *i , int *j , Student *student);
void TOF_LineToRecord(char* line,Student* student,enum TOF_LINE_STATUS *LineStatus);
void parseLine(char *line, char fields[NUM_FIELDS][MAX_FIELD_LENGTH]);
void TOF_writeLineToLog(FILE *f , int lineNumber , enum TOF_LINE_STATUS lineS , enum TOF_INSERT_STATUS insertS);
enum TOF_INSERT_STATUS insertElement(TOF_FILE *f , Student e);
int TOF_createFile(TOF_FILE *dest , FILE *src , FILE *logFile);
int TOF_open(const char *name , TOF_FILE *file , char mode);
int TOF_close(TOF_FILE *file);

void TOF_printFile(TOF_FILE *f);
enum TOF_INSERT_STATUS TOF_inserWithLoadingFactor(TOF_FILE *f , Student e);
int TOF_recordFragmentedSpace(Student s);
void TOF_writeSummaryToLog(FILE *f,TOF_FILE *tof,int Totalr,int Totalw ,int fragment, int *lineStat , int *insertStat );
#endif
/*


parcourir les line 
transforme lines to rec"TOF_LineToRec" and indicate its status
if ValidLine insert the rec in file "TOF_Insertion"
 WriteLineToLog





*/