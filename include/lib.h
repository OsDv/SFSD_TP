#ifndef _LIB_H_
#define  _LIB_H_

#define MAX_LINE_SIZE 250

#include <stdio.h>
#include <stdlib.h>
#define CONFIG_FILE_NAME "./config.config"

extern int NumberOfLinesCSV1;
extern int NumberOfLinesCSV2;
extern int NumberOfLinesDelete;
void ConfigProgram();
void showProgressBar(int current, int total);
void readINT(int *i);
#endif