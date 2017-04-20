#include <stdio.h>
#include <malloc.h>

#include "strencoder.h"
#define OUT

int readTextFile(char* fileName,OUT char** str);

int main(){
  char *a;
  PEncodedString pEncodedString;
  readTextFile("article.txt",&a);
  pEncodedString=encodeString(a);
  decodeString(pEncodedString);
  return 0;
}

int readTextFile(char* fileName,OUT char** str){
  FILE *file;unsigned long size;
  if ((file=fopen(fileName,"rb"))!=NULL) {          
    fseek(file,0,SEEK_END);                         
    size=ftell(file);                               
    (*str)=(char*)malloc(size);
    fseek(file,0,SEEK_SET);
    return fread(*str,1,size,file)==size ? size : 0;
  }                                                 
  return 0;
}
