#include <TOVS.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

// global variables to count number of read/writes for each insertion/deletion
int NUMBER_OF_READS;
int NUMBER_OF_WRITES;
int TOVS_setHeader(TOVS_FILE *file, TOVS_Header *header){
	file->header = (*header);
}
int TOVS_getHeader(TOVS_FILE *file, TOVS_Header *header){
	(*header)=file->header;
}

int TOVS_open(const char *name , TOVS_FILE *file , char mode){
    switch (mode)
    {
    case 'n':
    file->file= fopen(name,"wb");
    file->header.NB=0;
    file->header.NC=0;
        break;
    case 'r':
    file->file= fopen(name,"rb");
	fseek(file->file , 0 , SEEK_SET );
	fread(&(file->header) , TOVS_HEADER_SIZE , 1,file->file);
        break;
    case 'a':
    file->file= fopen(name,"rb+");
	fseek(file->file , 0 , SEEK_SET );
	fread(&(file->header) , TOVS_HEADER_SIZE , 1,file->file);        
        break;
    
    default:
        break;
    }
}

int TOVS_close(TOVS_FILE *file){
	fseek(file->file , 0 , SEEK_SET );
	fwrite(&(file->header) , TOVS_HEADER_SIZE , 1,file->file);    
    fclose(file->file);
    file->file=NULL;
}

int TOVS_readBlock(TOVS_FILE *f, int n , TOVS_Block *buffer) {
	fseek(f->file , TOVS_HEADER_SIZE+TOVS_BLOCK_SIZE*(n-1) , SEEK_SET);
	fread(buffer ,TOVS_BLOCK_SIZE , 1 , f->file); 
    NUMBER_OF_READS++;
}

int TOVS_writeBlock(TOVS_FILE * f , int n , TOVS_Block *buffer){
	fseek(f->file ,  TOVS_HEADER_SIZE+TOVS_BLOCK_SIZE*(n-1), SEEK_SET);
	fwrite(buffer ,TOVS_BLOCK_SIZE,1, f->file);
    NUMBER_OF_WRITES++;
}

int TOVS_getId(TOVS_Buffer buffer , int j){
    char key[TOVS_RECORDS_id_WIDTH+1];
    key[TOVS_RECORDS_id_WIDTH]=0;
    strncpy(key,&buffer.data[j+TOVS_RECORDS_SIZE_WIDTH],TOVS_RECORDS_id_WIDTH);
    return atoi(key);
}

int TOVS_getSize(TOVS_Buffer buffer , int j){
    char size[TOVS_RECORDS_SIZE_WIDTH+1];
    size[TOVS_RECORDS_SIZE_WIDTH]=0;
    strncpy(size,&buffer.data[j],TOVS_RECORDS_SIZE_WIDTH);
    return atoi(size);
}

int TOVS_search(TOVS_FILE *f ,int key , bool *found , int *i , int *j  ){
    TOVS_Header header;
    TOVS_Buffer buffer;
    int id,size;
    TOVS_getHeader(f,&header);
    bool stop = false;
    (*found)= false;
    (*i)=1;
    (*j)=0;
    if (header.NB ==0) return 1; // the file is empty
    TOVS_readBlock(f,(*i),&buffer);
    while (!stop){
        if ((id=TOVS_getId(buffer,(*j)))==key){ // the element is found at position block i start on character j
            (*found)=true;
            stop=true;
        } else {
            if (id > key){
                stop = true; // element doesn't exists and it should be found in block i start on character j
            }else{
                (*j)=(*j) + TOVS_getSize(buffer , (*j));
                if ((*j)>=MAX_CHARS_TOVS) { 
                    while((*j)>=MAX_CHARS_TOVS){
                        (*j)=(*j)-MAX_CHARS_TOVS;
                        (*i)=(*i)+1;
                    }
                    if ((*i) <=header.NB) TOVS_readBlock(f,(*i),&buffer);
                }
                if (((*i) == header.NB && (*j)==header.NC) || (*i)>header.NB) stop = true; // reached end of file and the element not found 
            }
        }
    }
    return 0;
}


