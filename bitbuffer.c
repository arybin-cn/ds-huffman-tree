#include <stdio.h>
#include <malloc.h>

#include "bitbuffer.h"

#define BITS_OF_CHAR 8

PBitBuffer buildBitBuffer(int bits){
  int byteSize;
  PBitBuffer pBitBuffer;
  void *buffer;
  if(bits > 0){
    pBitBuffer=(PBitBuffer)malloc(sizeof(BitBuffer));
    byteSize=(bits/BITS_OF_CHAR)+((bits%BITS_OF_CHAR)!=0);
    buffer=malloc(byteSize);
    if(pBitBuffer && buffer){
      pBitBuffer->byteSize=byteSize;
      pBitBuffer->buffer=buffer;
      pBitBuffer->cursor=0;
      return pBitBuffer;
    }
    free(buffer);
    free(pBitBuffer);
  }
  printf("Fail to build BitBuffer..");
  return NULL;
}

int appendBitBufferByChar(PBitBuffer pBitBuffer,char charData,int bitLengthData){
  int bitLengthCurrent;
  int maxBits=pBitBuffer->byteSize*BITS_OF_CHAR;
  char* current;
  if((pBitBuffer->cursor+bitLengthData)>maxBits){
    printf("Insufficient buffer..\n");
    return 0;
  }
  current=((char*)pBitBuffer->buffer)+pBitBuffer->cursor/BITS_OF_CHAR;
  bitLengthCurrent=pBitBuffer->cursor%BITS_OF_CHAR;
  (*current)|=(charData<<bitLengthCurrent);
  (*(current+1))|=(charData>>(BITS_OF_CHAR-bitLengthCurrent));
  pBitBuffer->cursor+=bitLengthData;
  return bitLengthData;
}

void printBuffer(void* buffer,int bitSize,int bitsPerGroup,int groupsPerLine){
  int i=0,j=0,k=0,l=0;
  char* current;
  for(;i<bitSize;i++){
    current=((char*)buffer)+i/BITS_OF_CHAR;
    printf("%d",(int)(((*current)&(1<<j))!=0));
    if((++k%bitsPerGroup)==0){
      k=0;printf(" ");
      if((++l%groupsPerLine)==0){
        l=0;printf("\n");
      }
    }
    j++;j%=BITS_OF_CHAR;
  }
  printf("\n");
}

void printBitBuffer(PBitBuffer pBitBuffer,int bitsPerGroup,int groupsPerLine){
  printBuffer(pBitBuffer->buffer,pBitBuffer->byteSize*BITS_OF_CHAR,bitsPerGroup,groupsPerLine);
}

