#include <stdio.h>
#include <malloc.h>
#include "bitbuffer.h"
#include "htencoder.h"
#include "httree.h"

#define DBG

int strLen(char* str){
  int i=0;
  while(str[i]!='\0') i++;
  return i;
}

int analyseString(char* str,OUT char** occurs,OUT int **occursCount,OUT char** maps){
  //assume that str is made up of 128 printable ascii chars
  char currentChar;
  int uniqCount=0;
  int j,i=0,tmpOccurs[128]={0};
  while((currentChar=str[i])!='\0'){
    if(currentChar>-1 && currentChar<128){
      if(tmpOccurs[currentChar]==0){
        uniqCount++;
      }
      tmpOccurs[currentChar]+=1;
    }
    i++;
  }
  *occurs=(char*)malloc(sizeof(char)*uniqCount);
  *occursCount=(int*)malloc(sizeof(int)*uniqCount);
  *maps=(char*)malloc(sizeof(char)*128);
  for(j=0,i=0;i<128;i++){
    if(tmpOccurs[i]>0){
      (*maps)[i]=j;
      (*occurs)[j]=i;
      (*occursCount)[j]=tmpOccurs[i];
      j++;continue;
    }
    (*maps)[i]=-1;
  }
  return uniqCount;
}

PEncodedString encodeString(char* str){
  PEncodedString pEncodedString;
  PHtTree pHtTree;
  PBitBuffer pBitBuffer;
  int i,j,uniqCount,strLength;
  char *occurs,*maps,*paths,*pathsLength;
  int *occursCount;

  strLength=strLen(str);
  uniqCount=analyseString(str,&occurs,&occursCount,&maps);
  pHtTree=buildHtTree(occursCount,uniqCount);

#ifdef DBG
  printHtTree(pHtTree);
#endif


  buildHtTreePathInChar(pHtTree,&paths,&pathsLength);
  pBitBuffer=buildBitBuffer(getHtTreeWPL(pHtTree,pathsLength));

  for(i=0;i<strLength;i++){
    j=maps[str[i]];
    appendBitBufferByChar(pBitBuffer,paths[j],pathsLength[j]);
  }

  pEncodedString = (PEncodedString)malloc(sizeof(EncodedString));
  pEncodedString->chars=occurs;
  pEncodedString->paths=paths;
  pEncodedString->data=pBitBuffer->buffer;
  pEncodedString->totalCount=strLength;
  pEncodedString->uniqCount=uniqCount;
  pEncodedString->dataSize=pBitBuffer->byteSize;

#ifdef DBG
  printf("***************Separator**************\n");
  for(i=0,j=0;i<uniqCount;i++){
    printf(" %c %.3d | ",occurs[i],paths[i]);
    if(++j%3==0){
      printf("\n");
    }
  }
  printf("***************Separator**************\n");
  printBitBuffer(pBitBuffer,4,6);
  printf("%s\n",str);
#endif

  free(maps);free(occursCount);
  free(pHtTree->nodes);free(pHtTree);
  free(pBitBuffer);

  return pEncodedString;
}

char* decodeString(PEncodedString p){
  int i,j,count=0,cursor=0,max=p->dataSize*8;
  char currentBit,matched,currentChar=0,offset=0;
  char* str=(char*)malloc(p->uniqCount*sizeof(char));
  while(cursor<max && count<p->totalCount){
    currentBit=((char*)p->data)[cursor/BITS_OF_CHAR];
    currentBit=(currentBit&(1<<(cursor%BITS_OF_CHAR)))!=0;
    currentChar|=(currentBit<<offset);
    for(i=0,j=-1;i<p->uniqCount;i++){
      matched=!(char)((p->paths[i]^currentChar)<<(BITS_OF_CHAR-(offset+1)));
      if(matched){
        if(j==-1){
          j=i;
        }else{
          j=-1;
          break;
        }
      }
    }
    if(j!=-1){
      str[count++]=p->chars[j];
      currentChar=0;
      offset=0;
    }else{
      offset++;
    }
    cursor++;
  }
  return str;
}

