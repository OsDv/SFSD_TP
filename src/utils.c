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
    int status = (TOF_createFile(&TOF_file , csv , TOF_log));
    TOF_close(&TOF_file);
    if (status==0)printf("\nDone files created!\n");
    else printf("\nerror creating files try again\n");
}
bool CreatTOVSMenu(){
    FILE *TOVS_log , *csv;
    TOF_FILE TOF_file;
    TOVS_FILE TOVS_file;

    
    printf("Reading from %s and %s\n",CSV2_NAME , TOF_FILE_NAME);
    csv = fopen(CSV2_NAME,"r");
    TOF_open(TOF_FILE_NAME , &TOF_file , 'r');

    if (TOF_file.file == NULL || csv == NULL) {
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
    TOVS_createFile(&TOVS_file,&TOF_file,csv,TOVS_log);
    fclose(TOVS_log);
    TOVS_close(&TOVS_file);
    TOF_close(&TOF_file);
    fclose(csv);

}

void ShowStudentINfos(){
    
}

void TOVS_printStudentInfos(){
    TOVS_FILE tovs;

}
