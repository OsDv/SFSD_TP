#include <TOVS.h>
#include <string.h>
#include <stdbool.h>
int TOVS_setHeader(FILE *f , TOVS_Header *header){
	fseek(f , 0 , SEEK_SET );
	fwrite(header , TOVS_HEADER_SIZE , 1,f);
}
int TOVS_getHeader(FILE *f , TOVS_Header *header){
	fseek(f , 0 , SEEK_SET);
	fread(header , TOVS_HEADER_SIZE , 1,f);
}

int TOVS_readBlock(FILE * f , int n , TOVS_Block *buffer) {
	fseek(f , TOVS_HEADER_SIZE+TOVS_BLOCK_SIZE*(n-1) , SEEK_SET);
	fread(buffer ,TOVS_BLOCK_SIZE , 1 , f); 
}

int TOVS_writeBlock(FILE * f , int n , TOVS_Block *buffer){
	fseek(f ,  TOVS_HEADER_SIZE+TOVS_BLOCK_SIZE*(n-1), SEEK_SET);
	fwrite(buffer ,TOVS_BLOCK_SIZE,1, f);
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

int TOVS_search(FILE *f ,int key , bool *found , int *i , int *j  ){
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


int TOVS_writeString(FILE *f , char *src , int size , int block , int index){
    TOVS_Header header;
    TOVS_getHeader(f,&header);
    TOVS_Buffer buffer;
    if (block <= header.NB) TOVS_readBlock(f,block,&buffer);
    for (int i =0 ; i<size ;i++){
        if (index==MAX_CHARS_TOVS){
            index=0;
            TOVS_writeBlock(f,block++,&buffer);
        }
        buffer.data[index++] = src[i];
    }
    TOVS_writeBlock(f,blcok++,&buffer);


}
int TOVS_insert(FILE *f , char *src , int size){
    char id[TOVS_RECORDS_id_WIDTH+1];
    id[TOVS_RECORDS_id_WIDTH]=0;
    strncpy(id,&src[TOVS_RECORDS_SIZE_WIDTH],TOVS_RECORDS_id_WIDTH);
    int key = atoi(id);
    int i,j;
    bool found;
    TOVS_search(f,key,&found,&i,&j);
    if (found) return 1;
    TOVS_Header header;
    TOVS_getHeader(f,&header);
    int blockR=header.NB,indexR=header.NC-1;
    int blockW=header.NB+((size+header.NC)/MAX_CHARS_TOVS) , indexW = ((indexR+size)%MAX_CHARS_TOVS)-1;
    header.NB=blockW;
    header.NC=indexW+1;
    TOVS_setHeader(f,&header);
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
        TOVS_writeString(f,src,size,i,j);

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