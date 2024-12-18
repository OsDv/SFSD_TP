#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
TOF_SI_BirthDate BirthDateIndex;
TOF_PI_ID tab[10000];
int primaryIndexSize;
void printMenu() {
    printf("Select an option:\n");
    printf("1. Create TOF file\n");
    printf("2. Create TOVS file\n");
    printf("3. Delete selected records TOF\n");
    printf("4. Delete selected records TOVS\n");
    printf("5. Information about the files\n");
    printf("6. Status\n");
    printf("7. Get Student Infos\n");
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


void printStudentInfosMenu(){
    int id;
    TOVS_FILE tovs;
    TOVS_open(TOVS_FILE_NAME,&tovs,'r');
    // printFile(tovs);
    int i,j;
    bool found;
    char buffer[TOVS_MAX_ELEMENT_SIZE];
    printf("Enter student id:");
    readINT(&id);
    TOVS_search(&tovs,id,&found,&i,&j);
    if (!found){
        printf("Id doesn't exists \n");
        return;
    }
    TOVS_getElement(&tovs,buffer,i,j);
    TOVS_printStudentInfos(buffer);
    TOVS_close(&tovs);
}

void checkStatus(){
    TOF_FILE tof;
    TOVS_FILE tovs;
    FILE *tofLog,*tovsLog;
    TOF_open(TOF_FILE_NAME,&tof,'r');
    if (TOF_getFile(&tof)!=NULL) printf("TOF  file: %s\n",TOF_FILE_NAME);
    else printf("TOF file: NOT-Found\n");
    TOVS_open(TOVS_FILE_NAME,&tovs,'r');
    if (TOVS_getFile(&tovs)!=NULL) printf("TOVS file: %s\n",TOVS_FILE_NAME);
    else printf("TOVS file: NOT-Found\n");

}
void TOVSDeleteFromFile(){
    FILE *deleteFiel=fopen(DELETE_FILE_NAME,"r");
    FILE *log=fopen(TOVS_LOG_DELETE_FILE,"w");
    TOVS_FILE tovs;
    TOVS_open(TOVS_FILE_NAME,&tovs,'a');
    if (deleteFiel==NULL || log==NULL || TOVS_getFile(&tovs)==NULL){
        printf("ERROR OPEN FILES!\ncheck files and try again...\n");
        return;
    }
    TOVS_deleteFromFile(&tovs,deleteFiel,log);
    printf("\nDelete End...\n");
}
void creatTOF_SIBirthDate(){
    TOF_FILE tof;
    TOF_open(TOF_FILE_NAME,&tof,'r');
    if (tof.file==NULL){
        printf("cant open file tof\n");
        return;
    }
    TOF_creatSIonBirthDate(&tof,&BirthDateIndex);
    printf("creatio  is DONE! \n");
}
void creatTOF_primaryIndex(){
    TOF_FILE tof;
    FILE *file=fopen("TOF.index","wb");
    TOF_open(TOF_FILE_NAME,&tof,'r');
    if (tof.file==NULL){
        printf("cant open file tof\n");
        return;
    }
    TOF_primaryIndex(&tof,file,tab,&primaryIndexSize);
    printf("creatio  is DONE! \n");    
}