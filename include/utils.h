#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_
#include <stdio.h>
#include <stdlib.h>
#include <TOVS.h>
#include <TOF.h>
#include <lib.h>
#define CLEAR_SCREEN "\033[2J"
#define CSV2_NAME "./resources/students_data_2a.csv"
#define CSV1_NAME "./resources/students_data_1a.csv"

void printMenu();
bool CreatTOFMenu();
bool CreatTOVSMenu();

#endif