int TOVS_writeString(TOVS_FILE *f , char *src , int size , int block , int index ,bool *strudle){
    TOVS_Header header;
    TOVS_getHeader(f,&header);
    TOVS_Buffer buffer;
    if (block <= header.NB) TOVS_readBlock(f,block,&buffer);
    (*strudle)=false;
    for (int i =0 ; i<size ;i++){
        if (index==MAX_CHARS_TOVS){
            (*strudle)=true;
            index=0;
            TOVS_writeBlock(f,block++,&buffer);
            if (block <= header.NB) TOVS_readBlock(f,block,&buffer);
        }
        buffer.data[index++] = src[i];
    }
    TOVS_writeBlock(f,block++,&buffer);


}
enum InsertStatus TOVS_insert(TOVS_FILE *f , char *src , int size){
    char id[TOVS_RECORDS_id_WIDTH+1]; // buffer to read id from string
    id[TOVS_RECORDS_id_WIDTH]=0; 
    strncpy(id,&src[TOVS_RECORDS_SIZE_WIDTH],TOVS_RECORDS_id_WIDTH);// copy the id to the buffer
    int key = atoi(id);// make integer from the id
    int i,j; // indexes for block and deplacement
    bool found; 
    TOVS_Header header; // buffer for file header
    TOVS_getHeader(f,&header); 
    bool strudle;
    // CASE FILE EMPTY
    if (header.NB==0){
        TOVS_writeString(f,src,size,1,0,&strudle);
        header.NB=1;
        header.NC=size;
        TOVS_setHeader(f,&header);
                if (strudle) return INSERT_SUCCUSFUL_STRUDLE;
        else return INSERT_SUCCUSFUL;
    }
    TOVS_search(f,key,&found,&i,&j);
    if (found) return RECORD_EXISTS; // if element already inserted we dont insert the duplicat
    int blockR=header.NB,indexR=header.NC-1;
    int blockW=header.NB+((size+header.NC)/MAX_CHARS_TOVS) , indexW = ((indexR+size)%MAX_CHARS_TOVS)-1;
    // update header values
    header.NB=blockW;
    header.NC=indexW+1;
    TOVS_setHeader(f,&header);
    // two buffers to perform shift operation inter and intra blocks
    TOVS_Buffer readBuffer,writeBuffer;
    bool stop =((header.NB==i && header.NC ==j)|| i>header.NB);
    while(!stop){
        writeBuffer.data[indexW] = readBuffer.data[indexR];

        if (indexR==j && blockR==i) stop=true;
        indexR--;
        indexW--;
        if (indexR<0){
            indexR=MAX_CHARS_TOVS-1;
            blockR--;
            TOVS_readBlock(f,blockR,&readBuffer);
        }
        if (indexW<0){
            indexR=MAX_CHARS_TOVS-1;
            TOVS_writeBlock(f,blockW,&writeBuffer);
            blockW--;
        }
    }
        TOVS_writeBlock(f,blockW,&writeBuffer);        
        // function to write the element after doing necessary shifts
        TOVS_writeString(f,src,size,i,j,&strudle);
        if (strudle) return INSERT_SUCCUSFUL_STRUDLE;
        else return INSERT_SUCCUSFUL;

}
int TOVS_lineToString(char *src , char *dest , int *size_){
    int n=strlen(src);
    src[n-1]=0;
    char size[TOVS_RECORDS_SIZE_WIDTH];
    TOVS_sizeToString(n-2+TOVS_RECORDS_SIZE_WIDTH,size);
    (*size_) = n-2+TOVS_RECORDS_SIZE_WIDTH;
    for(int i=0;i<TOVS_RECORDS_SIZE_WIDTH;i++) dest[i] = size[i];
    for(int i=0;i<TOVS_RECORDS_id_WIDTH;i++) dest[i + TOVS_RECORDS_SIZE_WIDTH]=src[i];
    for(int i=0;i<n-2-TOVS_RECORDS_id_WIDTH;i++) dest[i + TOVS_RECORDS_SIZE_WIDTH + TOVS_RECORDS_id_WIDTH ]=src[i + TOVS_RECORDS_id_WIDTH+1];
}


