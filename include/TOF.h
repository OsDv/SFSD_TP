#ifndef _TOF_HEADER_
#define _TOF_HEADER_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
/*
 * declaration of types and constants
 */
// structure contain student personal informations

#define MAX_NAME_SIZE  20
#define MAX_CITY_NAME  20
#define DATE_SIZE 8
#define StudentSize  sizeof(Student)
#define MAX_RECORDS  100
#define TOF_blockSize  sizeof(TOF_Block)
#define TOF_HEADER_SIZE  sizeof(TOF_Header )


typedef struct {
	int id;
	char firstName[MAX_NAME_SIZE];
	char lastName[MAX_NAME_SIZE];
	char birthDate[DATE_SIZE];
	char birthCity[MAX_CITY_NAME];
} Student;


typedef struct {
	Student data[MAX_RECORDS];
	bool del[MAX_RECORDS];
	int NR;
	int ND;
} TOF_Block,TOF_Buffer;

typedef struct {
	int NB;// number of blocks in the file
	int NR;//number of records in the file
	int ND;// number of deleted records in the file
} TOF_Header;

#define MAX_NAME_SIZE  20
#define MAX_CITY_NAME  20
#define DATE_SIZE 8
#define StudentSize  sizeof(Student)
#define MAX_RECORDS  100
#define TOF_blockSize  sizeof(TOF_Block)
#define TOF_HEADER_SIZE  sizeof(TOF_Header )


#endif
