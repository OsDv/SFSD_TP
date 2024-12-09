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

int TOF_setHeader(TOF_FILE *f, TOF_Header *header){
	(*header)=f->header;
}
int TOF_getHeader(TOF_FILE *f , TOF_Header *header){
	(*header)=f->header;
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
int TOF_creatFile()
