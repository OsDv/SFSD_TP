#include "C:\Users\adlen\OneDrive\Documents\GitHub\SFSD_TP\include\TOF.h"

#define MAX_LINE_LENGTH 1024
#define NUM_FIELDS 5


int TOF_setHeader(FILE *f , TOF_Header *header){
	fseek(f , 0 , SEEK_SET );
	fwrite(header , TOF_HEADER_SIZE , 1,f);
}
int TOF_getHeader(FILE *f , TOF_Header *header){
	fseek(f , 0 , SEEK_SET);
	fread(header , TOF_HEADER_SIZE , 1,f);
}

int TOF_readBlock(FILE * f , int n , TOF_Buffer *buffer) {
	fseek(f , TOF_HEADER_SIZE+TOF_blockSize*(n-1) , SEEK_SET);
	fread(buffer ,TOF_blockSize , 1 , f); 
}

int TOF_writeBlock(FILE * f , int n , TOF_Buffer *buffer){
	fseek(f ,  TOF_HEADER_SIZE+TOF_blockSize*(n-1), SEEK_SET);
	fwrite(buffer ,TOF_blockSize,1, f);
}

int TOF_search(FILE *f , int key , bool *found , int *i , int *j , Student *student){
	TOF_Header header;
	TOF_Buffer buffer;
	TOF_getHeader(f,&header);
    int sup,inf;
	sup = header.NB;
	inf = 1 ;
	bool stop = false;
	(*found) = false;
	while(!stop){
		*i = (sup + inf )/2;
		TOF_readBlock(f,*i,&buffer);
		if (key >= buffer.data[0].id && key <= buffer.data[buffer.NR].id){ 
            //  the element should be found in the block number i
            inf = 0;
            sup = buffer.NR -1;
            while (!stop){
                *j = (sup + inf )/2;
                if (key == buffer.data[*j].id && !buffer.del[*j]){    // + deleted student
                    (*found) = true;
                    if (student) (*student ) = buffer.data[*j];  
                    stop = true;
                } else {
                    if (key , buffer.data[*j].id) sup = *j-1;
                    else inf = *j+1;
                }
                if (inf > sup) stop = false;
            }            

        } else if(key < buffer.data[0].id){
            // the element should be found in the inferior half of the file
                sup = i-1;
        } else {
            // the element should be found in the superior half of the file
            inf = i + 1;
        }
        // search ended and element not founded should be inserted in block i index j
        if (inf > sup) stop = true ;
	}
}



InsertionStatus insertElement(FILE *f , elm_t e){
    int i,j;
    bool found;
    TOF_Buffer buffer,nextBuffer;
    Student tmp;
    TOF_Header header;
    TOF_getHeader(f,&header);
    TOF_search(f,e.id,&found,&i,&j,NULL);
    if (found) return -1;   //Already exist
    TOF_readBlock(f,i,&buffer);
    bool stop = false;
    while (!stop){
        // element should be inserted in the bloc i and do only internal shifts
        if ((buffer.NR - buffer.ND+1)< MAX_RECORDS*LOADING_FACTOR){
            buffer.NR++;
            while (!stop){
                tmp = buffer.data[j];
                buffer.data[j] = e;
                if (buffer.del[j] || j>=buffer.NR) {
                    stop = true;
                    if (buffer.del[j]) {
                        buffer.ND--;
                        buffer.NR--;
                        header.ND--;
                        buffer.del[j]=false;
                    }
                }
                j++;
                e=tmp;
            }
         } else {
            
            int nextBlockIndex = i + 1;

            // If the next block does not exist create a new block
            if (nextBlockIndex >= header.NB) {
                //---allocate block----\\ 
                header.NB++; 
            } else {
                TOF_readBlock(f, nextBlockIndex, &nextBuffer);
            }

            nextBuffer.data[0] = buffer.data[MAX_RECORDS - 1];
            nextBuffer.NR++;
            buffer.NR--;
            TOF_writeBlock(f, i, &buffer);
            TOF_writeBlock(f, nextBlockIndex, &nextBuffer); 
            i = nextBlockIndex;
            j = 0;
            TOF_setHeader(f, &header);
        }
    }

   
    TOF_setHeader(f, &header);
    return 0; // Successful insertion
}

void TOF_LineToRecord(char* line,Student* student,int *LineStatus);//insetion status enum
{

    char *fields[5];
    int field_count = 0;
    char *token = strtok(line, ",");
    







}

int TOF_createFile(TOF_FILE *dest , FILE *src , FILE *logFile)
{
if ((dest==NULL)||(src==NULL)) return -1;
    TOF_Header header={0,0,0};
    TOF_setHeader(dest,&header);
    TOF_Buffer buffer;
    char line[MAX_LINE_SIZE];
    enum INSERT_STATUS insertStatus;
    int lineNumber=0;
    enum TOF_LINE_STATUS LineStatus;
    Student student;
    // Skip the first line
    fgets(line,MAX_LINE_SIZE, src);
    while (fgets(line, MAX_LINE_SIZE, src))
    {
    TOF_LineToRecord(line,&student,&LineStatus);
    if (LineStatus==VALID_LINE) {
       insertStatus=insertElement(dest,student);
        
    TOF_writeLineToLog(logFile ,lineNumber,LineStatus,insertStatus);

    }

}
}
