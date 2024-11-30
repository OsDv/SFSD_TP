#ifndef _TOF_HEADER_
#define _TOF_HEADER_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
/*
 * declaration of types and constants
 */
// structure contain student personal informations
extern const int MAX_NAME_SIZE;
extern const int MAX_CITY_NAME;
extern const int DATE_SIZE;
typedef struct {
	int id;
	char firstName[MAX_NAME_SIZE];
	char lastName[MAX_NAME_SIZE];
	char birthDate[DATE_SIZE];
	char birthCity[MAX_CITY_NAME];
} Student;
extern const int StudentSize;
extern const int MAX_RECORDS;

typedef struct {
	Student data[MAX_RECORDS];
	bool del[MAX_RECORDS];
	int NB;
	int ND;
} TOF_Block,TOF_Buffer;
extern const int TOF_blockSize;

typedef struct {
	int NB;// number of blocks in the file
	int NR;//number of records in the file
	int ND;// number of deleted records in the file
} TOF_Header;
extern const int TOF_HEADER_SIZE;




#endif