enum LineStatus checkValidLine(char *line){
    int index=0;
    if (line==NULL || *line=='\n' || *line=='\0') return EMPTY_LINE;
    // check if the all the first 5 characters are numerical values represent the id else the line consdired as invalid
    while(line[index]!=';' && index<TOVS_RECORDS_id_WIDTH){
        if ( (line[index]<'0') || (line[index]>'9')) return LINE_MISSING_ID;
        index++;
    }
    // check if the separator ';' present after the the id else the id considired as invalid 
    if (line[index++]!=';') return LINE_MISSING_ID;
    // check if the description string (skills) is not empty else the line considred invalide
    if (line[index]=='\n') return LINE_MISSING_DESCRIPTION;
    // the line passes all the test then it is valide 
    return VALID_LINE;
    

}
int TOVS_sizeToString(int n , char * dest){
    dest[2] = n%10 + '0';
    dest[1] = (n/10)%10 +'0';
    dest[0] = (n/100)%10 +'0';
}

int TOVS_createFile(TOVS_FILE *dest , FILE *src , FILE *logFile){
    if ((dest==NULL)||(src==NULL)) return -1;
    TOVS_Header header={0,0};
    TOVS_setHeader(dest,&header);
    TOVS_Buffer buffer;
    char line[MAX_LINE_SIZE];

    /*
    *   reading element varaibles
    */
    int lineNumber=0;
    int size;
    char recordStr[MAX_LINE_SIZE+TOVS_RECORDS_SIZE_WIDTH];
    enum LineStatus lineStatus;
    enum InsertStatus insertStatus;
    fgets(line,MAX_LINE_SIZE,src);//skip the first line of the file contains fields titles
    int totalRead=0,totalWrite=0;
    int insertSummary[N_INSERT_STATUS]={0};
    int linesStatusSummary[N_INSERT_STATUS]={0};


    while(fgets(line,MAX_LINE_SIZE,src)!=NULL){
        lineNumber++;
        lineStatus= checkValidLine(line);
//      initialise number read/write with 0 for each line read from source file
        NUMBER_OF_READS=0;
        NUMBER_OF_WRITES=0;
        // insert Valid Line
        if (lineStatus==VALID_LINE){
            TOVS_lineToString(line,recordStr,&size);
            insertStatus = TOVS_insert(dest,recordStr,size);
        }
        TOVS_writeLineToLog(logFile ,lineNumber ,insertStatus,lineStatus);
        totalRead+=NUMBER_OF_READS;
        totalWrite+=NUMBER_OF_WRITES;
        insertSummary[insertStatus]++;
        linesStatusSummary[lineStatus]++;
    }
    TOVS_getHeader(dest,&header);
    TOVS_writeLogSummary(logFile,header,insertSummary,linesStatusSummary);

    
}

