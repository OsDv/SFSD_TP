#ifndef _TOVS_LIBRARY_
#define _TOVS_LIBRARY_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <TOF.h>
#include <lib.h>
#define MAX_LINE_SIZE 250
#define TOVS_MAX_ELEMENT_SIZE 400
#define MAX_CHARS_TOVS 100
#define TOVS_RECORDS_SIZE_WIDTH 3
#define TOVS_RECORDS_id_WIDTH 5
#define TOVS_YEAR_WIDTH 1
#define PRINT_N_RW_WIDTH 5
#define PRINT_LINE_NUMBER_WIDTH 5
#define TOVS_SEPARATOR '$'
#define TOVS_FILE_NAME "./result/Students_InfosSkills.tovs\0"
#define TOVS_LOG_FILE "./result/TOVS_LOG.txt\0"
typedef struct {
    char data[MAX_CHARS_TOVS];
} TOVS_Block,TOVS_Buffer;
#define TOVS_BLOCK_SIZE sizeof(TOVS_Block)

typedef struct {
    int NB;
    int NC;
} TOVS_Header;
#define TOVS_HEADER_SIZE sizeof(TOVS_Header)

typedef struct {
    FILE *file;
    TOVS_Header header;
} TOVS_FILE;

enum LineStatus {VALID_LINE , LINE_MISSING_ID , LINE_MISSING_DESCRIPTION ,LINE_MISSING_YEAR,EMPTY_LINE}; 
#define N_LINE_STATUS 5 // number of possible cases for lineStatus
enum InsertStatus {INSERT_SUCCUSFUL , RECORD_EXISTS , INSERT_SUCCUSFUL_STRUDLE};
#define N_INSERT_STATUS 3 // number of possible cases for inserStatus

// global variables to count number of read/writes for each insertion/deletion
extern int NUMBER_OF_READS;
extern int NUMBER_OF_WRITES;
/* 
* records structure : size[3] | id[5] | skills[VARIABLE]
*/
/*
* ABSTRACT MACHINE 
*/
int TOVS_setHeader(TOVS_FILE *f , TOVS_Header *header);
int TOVS_getHeader(TOVS_FILE *f , TOVS_Header *header);
int TOVS_readBlock(TOVS_FILE * f , int n , TOVS_Block *buffer);
int TOVS_writeBlock(TOVS_FILE * f , int n , TOVS_Block *buffer);
int TOVS_close(TOVS_FILE *file);
int TOVS_open(const char *name , TOVS_FILE *file , char mode);
/*
* other functions
*/
int TOVS_getId(TOVS_Buffer buffer ,TOVS_Buffer buffer1, int j);
int TOVS_getSize(TOVS_Buffer buffer,TOVS_Buffer buffer1 , int j);
FILE *TOVS_getFile(TOVS_FILE *f);
void TOVS_setFile(TOVS_FILE *f , FILE *file);
int TOVS_search(TOVS_FILE *f ,int key , bool *found , int *i , int *j  );
int TOVS_writeString(TOVS_FILE *f , char *src , int size , int block , int index,bool *strudle);
enum InsertStatus TOVS_insert(TOVS_FILE *f , char *src , int size);
int TOVS_sizeToString(int n , char * dest);
enum LineStatus checkValidLine(char *line);
void TOVS_writeLogSummary(FILE *f ,TOVS_Header header , int *inserSummary,int *linesSummary);
void TOVS_writeLineToLog(FILE *f , int lineNumber , enum InsertStatus insertS , enum LineStatus lineS);
int TOVS_createFile(TOVS_FILE *dest ,TOF_FILE *tof, FILE *src , FILE *logFile);
int TOVS_shiftRight(TOVS_FILE *f , int block , int offset , int step);
int TOVS_exractYear(char * srs  , char *dest);
int printFile(TOVS_FILE f);
int TOVS_lineToString(char *src,TOF_FILE *tof , char *dest ,enum LineStatus lineStat, int *size_);
void TOVS_getElement(TOVS_FILE *f , char *dest , int block , int offset);
bool TOVS_isDeleted(char *elm);
void TOVS_printStudentInfos(char *src);

#endif
