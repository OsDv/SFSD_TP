#include <TOF.h>
#include <windows.h>

TOF_SI_BirthDate BirthDateIndex={0};
TOF_PI_ID TOF_primaryIndex[10000];
int primaryIndexSize;
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
    file->file= fopen(name,"rb+");
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
FILE * TOF_getFile(TOF_FILE *f){
    return f->file;
}
void TOF_setFile(TOF_FILE *f,FILE *file){
    f->file=file;
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

int TOF_search(TOF_FILE *f, int key, bool *found, int *i, int *j, Student *student) {
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
                    if(!buffer.del[(*j)]){
                        (*found) = true;
                        if (student) (*student) = buffer.data[(*j)]; // Copy student data
                    }
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
    if (insertS==TOF_RECORD_EXISTS){
        fprintf(f,"- %*d | NOT-INSERTED | DUPLICAT | %4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
        return;
    }
    if (insertS==TOF_NOT_INSERTED){
        if (lineS==TOF_EMPTY_LINE) {
            fprintf(f,"-%*d | NOT-INSERTED | EMPTY-LINE | %4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
            return;
        }
        else fprintf(f,"- %*d | NOT-INSERTED | ",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber);//:%4dR %4dW\n",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
    } else {
        fprintf(f,"+ %*d | INSERTED | ",TOF_PRINT_LINE_NUMBER_WIDTH,lineNumber);        
    }
    if (lineS==TOF_VALID_LINE)fprintf(f,"VALID LINE ");
    else {
        fprintf(f,"MISSING: ");
        if (lineS&TOF_MISSING_ID)fprintf(f,"ID ");
        if (lineS&TOF_MISSING_FIRST_NAME)fprintf(f,"FIRST_NAME ");
        if (lineS&TOF_MISSING_LAST_NAME)fprintf(f,"LAST_NAME ");
        if (lineS&TOF_MISSING_BIRTH_DATE)fprintf(f,"BIRTH_DATE ");
        if (lineS&TOF_MISSING_BIRTH_CITY)fprintf(f,"BIRTH_CITY ");
    }
    fprintf(f,"| %4dR %4dW\n",TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
    
}

void TOF_writeSummaryToLog(FILE *f,TOF_FILE *tof,int Totalr,int Totalw ,int fragment, int *lineStat , int *insertStat ){
    TOF_Header header;
    TOF_getHeader(tof,&header);
    fputs("///////////\t\t\tTHE SUMMARY\t\t\t///////////\n",f);
    // created file informations
    fputs("\t1) FILE INFORMATIONS :\n",f);
    fprintf(f,"file name:\"%s\"\n",TOF_FILE_NAME);
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);
    fprintf(f,"created on : %02d-%02d-%04d %02d:%02d:%02d\n",systemTime.wDay, systemTime.wMonth, systemTime.wYear, 
            systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
    fprintf(f,"number of blocks: %d block\n",header.NB);
    fprintf(f,"number of records inserted: %d\n",header.NR);
    fprintf(f,"file loading factor: %d %%\n",header.NR*100/(header.NB*MAX_RECORDS));
    fprintf(f,"fragmented space: %d Byte\n",fragment);
    // insertion performance
    fputs("\t2) INSERTION LOG :\n",f);
    fprintf(f,"total read  operation: %d\n",Totalr);
    fprintf(f,"total write operation: %d\n",Totalw);
    // source file status
    fputs("\t2) SOURCE LOG :\n",f);
    fprintf(f,"number of valid line: %d\n",lineStat[(int)log2(TOF_VALID_LINE)]);
    fprintf(f,"number of lines missing id: %d\n",lineStat[(int)log2(TOF_MISSING_ID)]);
    fprintf(f,"number of lines missing first name: %d\n",lineStat[(int)log2(TOF_MISSING_FIRST_NAME)]);
    fprintf(f,"number of lines missing last name: %d\n",lineStat[(int)log2(TOF_MISSING_LAST_NAME)]);
    fprintf(f,"number of lines missing birth date: %d\n",lineStat[(int)log2(TOF_MISSING_BIRTH_DATE)]);    
    fprintf(f,"number of lines missing birth city : %d\n",lineStat[(int)log2(TOF_MISSING_BIRTH_CITY)]) ;       
    fprintf(f,"number of EMPTY lines : %d\n",lineStat[(int)log2(TOF_EMPTY_LINE)]) ;           
    fprintf(f,"number of duplicates id: %d\n",insertStat[TOF_RECORD_EXISTS]);
    int sum=0 ;
    for (int i =0;i<TOF_N_INSERT_STATUS;i++)sum+=insertStat[i];
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
    (*LineStatus)=0;
    if (*line=='\0'||*line=='\n'){
        (*LineStatus)=TOF_EMPTY_LINE;
        return;
    }
    char fields[NUM_FIELDS][MAX_FIELD_LENGTH];
    parseLine(line,fields);
    student->id=atoi(fields[0]);
    strcpy(student->firstName,fields[1]);
    strcpy(student->lastName,fields[2]);
    strcpy(student->birthDate,fields[3]);
    strcpy(student->birthCity,fields[4]);
    (*LineStatus)=TOF_VALID_LINE;
    if (student->id==0) (*LineStatus)|=TOF_MISSING_ID;
    if (strcmp(student->firstName,"")==0) (*LineStatus) |= TOF_MISSING_FIRST_NAME;
    if (strcmp(student->lastName,"")==0) (*LineStatus) |= TOF_MISSING_LAST_NAME;
    if (strcmp(student->birthDate,"")==0) (*LineStatus) |= TOF_MISSING_BIRTH_DATE;
    if (strcmp(student->birthCity,"")==0) (*LineStatus) |= TOF_MISSING_BIRTH_CITY;
    if ((*LineStatus)!=TOF_VALID_LINE) (*LineStatus)&=(~TOF_VALID_LINE);
}
void TOF_updateLinesSummary(int *lineSummary,enum TOF_LINE_STATUS lineStatus){
    for (int i=0;i<TOF_N_LINE_STATUS;i++){
        if (lineStatus%2)lineSummary[i]++;
        lineStatus=lineStatus>>1;
    }
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
        // empty line or missing id
        if (LineStatus==TOF_EMPTY_LINE || (LineStatus>>1)%2==1){
            insertStatus=TOF_NOT_INSERTED;
        } else {
            insertStatus = TOF_inserWithLoadingFactor(dest ,student);
        }
        lineNumber++;
        TOF_writeLineToLog(logFile ,lineNumber,LineStatus,insertStatus);
        fragmentedSpace+=TOF_recordFragmentedSpace(student);
        showProgressBar(lineNumber,NumberOfLinesCSV1);
        TotalReads+=TOF_NUMBER_OF_READS;
        TotalWrites+=TOF_NUMBER_OF_WRITES;
        if (insertStatus==TOF_INSERT_SUCCUSFUL)TOF_updateLinesSummary(linesSummary,LineStatus);
        insertSummary[insertStatus]++;
    }
    TOF_getHeader(dest,&header);
    fragmentedSpace+=((header.NB*MAX_RECORDS) - header.NR)*StudentSize;
    TOF_writeSummaryToLog(logFile,dest,TotalReads,TotalWrites,fragmentedSpace,linesSummary,insertSummary);
    return 0;
    
}

void TOF_printFile(TOF_FILE *f){
    TOF_Buffer buffer;
    Student s;
    for (int i=1;i<=f->header.NB;i++){
        TOF_readBlock(f,i,&buffer);
        printf("block %d=========%d \n",i,buffer.NR);
        for (int j=0;j<buffer.NR;j++){
            s=buffer.data[j];
            printf("%d|%s|%s|%.10s|%s\n",s.id,s.firstName,s.lastName,s.birthDate,s.birthCity);
        }
    }
    fprintf(stderr,"done");
}
void TOF_printStudent(Student s){
    printf("%d|%-*s|%-*s|%.10s|%s\n",s.id,MAX_NAME_SIZE,s.firstName,MAX_NAME_SIZE,s.lastName,s.birthDate,s.birthCity);
}
bool TOF_deleteRecord(TOF_FILE *f,int id)
{
TOF_Buffer buffer;
bool found;
int i,j;
TOF_search(f,id,&found,&i,&j,NULL);
if (!found) return false; //NOT FOUND 
TOF_readBlock(f,i,&buffer);
buffer.del[j]=true;
buffer.ND++;
// buffer.NR--;
TOF_writeBlock(f,i,&buffer);
return true;
}
void TOF_writeLineTodeleteLog(FILE *log ,int id,bool status){
    if (status){
        fprintf(log,"%d|DELETED|%d read %d write\n",id,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);
    } else {
        fprintf(log,"%d|not-found|%d read %d write\n",id,TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);        
    }
}
void TOF_deletefromfile(TOF_FILE *tof , FILE *list , FILE *logFile) //list == delete_students.csv
{
    
if ((tof==NULL)||(list==NULL))  return;
    TOF_Buffer buffer;
    TOF_Header header;
    TOF_getHeader(tof,&header);
    char line[MAX_LINE_SIZE];
    int lineNumber=0;
    int id;
    int numberNotFound=0;
    int numberDeleted=0;
    bool DeleteStatus;
    // Skip the first line
    fgets(line,MAX_LINE_SIZE, list);
    
    int TotalWrites=0;
    int TotalReads=0;
    while(fgets(line,MAX_LINE_SIZE, list))
    {
    TOF_NUMBER_OF_READS=0;
    TOF_NUMBER_OF_WRITES=0;
    id=atoi(line);
    DeleteStatus=TOF_deleteRecord(tof,id);
    if (DeleteStatus) numberDeleted++ ; else numberNotFound++;//for summary
    TOF_writeLineTodeleteLog(logFile,id,DeleteStatus);
    //showProgressBar(lineNumber++,NumberOfLinesDelete);
    TotalReads+=TOF_NUMBER_OF_READS;
    TotalWrites+=TOF_NUMBER_OF_WRITES;
 
    }
    header.ND+=numberDeleted;
    // header.NR-=numberDeleted;
    TOF_setHeader(tof,&header);
    int LoadingFactor=(header.NR-header.ND)/header.NB;
    TOF_deleteWriteSummaryToLog(logFile,TotalReads,TotalWrites,LoadingFactor,numberDeleted,numberNotFound); 
    TOF_NUMBER_OF_READS=TotalReads;
    TOF_NUMBER_OF_WRITES=TotalWrites;
    return;
}

void TOF_deleteWriteSummaryToLog(FILE *log , int totalR,int totalW,int LoadingFactor,int deleted,int notFound){
    fputs("\n====\tDelete Summary\t====\n",log);
    fprintf(log,"Total deleted: %d\n",deleted);
    fprintf(log,"Total not found: %d\n",notFound);
    fprintf(log,"Total reads %d\twrites %d\n",totalR,totalW);
    fprintf(log,"New Loading Factor: %d %%\n",LoadingFactor);
}
/*  TP PRESENCIEL   */
void TOF_searchInterBlock(TOF_Buffer buffer ,int id ,int *pos, bool *found){
    int i;
    int inf=0;
    int sup= buffer.NR;
    (*found)=false;
    while (!(*found)&&(inf<=sup))
    {
        i=(int)((inf+sup)/2);
        if (id==buffer.data[i].id){
            (*found)=true;
            (*pos)=i;
        }else{
            if (id<buffer.data[i].id)sup=i-1;
            else inf = i+1;
        }
    }
    if (*found && buffer.del[i]) (*found)=false;
    if (!(*found)) (*pos)=inf;
    
}
int CompareDates(char *date1 , char *date2){
    int result;
    result = strncmp(&(date1[6]),&(date2[6]),4);
    if (result!=0) return result;
    result = strncmp(&(date1[3]),&(date2[3]),2);
    if (result!=0) return result;    
    result = strncmp(&(date1[0]),&(date2[0]),2);
    return result;
}
// 11/11/1111
void TOF_searchSIonBirthDate(TOF_SI_BirthDate *index,char *date,int id , int *pos , bool *found){
    (*found)=false;
    if (index->size==0){
        (*pos)=0;
        return;
    }
    int sup,inf;
    sup = index->size-1;
    inf =0;
    int result;
    int i;
    bool stop=false;
    TOF_SI_BirthDate_LIST *list;
    while (!stop && sup>=inf){
        i= (int)((sup+inf)/2);
        result = CompareDates(date,index->tab[i].birthDate);
        if (result<0) sup =i-1;
        else if (result>0) inf =i+1;
        else {
            list=index->tab[i].list;
            while (list!=NULL){
                if (list->id==id){
                    (*found)=true;
                }
                list=list->next;
            }
            inf=i;
            stop=true;
        }
    }
    (*pos)=inf;
}
void TOF_shiftSIonBirthDate(TOF_SI_BirthDate *index , int pos ,int step){
    for (int i=index->size-1;i>=pos;i--){
        index->tab[i+step]=index->tab[i];
    }
}
void TOF_insertSIonBirthDate(TOF_SI_BirthDate *index,char *date ,int id ){
    bool found;
    int pos;
    TOF_searchSIonBirthDate(index,date,id,&pos,&found);
    if (found) return;
    TOF_SI_BirthDate_LIST *new=malloc(sizeof(TOF_SI_BirthDate_LIST));
    new->id=id;
    new->next=NULL;
    if (pos == index->size){
        strncpy((index->tab[pos].birthDate),date,DATE_SIZE);
        index->tab[pos].list=new;
        index->size++;
    } else {
        if (!strncmp(date,index->tab[pos].birthDate,DATE_SIZE)){
            TOF_SI_BirthDate_LIST * list=index->tab[pos].list;
            index->tab[pos].list=new;
            new->next=list;
        } else {
            TOF_shiftSIonBirthDate(index,pos,1);
            strncpy((index->tab[pos].birthDate),date,DATE_SIZE);
            index->tab[pos].list=new;
            index->size++;
        }
    }

}
void TOF_creatSIonBirthDate(TOF_FILE *file ,TOF_SI_BirthDate *dest){
    if (dest->size!=0){
        TOF_SI_BirthDate_LIST *list;
        for(int i=0;i<dest->size;i++){
            list=dest->tab[i].list;
            dest->tab[i].list=dest->tab[i].list->next;
            free(list);
        }
    }
    TOF_Header header;
    TOF_getHeader(file,&header);
    TOF_Buffer buffer;
    int index;
    int count=0;
    for(int i=1;i<=header.NB;i++){
        TOF_readBlock(file,i,&buffer);
        for (int j=0;j<buffer.NR;j++){
            if ((!buffer.del[j])&&(buffer.data[j].birthDate[0]!='\0')){
                TOF_insertSIonBirthDate(dest,buffer.data[j].birthDate,buffer.data[j].id);
            }
        }
    }
}
void TOF_printSIonBirthDate(TOF_SI_BirthDate *src){
    TOF_SI_BirthDate_LIST *list;
    for (int i=0;i<src->size;i++){
        printf("%.*s|",DATE_SIZE,src->tab[i].birthDate);
        list=src->tab[i].list;
        while(list){
            printf("%d|",list->id);
            list=list->next;
        }
        printf("\n");
    }
}
/*
* to save the index in file we transform each linked list to an array 
* and save it's size with the correspending date 
*/
void TOF_transformToArraySIonBirthDate(TOF_SI_BirthDate *index,TOF_SI_BirthDate_file *dest){
    int *id_array;
    TOF_SI_BirthDate_LIST *list;
    int n=0;
    for (int i=0;i<index->size;i++){
        strncpy(dest[i].date,index->tab[i].birthDate,DATE_SIZE);
        list=index->tab[i].list;
        n=0;
        while (list!=NULL){
            n++;
            list=list->next;
        }
        dest[i].n=n;
        id_array = malloc(sizeof(int)*n);
        list=index->tab[i].list;
        for (int j=0;j<n;j++){
            id_array[j]=list->id;
            list=list->next;
        }
        dest[i].id_array=id_array;
    }
}
void TOF_saveSIonBirthDate(TOF_SI_BirthDate *index,FILE *file){
    TOF_SI_BirthDate_file *arrayOfIndex=malloc(sizeof(TOF_SI_BirthDate_file)*(index->size));
    TOF_transformToArraySIonBirthDate(index,arrayOfIndex);
    fwrite(&(index->size),sizeof(int),1,file);
    for(int i=0;i<index->size;i++){
        fwrite(arrayOfIndex[i].date,DATE_SIZE,1,file);
        fwrite(&(arrayOfIndex[i].n),sizeof(int),1,file);
        fwrite(arrayOfIndex[i].id_array,sizeof(int),arrayOfIndex[i].n,file);
        free(arrayOfIndex[i].id_array);
    }
    free(arrayOfIndex);
}
void TOF_loadSIonBirthDate(TOF_SI_BirthDate *index,FILE *file){
    fread(&(index->size),sizeof(int),1,file);
    int n;
    TOF_SI_BirthDate_LIST *list;
    for(int i=0;i<index->size;i++){
        showProgressBar(i,index->size-1);
        fread(index->tab[i].birthDate,DATE_SIZE,1,file);
        fread(&n,sizeof(int),1,file);
        index->tab[i].list=NULL;
        for (int j=0;j<n;j++){
            list=malloc(sizeof(TOF_SI_BirthDate_LIST));
            fread(&(list->id),sizeof(int),1,file);
            list->next=index->tab[i].list;
            index->tab[i].list=list;
        }
    }    
}
void TOF_creatPrimaryIndex(TOF_FILE *f, FILE*dest,int* size,TOF_PI_ID *tab )
{
TOF_Header header;
TOF_getHeader(f,&header);
TOF_Buffer buffer;
int i;
int j;
int index=0;
TOF_PI_ID IndexBuf;
for ( i=1; i <= header.NB; i++)
{
 TOF_readBlock(f,i,&buffer);
 j=buffer.NR;
 while (buffer.del[j] && j!=-1)
 {
 j--;
 }
 if (j>-1)
 {
 IndexBuf.id=buffer.data[j].id;
 IndexBuf.block=i;
 IndexBuf.pos=j;
 tab[index++]=IndexBuf;
 }
}
(*size)=index;
fwrite(&index,sizeof(int),1,dest);
fwrite(tab,sizeof(TOF_PI_ID),10000,dest);
}
void TOF_loadPrimaryIndex(FILE *src ,TOF_PI_ID *tab){
    if (tab==NULL || src==NULL) return;
    fread(&primaryIndexSize,sizeof(int),1,src);
    fread(tab,sizeof(TOF_PI_ID),10000,src);
}
void TOF_searchPrimaryIndex(int id , int *block){
    int inf =0;
    int sup=primaryIndexSize-1;
    bool stop=false;
    int i;
    while(!stop){
        i=(int)((sup+inf)/2);
        if (id<TOF_primaryIndex[i].id)sup=i-1;
        else inf=i+1;
        if (id==TOF_primaryIndex[i].id){
            stop=true;
            (*block)=TOF_primaryIndex[i].block;
        } else{
            if (sup<inf){
                stop=true;
                (*block)=TOF_primaryIndex[inf].block;
            }
        }
    }
}
void TOF_BirthDateIntervalQuery(TOF_FILE *tof , TOF_PI_ID *pIndex , TOF_SI_BirthDate *sIndex,char* inf,char* sup){
    TOF_NUMBER_OF_READS=0;
    TOF_NUMBER_OF_WRITES=0;
    ID_LIST *head=NULL,*list;
    TOF_SI_BirthDate_LIST *indexList;
    int pos;
    bool found;
    int QuerySize=0;
    TOF_searchSIonBirthDate(sIndex,inf,0,&pos,&found);
    while(CompareDates(sIndex->tab[pos].birthDate,sup)<=0){
        indexList=sIndex->tab[pos].list;
        while(indexList){
            IDlist_insertOrd(&head,indexList->id);
            indexList=indexList->next;
            QuerySize++;
        }
        pos++;
    }
    if (QuerySize==0)return;
    int block,offset;
    int oldBlock=-1;
    TOF_Buffer buffer;
    list=head;
    while(list){
        TOF_searchPrimaryIndex(list->id,&block);
        if (block!=oldBlock){
            TOF_readBlock(tof,block,&buffer);
            oldBlock=block;
        }
        TOF_searchInterBlock(buffer,list->id,&offset,&found);
        if (found)TOF_printStudent(buffer.data[offset]);
        
        list=list->next;
        free(head);
        head=list;
    }
    printf("THE QUERY TOOK: %d READ %d WRITE\n",TOF_NUMBER_OF_READS,TOF_NUMBER_OF_WRITES);

}
