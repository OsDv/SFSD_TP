#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

void printMenu() {
    printf("Select an option:\n");
    printf("1. Create TOF file\n");
    printf("2. Create TOVS file\n");
    printf("3. Delete selected records\n");
    printf("4. Information about the files\n");
    printf("5. Status\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}
bool CreatTOFMenu(){
    FILE *TOF_log , *csv;
    TOF_FILE TOF_file;
    printf("Reading from %s...\n",CSV1_NAME);
    csv = fopen(CSV1_NAME,"r");

    if (csv == NULL){
        printf("ERROR OPEN FILES!\ncheck files and try again...\n");  
        return false; 
    }
    printf("Creating %s and %s\n", TOF_FILE_NAME , TOF_LOG_FILE);
    TOF_log = fopen(TOF_LOG_FILE,"w");
    TOF_open(TOF_FILE_NAME , &TOF_file , 'n');
    bool status = (!TOF_createFile(&TOF_file , csv , TOF_log));
    TOF_close(&TOF_file);
    if (status)printf("Done files created!\n");
    else printf("error creating files try again\n");
}
bool CreatTOVSMenu(){
    FILE *TOVS_log , *csv;
    TOF_FILE TOF_file;
    TOVS_FILE TOVS_file;

    
    printf("Reading from %s and %s\n",CSV2_NAME , TOF_FILE_NAME);
    csv = fopen(CSV2_NAME,"r");
    TOF_open(TOF_FILE_NAME , &TOF_file , 'n');

    if (TOVS_log == NULL || csv == NULL) {
        printf("ERROR OPEN FILES!\ncheck files and try again...\n");
        return false;
    }
    
    printf("Creating %s and %s\n", TOVS_FILE_NAME , TOVS_LOG_FILE);
    TOVS_log = fopen(TOVS_LOG_FILE,"w");
    TOVS_open(TOVS_FILE_NAME , &TOVS_file , 'n');
    if (TOVS_log == NULL || TOVS_file.file == NULL){
        printf("ERROR CREATING FILES!\ncheck files and try again...\n");
        return false;
    }
    fclose(csv);
    

    



}