#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

int NumberOfLinesCSV1;
int NumberOfLinesCSV2;
int NumberOfLinesDelete;
void showProgressBar(int current, int total) {
    int percentage = (current * 100) / total;
    int progress = (percentage / 2); // Assuming the bar width is 50

    printf("\r[%-50s] %d%%", "==================================================" + (50 - progress), percentage);
    fflush(stdout);
}
// secured input function to read an ineger from user
void readINT(int *i){
    char c;
    while(scanf("%d",i)!=1){
        while((c=fgetc(stdin))!=EOF && (c!='\n'));
        printf("Enter a valid number:");
    }
}

void ConfigProgram(){
    FILE *f=fopen(CONFIG_FILE_NAME,"r");
    char buffer[MAX_LINE_SIZE];
    fgets(buffer,MAX_LINE_SIZE,f);
    sscanf(buffer,"%d",&NumberOfLinesCSV1);
    fgets(buffer,MAX_LINE_SIZE,f);
    sscanf(buffer,"%d",&NumberOfLinesCSV2);
    fgets(buffer,MAX_LINE_SIZE,f);
    sscanf(buffer,"%d",&NumberOfLinesDelete);
}

void IDlist_insertOrd(ID_LIST **head , int id){
    ID_LIST **list=head;
    ID_LIST *new,*tmp;
    new=malloc(sizeof(ID_LIST));
    new->id=id;
    new->next=NULL;
    while((*list) && (*list)->id<id){
        list=&((*list)->next);
    }
    new->next=(*list);
    (*list)=new;
}