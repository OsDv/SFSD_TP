#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

void printMenu() {
    printf("Select an option:\n");
    printf("1. Create TOF file\n");
    printf("2. Create TOVS file\n");
    printf("3. Delete selected records TOF\n");
    printf("4. Delete selected records TOVS\n");
    printf("5. Create TOF indexes (primary/BirthDate)\n");
    printf("6. search interval by BirthDate\n");
    printf("7. Information about the files (TOVS)\n");
    printf("8. Get Student Infos\n");
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
    fclose(TOF_log);
    fclose(csv);
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
    int size;
    TOVS_getElement(&tovs,buffer,&size,i,j);
    TOVS_printStudentInfos(buffer,size);
    TOVS_close(&tovs);
}

void checkStatus(){
    TOF_FILE tof;
    TOVS_FILE tovs;
    FILE *tofLog,*tovsLog;
    TOF_open(TOF_FILE_NAME,&tof,'r');
    if (TOF_getFile(&tof)!=NULL) {
        printf("TOF  file: %s\n",TOF_FILE_NAME);
        TOF_close(&tof);
    }
    else printf("TOF file: NOT-Found\n");
    TOVS_open(TOVS_FILE_NAME,&tovs,'r');
    if (TOVS_getFile(&tovs)!=NULL) {
        printf("TOVS file: %s\n",TOVS_FILE_NAME);
        TOVS_close(&tovs);
    }
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
    fclose(deleteFiel);
    fclose(log);
    TOVS_close(&tovs);
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
    printf("Secondary Index created succusfully \n");
    // TOF_printSIonBirthDate(&BirthDateIndex);
    TOF_close(&tof);
}
void creatTOF_primaryIndex(){
    TOF_FILE tof;
    FILE *file=fopen("TOF.index","wb");
    TOF_open(TOF_FILE_NAME,&tof,'r');
    if (tof.file==NULL){
        printf("cant open file tof\n");
        return;
    }
    TOF_creatPrimaryIndex(&tof,file,&primaryIndexSize,TOF_primaryIndex);
    printf("Primary Index created succusfully \n");    
    fclose(file);
    TOF_close(&tof);
}
void TOF_BirthDateQueryMenu(){
    if (primaryIndexSize==0 || BirthDateIndex.size==0){
        printf("Create Index first \n");
        return;
    }
    TOF_FILE tof;
    TOF_open(TOF_FILE_NAME,&tof,'r');
    char date1[DATE_SIZE],date2[DATE_SIZE];
    printf("Enter the first date(dd/mm/yyyy) :");
    // scanf("%.*s",DATE_SIZE,date1);
    char c;
    int i=0;
    while ((c=fgetc(stdin))!=EOF && c!='\n'); 
    while ((c=fgetc(stdin))!=EOF && c!='\n')
    {
        date1[i++]=c;
    }
    
    printf("\nEnter the scond date(dd/mm/yyyy) :");
    i=0;
    while ((c=fgetc(stdin))!=EOF && c!='\n')
    {
        date2[i++]=c;
    }
    // scanf("%.*s",DATE_SIZE,date2);
    TOF_BirthDateIntervalQuery(&tof,TOF_primaryIndex,&BirthDateIndex,date1,date2);
    TOF_close(&tof);

}
void TOFdeleteSelected()
{

    TOF_FILE  tof ;
    FILE * logdel=fopen(TOF_DELETE_LOG_FILE,"w");
    TOF_open(TOF_FILE_NAME,&tof,'a');               
    FILE* DelList= fopen(DELETE_FILE_NAME,"r");
    TOF_deletefromfile(&tof,DelList,logdel);
    fclose(DelList);
    fclose(logdel);
    TOF_close(&tof);

}

void PrintFilesInfos(){
    TOVS_FILE tovs;
    TOF_FILE tof;
    TOF_Header tofHeader;
    TOVS_Header tovsHeader;
    TOF_open(TOF_FILE_NAME,&tof,'r');
    TOVS_open(TOVS_FILE_NAME,&tovs,'r');
    TOVS_getHeader(&tovs,&tovsHeader);
    TOF_getHeader(&tof,&tofHeader);
    printf("TOF INFOS:\n");
    printf("Number of blocks: %d\n",tofHeader.NB);
    printf("Number of records: %d\n",tofHeader.NR);
    printf("Number of deleted records: %d\n",tofHeader.ND);
    printf("Loading factor: %.2f\n",(float)((tofHeader.NR-tofHeader.ND)/tofHeader.NB));
    printf("TOVS INFOS:\n");
    printf("Number of blocks: %d\n",tovsHeader.NB);
    printf("Number of Characters in last Block: %d",tovsHeader.NC);
    TOF_close(&tof);
    TOVS_close(&tovs);
}