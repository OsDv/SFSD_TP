#ifndef _TOVS_LIBRARY_
#define _TOVS_LIBRARY_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_CHARS_TOVS 1000
#define TOVS_RECORDS_SIZE_WIDTH 3
#define TOVS_RECORDS_id_WIDTH 5
typedef struct {
    char data[MAX_CHARS_TOVS];
} TOVS_Block,TOVS_Buffer;
#define TOVS_BLOCK_SIZE sizeof(TOVS_Block)

typedef struct {
    int NB;
    int NC;
} TOVS_Header;
#define TOVS_HEADER_SIZE sizeof(TOVS_Header)


/* 
* records structure : size[3] | id[5] | skills[VARIABLE]
*/
/*
* ABSTRACT MACHINE 
*/
int TOVS_setHeader(FILE *f , TOVS_Header *header);
int TOVS_getHeader(FILE *f , TOVS_Header *header);
int TOVS_readBlock(FILE * f , int n , TOVS_Block *buffer);
int TOVS_writeBlock(FILE * f , int n , TOVS_Block *buffer);
/*
* other functions
*/
int TOVS_getId(TOVS_Buffer buffer , int j);
int TOVS_getSize(TOVS_Buffer buffer , int j);
int TOVS_search(FILE *f ,int key , bool *found , int *i , int *j  );
int TOVS_writeString(FILE *f , char *src , int size , int block , int index);
int TOVS_insert(FILE *f , char *src , int size);
#endif