void TOVS_writeLineToLog(FILE *f , int lineNumber , enum InsertStatus insertS , enum LineStatus lineS){
    if (lineS==VALID_LINE){
        switch (insertS)
        {
        case INSERT_SUCCUSFUL:
            fprintf(f,"+%*d:INSERTED:NON-STRUDLE:%*dR %*dW\n",PRINT_LINE_NUMBER_WIDTH,lineNumber,PRINT_N_RW_WIDTH,NUMBER_OF_READS,PRINT_N_RW_WIDTH,NUMBER_OF_WRITES);
            break;
        case INSERT_SUCCUSFUL_STRUDLE:
            fprintf(f,"+%*d:INSERTED:STRUDLE:%*dR %*dW\n",PRINT_LINE_NUMBER_WIDTH,lineNumber,PRINT_N_RW_WIDTH,NUMBER_OF_READS,PRINT_N_RW_WIDTH,NUMBER_OF_WRITES);
            break;
        case RECORD_EXISTS:
            fprintf(f,"-%*d:NOT-INSERTED:DUPLICAT:%*dR %*dW\n",PRINT_LINE_NUMBER_WIDTH,lineNumber,PRINT_N_RW_WIDTH,NUMBER_OF_READS,PRINT_N_RW_WIDTH,NUMBER_OF_WRITES);
            break;
        default:
            break;
        }
    } else {
        switch (lineS)
        {
        case EMPTY_LINE:
            fprintf(f,"*%*d:NOT-INSERTED:EMPTY-LINE:%5dR %5dW\n",PRINT_LINE_NUMBER_WIDTH,lineNumber,NUMBER_OF_READS,NUMBER_OF_WRITES);
            break;
        
        case LINE_MISSING_DESCRIPTION   :
            fprintf(f,"*%*d:NOT-INSERTED:MISSING-DESCRIPTION:%5dR %5dW\n",PRINT_LINE_NUMBER_WIDTH,lineNumber,NUMBER_OF_READS,NUMBER_OF_WRITES);            
            break;
        
        case LINE_MISSING_ID:
            fprintf(f,"*%*d:NOT-INSERTED:MISSING-ID:%5dR %5dW\n",PRINT_LINE_NUMBER_WIDTH,lineNumber,NUMBER_OF_READS,NUMBER_OF_WRITES);
            break;
        
        default:
            break;
        }
    }
}

void TOVS_writeLogSummary(FILE *f ,TOVS_Header header , int *inserSummary,int *linesSummary){
    fputs("///////////\t\t\tTHE SUMMARY\t\t\t///////////\n",f);
    // created file informations
    fputs("\t1) FILE INFORMATIONS :\n",f);
    fprintf(f,"file name:\"%s\"\n",TOVS_FILE_NAME);
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);
    fprintf(f,"created on : %02d-%02d-%04d %02d:%02d:%02d\n",systemTime.wDay, systemTime.wMonth, systemTime.wYear, 
            systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
    fprintf(f,"file size:%d byte of characters + %d byte header size\n",header.NB*MAX_CHARS_TOVS+header.NC,TOVS_HEADER_SIZE);
    fprintf(f,"number of blocks: %d block\n",header.NB);
    // insertion log
    fputs("\t2) INSERTION LOG :\n",f);
    fprintf(f,"number of inserted records: %d \n",inserSummary[INSERT_SUCCUSFUL]+inserSummary[INSERT_SUCCUSFUL_STRUDLE]);
    fprintf(f,"number of strudle records: %d\n",inserSummary[INSERT_SUCCUSFUL_STRUDLE]);
    fprintf(f,"number of duplicat not inserted: %d",inserSummary[RECORD_EXISTS]);
    fprintf(f,"average records per block: %.2f\n",(double)(inserSummary[INSERT_SUCCUSFUL]+inserSummary[INSERT_SUCCUSFUL_STRUDLE])/(double)header.NB);
    // source file status
    fputs("\t2) SOURCE LOG :\n",f);
    fprintf(f,"number of valid lines: %d\n",linesSummary[VALID_LINE]);
    fprintf(f,"number of empty lines: %d\n",linesSummary[EMPTY_LINE]);
    fprintf(f,"number of lines missing id: %d\n",linesSummary[LINE_MISSING_ID]);
    fprintf(f,"number of lines missng description: %d\n",linesSummary[LINE_MISSING_DESCRIPTION]);
    
    

}
/*
0 *
1 *
2 
3
4
5

0
1
2 -
3
4
5

*/