#include <TOF.h>
#include <time.h>
struct tm theTime;
int TOF_NUMBER_OF_READS=0;
int TOF_NUMBER_OF_WRITES=0;
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


int TOF_setHeader(TOF_FILE *f , TOF_Header *header){
	f->header=(*header);
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

int TOF_search(TOF_FILE *f, int key, bool *found, int (*i), int (*j), Student *student) {
    TOF_Header header;
    TOF_Buffer buffer;

    // Get the file header to understand the structure
    TOF_getHeader(f, &header);

    // Case: Empty file
    if (header.NB == 0) {
        (*i) = 1;  // Suggest insertion in the first block
        (*j) = 0;  // Suggest insertion at the start of the block
        (*found) = false;
        return EXIT_SUCCESS;
    }

    int low = 1, high = header.NB; // Block range (1-based indexing)
    (*found) = false;

    // Perform binary search across blocks
    while (low <= high) {
        (*i) = (low + high) / 2; // Middle block
        TOF_readBlock(f, (*i), &buffer);

        // Check if the key falls within the range of this block
        if (key >= buffer.data[0].id && key <= buffer.data[buffer.NR - 1].id) {
            // Perform binary search within the block
            int block_low = 0, block_high = buffer.NR - 1;

            while (block_low <= block_high) {
                (*j) = (block_low + block_high) / 2;

                if (key == buffer.data[(*j)].id && !buffer.del[(*j)]) {
                    (*found) = true;
                    if (student) (*student) = buffer.data[(*j)]; // Copy student data
                    return EXIT_SUCCESS;
                }

                if (key < buffer.data[(*j)].id) {
                    block_high = (*j) - 1;
                } else {
                    block_low = (*j) + 1;
                }
            }

            // Not found within the block; suggest insertion point
            (*j) = block_low;
            return EXIT_SUCCESS;
        }

        // Move search to appropriate half
        if (key < buffer.data[0].id) {
            high = (*i) - 1; // Search in the lower half
        } else {
            low = (*i) + 1; // Search in the upper half
        }
    }

    // Key not found in any block; suggest insertion in a new block
    if (high >0){
        (*i) = high;  // Suggest the next block for insertion
        TOF_readBlock(f,high,&buffer);
        (*j) = buffer.NR;    // Start at the first position of the new block
    } else {
        (*i)=1;
        (*j)=0;
    }    
    return EXIT_SUCCESS;
}


enum TOF_INSERT_STATUS TOF_inserWithLoadingFactor(TOF_FILE *f , Student e){
    int i,j;
    bool found;
    TOF_search(f,e.id,&found,&i,&j,NULL);
    if (found) return TOF_RECORD_EXISTS;
    bool stop=false;
    TOF_Header header;
    TOF_Buffer buffer;
    Student tmp;
    TOF_getHeader(f,&header);
    while (!stop && i<=header.NB){
        // Read the block
        TOF_readBlock(f, i, &buffer);
        // Shift records to make space
        for (int k =buffer.NR; k > j; k--) {
            buffer.data[k] = buffer.data[k - 1];
        }
        buffer.data[j] = e;
        buffer.NR++;
        if (buffer.NR > MAX_RECORDS*LOADING_FACTOR){
            e=buffer.data[buffer.NR-1];
            buffer.NR--;
            TOF_writeBlock(f,i,&buffer);
            i++;
            j=0;
        } else {
            stop=true;
        }
    }
    if (i>header.NB){
        header.NB++;
        memset(&buffer,0,TOF_blockSize);
        buffer.data[0]=e;
        buffer.NR++;
    }
    TOF_writeBlock(f,i,&buffer);
    header.NR++;
    TOF_setHeader(f,&header);
    return TOF_INSERT_SUCCUSFUL;
}

enum TOF_INSERT_STATUS insertElement(TOF_FILE *f, Student e) {
    int i, j, k;
    bool found;
    TOF_Buffer buffer = {{0}, {false}, 0, 0};
    Student tmp;
    TOF_Header header;

    // Retrieve header
    TOF_getHeader(f, &header);

    // Search for the record
    TOF_search(f, e.id, &found, &i, &j, &tmp);
//    printf("Search result: i = %d, j = %d, found = %d\n", i, j, found);
      if (found) {
        return TOF_RECORD_EXISTS; // Record already exists
    }

    bool stop = false;
    while (!stop && i <= header.NB) {
        // Read the block
        TOF_readBlock(f, i, &buffer);

        // Shift records to make space
        for (k = buffer.NR; k > j; k--) {
            buffer.data[k] = buffer.data[k - 1];
        }
        buffer.data[j] = e;

        // Check if block can accommodate the new record
        if ((buffer.NR - buffer.ND + 1) < (int)(MAX_RECORDS * LOADING_FACTOR)) {
            buffer.NR++;
            TOF_writeBlock(f, i, &buffer);
            stop = true;
        } else {
            // Prepare for the next block
            tmp = buffer.data[MAX_RECORDS - 1]; // Last element of the full block
            buffer.NR = MAX_RECORDS;           // Mark block as full
            TOF_writeBlock(f, i, &buffer);

            // Move to the next block
            i++;
            j = 0;
            e = tmp;
        }
    }

    // If a new block is required
    if (i > header.NB) {
        memset(&buffer, 0, sizeof(buffer)); // Initialize a new block
        buffer.data[0] = e;
        buffer.NR = 1;
        TOF_writeBlock(f, i, &buffer);
        header.NB++;
    }

    // Update header
    header.NR++;
    TOF_setHeader(f, &header);

    return TOF_INSERT_SUCCUSFUL; // Successful insertion
}

void TOF_writeLineToLog(FILE *f , int lineNumber , enum TOF_LINE_STATUS lineS , enum TOF_INSERT_STATUS insertS){
    if (insertS==TOF_INSERT_SUCCUSFUL){
        switch (lineS)
        {
        case TOF_EMPTY_LINE:
            fprintf(f,"+%*d:INSERTED:EMPTY-LINE:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        
        case TOF_VALID_LINE:
            fprintf(f,"+%*d:INSERTED:VALIDE_LINE:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case TOF_MISSING_BIRTH_CITY:
            fprintf(f,"+%*d:INSERTED:MISSING_BIRTH_CITY:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case TOF_MISSING_FIRST_NAME:
            fprintf(f,"+%*d:INSERTED:MISSING_FIRST_NAME:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case TOF_MISSING_LAST_NAME:
            fprintf(f,"+%*d:INSERTED:MISSING_LAST_NAME:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        case TOF_MISSING_BIRTH_DATE:
            fprintf(f,"+%*d:INSERTED:MISSING_BIRTH_DATE:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            break;
        
        default:
            break;
        }
    } else {
        if (lineS==TOF_MISSING_ID)fprintf(f,"*%*d:NOT-INSERTED:MISSING_ID:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
        else fprintf(f,"*%*d:NOT-INSERTED:DUPLICATE:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);

    }
}

void TOF_writeSummaryToLog(FILE *f,TOF_FILE *tof,int Totalr,int Totalw ,int fragment, int *lineStat , int *insertStat ){
    TOF_Header header;
    TOF_getHeader(tof,&header);
    fputs("///////////\t\t\tTHE SUMMARY\t\t\t///////////\n",f);
    // created file informations
    fputs("\t1) FILE INFORMATIONS :\n",f);
    fprintf(f,"file name:\"%s\"\n",TOF_FILE_NAME);
    _getsystime(&theTime);
    fprintf(f,"created on : %02d-%02d-%04d %02d:%02d:%02d\n",theTime.tm_mday, theTime.tm_mon, theTime.tm_year, 
            theTime.tm_hour, theTime.tm_min, theTime.tm_sec);
    fprintf(f,"number of blocks: %d block\n",header.NB);
    fprintf(f,"number of records inserted: %d\n",header.NR);
    fprintf(f,"file loading factor: %d %%\n",header.NR*100/(header.NB*MAX_RECORDS));
    fprintf(f,"fragmented space: %d Byte\n",fragment);
    // insertion performance
    fputs("\t2) INSERTION LOG :\n",f);
    fprintf(f,"total read operation: %d\n",Totalr);
    fprintf(f,"total write operation: %d\n",Totalw);
    // source file status
    fputs("\t2) SOURCE LOG :\n",f);
    fprintf(f,"number of valid line: %d\n",lineStat[TOF_VALID_LINE]);
    fprintf(f,"number of lines missing id: %d\n",lineStat[TOF_MISSING_ID]);
    fprintf(f,"number of lines missing first name: %d\n",lineStat[TOF_MISSING_FIRST_NAME]);
    fprintf(f,"number of lines missing last name: %d\n",lineStat[TOF_MISSING_LAST_NAME]);
    fprintf(f,"number of lines missing birth date: %d\n",lineStat[TOF_MISSING_BIRTH_DATE]);    
    fprintf(f,"number of lines missing birth city : %d\n",lineStat[TOF_MISSING_BIRTH_CITY]) ;       
    fprintf(f,"number of duplicates id: %d\n",insertStat[TOF_RECORD_EXISTS]);
    int sum=0 ;
    for (int i =0;i<TOF_N_LINE_STATUS;i++)sum+=lineStat[i];
    fprintf(f,"total lines: %d",sum);

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










int TOF_recordFragmentedSpace(Student s){
    int i=0;
    i+=(MAX_NAME_SIZE-strlen(s.firstName));
    i+=(MAX_NAME_SIZE-strlen(s.lastName));
    i+=(MAX_CITY_NAME-strlen(s.birthCity));
    i+=(DATE_SIZE-strlen(s.birthDate));
    return i;
}
void TOF_LineToRecord(char* line,Student* student,enum TOF_LINE_STATUS *LineStatus)
{
   
     char fields[NUM_FIELDS][MAX_FIELD_LENGTH];
     parseLine(line,fields);
    
        student->id=atoi(fields[0]);
        strcpy(student->firstName,fields[1]);
        strcpy(student->lastName,fields[2]);
        strcpy(student->birthDate,fields[3]);
        strcpy(student->birthCity,fields[4]);
        (*LineStatus)=TOF_VALID_LINE;
      if (student->id==0) (*LineStatus)=TOF_MISSING_ID;
      if (strcmp(student->firstName,"")==0) (*LineStatus) = TOF_MISSING_FIRST_NAME;
      if (strcmp(student->lastName,"")==0) (*LineStatus) = TOF_MISSING_LAST_NAME;
      if (strcmp(student->birthDate,"")==0) (*LineStatus) = TOF_MISSING_BIRTH_DATE;
      if (strcmp(student->birthCity,"")==0) (*LineStatus) = TOF_MISSING_BIRTH_CITY;
      if(!((*LineStatus)==TOF_MISSING_ID||(*LineStatus)==TOF_MISSING_FIRST_NAME||(*LineStatus)==TOF_MISSING_LAST_NAME||(*LineStatus)==TOF_MISSING_BIRTH_DATE||(*LineStatus)==TOF_MISSING_BIRTH_CITY))
      (*LineStatus)=TOF_VALID_LINE;


}

int TOF_createFile(TOF_FILE *dest , FILE *src , FILE *logFile)
{
if ((dest==NULL)||(src==NULL)) return -1;
    TOF_Header header={0,0,0};
    TOF_setHeader(dest,&header);
    TOF_Buffer buffer;
    char line[MAX_LINE_SIZE];
    enum TOF_INSERT_STATUS insertStatus;
    int lineNumber=0;
    enum TOF_LINE_STATUS LineStatus;
    Student student;
    int lol=0;
    // Skip the first line
    fgets(line,MAX_LINE_SIZE, src);

    /*
    *   log variables
    */
   int TotalWrites=0;
   int TotalReads=0;
   int insertSummary[TOF_N_INSERT_STATUS]={0};
   int linesSummary[TOF_N_LINE_STATUS]={0};
   int fragmentedSpace=0;
    while (fgets(line, MAX_LINE_SIZE, src))
    {
        TOF_NUMBER_OF_READS=0;
        TOF_NUMBER_OF_WRITES=0;
        TOF_LineToRecord(line,&student,&LineStatus);  
        // insertStatus=insertElement(dest,student);
        insertStatus = TOF_inserWithLoadingFactor(dest ,student);
        TOF_writeLineToLog(logFile ,lineNumber,LineStatus,insertStatus);
        // TOF_printFile(dest);
        fragmentedSpace+=TOF_recordFragmentedSpace(student);
        lineNumber++;
        TotalReads+=TOF_NUMBER_OF_READS;
        TotalWrites+=TOF_NUMBER_OF_WRITES;
        linesSummary[LineStatus]++;
        insertSummary[insertStatus]++;
    }
    TOF_getHeader(dest,&header);
    fragmentedSpace+=((header.NB*MAX_RECORDS) - header.NR)*StudentSize;
    TOF_writeSummaryToLog(logFile,dest,TotalReads,TotalWrites,fragmentedSpace,linesSummary,insertSummary);
    
}

void TOF_printFile(TOF_FILE *f){
    TOF_Buffer buffer;
    Student s;
    for (int i=1;i<=f->header.NB;i++){
        TOF_readBlock(f,i,&buffer);
        printf("block %d=========%d \n",i,buffer.NR);
        for (int j=0;j<buffer.NR;j++){
            s=buffer.data[j];
            if(j>0 && s.id<buffer.data[j-1].id)printf("%d|%s|%s|%.10s|%s\n",s.id,s.firstName,s.lastName,s.birthDate,s.birthCity);
        }
    }
    fprintf(stderr,"done");
}
void TOF_printStudent(Student s){
    printf("%d|%s|%s|%.10s|%s\n",s.id,s.firstName,s.lastName,s.birthDate,s.birthCity);
}
