#include <TOF.h>

const int MAX_NAME_SIZE = 20;
const int MAX_CITY_NAME = 20;
const int DATE_SIZE 8;
const int StudentSize = sizeof(Student);
const int MAX_RECORDS = 100;
const int TOF_blockSize = sizeof(TOF_Block);
const int TOF_HEADER_SIZE = sizeof(TOF_Header );


int TOF_setHeader(FILE *f , TOF_Header *header){
	fseek(f , 0 , SEEK_SET );
	fwrite(header , TOF_HEADER_SIZE , 1,f);
}
int TOF_getHeader(FILE *f , TOF_HEADER *header){
	fseek(f , 0 , SEEK_SET);
	fread(header , TOF_HEADER_SIZE , 1,f);
}

int TOF_readBlock(FILE * f , int n , TOF_Buffer *buffer) {
	fseek(f , TOF_HEADER_SIZE+TOF_blockSize*(n-1) , SEEK_SET);
	fread(buffer ,TOF_blockSize , 1 , f); 
}

int TOF_writeBlock(FILE * f , int n , TOF_Buffer *buffer
	fseek(f ,  TOF_HEADER_SIZE+TOF_blockSize*(n-1), SEEK_SET); 	fwrite(buffer ,TOF_blockSize,1, f);
}

int TOF_search(FILE *f , int key , bool found , int i , int j){
	
}
