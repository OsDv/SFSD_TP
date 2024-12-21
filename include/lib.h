#ifndef _LIB_H_
#define  _LIB_H_

#define MAX_LINE_SIZE 250

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define CONFIG_FILE_NAME "./config.config"
typedef struct ID_L
{
    int id;
    struct ID_L *next;
}ID_LIST;

extern int NumberOfLinesCSV1;
extern int NumberOfLinesCSV2;
extern int NumberOfLinesDelete;
void ConfigProgram();
void showProgressBar(int current, int total);
void readINT(int *i);
void IDlist_insertOrd(ID_LIST **head , int id);
#endif