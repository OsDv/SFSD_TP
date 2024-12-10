#include <TOF.h>
int TOF_NUMBER_OF_READS;
int TOF_NUMBER_OF_WRITES;
int TOF_open(const char *name , TOF_FILE *file , char mode){
    switch (mode)
    {
    case 'n':
    file->file= fopen(name,"wb+");
    file->header.NB=0;
    file->header.ND=0;
    file->header.NR=0;
        break;
    case 'r':
    file->file= fopen(name,"rb");
	fseek(file->file , 0 , SEEK_SET );
	fread(&(file->header) , TOF_HEADER_SIZE , 1,file->file);
        break;
    case 'a':
    file->file= fopen(name,"rb+");
	fseek(file->file , 0 , SEEK_SET );
	fread(&(file->header) , TOF_HEADER_SIZE , 1,file->file);        
        break;
    
    default:
        break;
    }
}

int TOF_close(TOF_FILE *file){
	fseek(file->file , 0 , SEEK_SET );
	fwrite(&(file->header) , TOF_HEADER_SIZE , 1,file->file);    
    fclose(file->file);
    file->file=NULL;
}

int TOF_getHeader(TOF_FILE *f , TOF_Header *header){
	(*header)=f->header;
}
#define MAX_LINE_LENGTH 1024
#define NUM_FIELDS 5


int TOF_getHeader(FILE *f , TOF_Header *header){
	fseek(f , 0 , SEEK_SET);
	fread(header , TOF_HEADER_SIZE , 1,f);
}

int TOF_readBlock(TOF_FILE *f, int n , TOF_Buffer *buffer) {
	fseek(f->file , TOF_HEADER_SIZE+TOF_blockSize*(n-1) , SEEK_SET);
	fread(buffer ,TOF_blockSize , 1 , f->file); 
    TOF_NUMBER_OF_READS++;
}

int TOF_writeBlock(TOF_FILE * f , int n , TOF_Buffer *buffer){
	fseek(f->file ,  TOF_HEADER_SIZE+TOF_blockSize*(n-1), SEEK_SET);
	fwrite(buffer ,TOF_blockSize,1, f->file);
    TOF_NUMBER_OF_WRITES++;
}

int TOF_search(TOF_FILE *f , int key , bool *found , int *i , int *j , Student *student){
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
                sup = (*i)-1;
        } else {
            // the element should be found in the superior half of the file
            inf = (*i) + 1;
        }
        // search ended and element not founded should be inserted in block i index j
        if (inf > sup) stop = true ;
	}
}



enum INSERT_STATUS insertElement(TOF_FILE *f , Student e){
    int i,j;
    bool found;
    TOF_Buffer buffer,nextBuffer;
    Student tmp;
    TOF_Header header;
    TOF_getHeader(f,&header);
    TOF_search(f,e.id,&found,&i,&j,NULL);
    if (found) return RECORD_EXISTS;   //Already exist
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
            if (nextBlockIndex >= header.NB) header.NB++;  //---allocate block----\\ 
               
            
            TOF_readBlock(f, nextBlockIndex, &nextBuffer);
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
    return INSERT_SUCCUSFUL; // Successful insertion  
}

void TOF_writeLineToLog(FILE *f , int lineNumber , enum TOF_LINE_STATUS lineS , enum INSERT_STATUS insertS){{
    if (insertS==INSERT_SUCCUSFUL){
        fprintf(f,"+%*d:VALIDE_LINE:INSERTED:%5dR %5dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
    } else {
        switch (lineS)
        {
        case EMPTY_LINE:
            fprintf(f,"*%*d:NOT-INSERTED:EMPTY-LINE:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        
        case VALID_LINE:
            fprintf(f,"*%*d:NOT-INSERTED:DUPLICAT:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case MISSING_ID:
            fprintf(f,"*%*d:NOT-INSERTED:MISSING_ID:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case MISSING_BIRTH_CITY:
            fprintf(f,"*%*d:NOT-INSERTED:MISSING_BIRTH_CITY:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case MISSING_FIRST_NAME:
            fprintf(f,"*%*d:NOT-INSERTED:MISSING_FIRST_NAME:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case MISSING_LAST_NAME:
            fprintf(f,"*%*d:NOT-INSERTED:MISSING_LAST_NAME:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case MISSING_BIRTH_DATE:
            fprintf(f,"*%*d:NOT-INSERTED:MISSING_BIRTH_DATE:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        
        default:
            break;
        }
    }
}

}

//////////////////////////



void parseLine(char *line, char fields[NUM_FIELDS][MAX_FIELD_LENGTH]) {
    int field_index = 0;
    int char_index = 0;
    int i = 0;

    while (line[i] != '\n' && line[i] != '\0') {
        if (line[i] == ',') {
            fields[field_index][char_index] = '\0'; // End the current field
            field_index++;
            char_index = 0;
        } else {
            fields[field_index][char_index++] = line[i];
        }
        i++;
    }
    fields[field_index][char_index] = '\0'; // End the last field
}











void TOF_LineToRecord(char* line,Student* student,enum INSERT_STATUS *LineStatus)
{
   
     char fields[NUM_FIELDS][MAX_FIELD_LENGTH];
     parseLine(line,fields);
    
        student->id=atoi(fields[0]);
        strcpy(student->firstName,fields[1]);
        strcpy(student->lastName,fields[2]);
        strcpy(student->birthDate,fields[3]);
        strcpy(student->birthCity,fields[4]);

      if (strcmp(student->id,"")==0) (*LineStatus)=MISSING_ID;
      if (strcmp(student->firstName,"")==0) (*LineStatus) = MISSING_FIRST_NAME;
      if (strcmp(student->lastName,"")==0) (*LineStatus) = MISSING_LAST_NAME;
      if (strcmp(student->birthDate,"")==0) (*LineStatus) = MISSING_BIRTH_DATE;
      if (strcmp(student->birthCity,"")==0) (*LineStatus) = MISSING_BIRTH_CITY;
      if(!(LineStatus==MISSING_ID||LineStatus==MISSING_FIRST_NAME||LineStatus==MISSING_LAST_NAME||LineStatus==MISSING_BIRTH_DATE||LineStatus==MISSING_BIRTH_CITY))
      (*LineStatus)=VALID_LINE;


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
    insertStatus=insertElement(dest,student);
    TOF_writeLineToLog(logFile ,lineNumber,LineStatus,insertStatus);

    

    }

}


