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
#define DELETE_FILE_NAME "./resources/delete_students.csv"
#define TOF_DELETE_LOG_FILE "./result/TOF_LOG_DEL.txt"
void printMenu();
bool CreatTOFMenu();
bool CreatTOVSMenu();
void printStudentInfosMenu();
void checkStatus();
void TOVSDeleteFromFile();
void creatTOF_SIBirthDate();
void creatTOF_primaryIndex();
void creatTOF_SIBirthDate();
void TOF_BirthDateQueryMenu();
void TOFdeleteSelected();
void PrintFilesInfos();
#endif