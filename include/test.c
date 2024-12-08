#include <TOVS.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int TOVS_sizeToString(int n , char * dest);
int main(){
    char line[150];
    int id,j;
    char size[3],str[150];
    int n;
    int k;
    FILE *f = fopen("student.txt","r") ;
    for (int i=0;i<5 ;i++){
        fgets(line ,150,f);
        n= strlen(line);
        n=n-1;
        line[n]=0;
        printf("element %d : %s , %d\n",i,line,n);
        sizeToString(n+TOVS_RECORDS_SIZE_WIDTH-1,size);
        for ( k=0;k<3;k++) str[k]=size[k];
        for( k=0;k<5;k++) str[k+3]=line[k];
        j=6;
        k=8;
        while(line[j]!=0){
            str[k++]=line[j++];
        }
        printf("line %d : %.*s \n",i,n+TOVS_RECORDS_SIZE_WIDTH-1,str);
    }
}

int TOVS_lineToString(char *src , char *dest , int *size_){
    int n=strlen(src);
    src[n-1]=0;
    char size[TOVS_RECORDS_SIZE_WIDTH];
    sizeToString(n-2+TOVS_RECORDS_SIZE_WIDTH,size);
    (*size_) = n-2+TOVS_RECORDS_SIZE_WIDTH;
    for(int i=0;i<TOVS_RECORDS_SIZE_WIDTH;i++) dest[i] = size[i];
    for(int i=0;i<TOVS_RECORDS_id_WIDTH;i++) dest[i + TOVS_RECORDS_SIZE_WIDTH]=src[i];
    for(int i=0;i<n-2-TOVS_RECORDS_id_WIDTH;i++) dest[i + TOVS_RECORDS_SIZE_WIDTH + TOVS_RECORDS_id_WIDTH -1]=src[i + TOVS_RECORDS_id_WIDTH+1];
}


bool checkValidLine(char *line){
    int index=0;
    // check if the all the first 5 characters are numerical values represent the id else the line consdired as invalid
    while(line[index]!=';' && index<TOVS_RECORDS_id_WIDTH){
        if ( (line[index]<'0') || (line[index]>'9')) return false;
        index++;
    }
    // check if the separator ';' present after the the id else the id considired as invalid 
    if (line[index++]!=';') return false;
    // check if the description string (skills) is not empty else the line considred invalide
    if (line[index]=='\n') return false;
    // the line passes all the test then it is valide 
    return true;
    

}
int TOVS_sizeToString(int n , char * dest){
    dest[2] = n%10 + '0';
    dest[1] = (n/10)%10 +'0';
    dest[0] = (n/100)%10 +'0';
}