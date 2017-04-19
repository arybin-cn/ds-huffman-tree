#include <stdio.h>
#include <malloc.h>

#define BITS_OF_CHAR 8
#define OUT
#define DBG


typedef struct{
  int uniqCount;
  int totalCount;
  int dataSize;
  void *data;
  char *chars;
  char *paths;
} EncodedString,*PEncodedString;

typedef struct{
  void* buffer;
  int cursor;
  int byteSize;
} BitBuffer,*PBitBuffer;

typedef struct{
  int weight;
  int parent,left,right;
} HtNode;

typedef struct{
  int leafCount;
  int root;
  HtNode* nodes;
} HtTree, *PHtTree;

// Utils

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

//*************************HtTree Start*************************

PHtTree buildHtTree(int *rawData, int length){
  int i,j,minIndex,secMinIndex;
  PHtTree pHtTree=(PHtTree)malloc(sizeof(HtTree));
  HtNode* nodes=(HtNode*)malloc(sizeof(HtNode)*(2*length-1));
  if(length<2 || !pHtTree || !nodes){
    printf("Length<2 or fail to alloc memory..\n");
    free(pHtTree);free(nodes);
    return NULL;
  }
  for(i=0;i<length;i++){
    nodes[i].weight=rawData[i];
    nodes[i].parent=-1;nodes[i].left=-1;nodes[i].right=-1;
  }
  for(i=0;i<length-1;i++){
    minIndex=-1;secMinIndex=-1;
    for(j=0;j<length+i;j++){
      if(nodes[j].parent==-1){
        if(minIndex==-1){
          minIndex=j;
          continue;
        }
        if(secMinIndex==-1){
          if(nodes[j].weight<nodes[minIndex].weight){
            secMinIndex=minIndex;
            minIndex=j;
            continue;
          }
          secMinIndex=j;
          continue;
        }
        if(nodes[j].weight<nodes[minIndex].weight){
          secMinIndex=minIndex;
          minIndex=j;
          continue;
        }
        if(nodes[j].weight<nodes[secMinIndex].weight){
          secMinIndex=j;
        }
      }
    }
    nodes[minIndex].parent=length+i;
    nodes[secMinIndex].parent=length+i;
    nodes[length+i].weight=nodes[minIndex].weight+nodes[secMinIndex].weight;
    nodes[length+i].parent=-1;
    nodes[length+i].left=minIndex;
    nodes[length+i].right=secMinIndex;
  }
  pHtTree->nodes=nodes;
  pHtTree->leafCount=length;
  pHtTree->root=pHtTree->leafCount*2-2;
  return pHtTree;
}

void buildHtTreePathInChar(HtTree* pHtTree,OUT char** paths,OUT char** pathsLength){
  int i,j,tmpLength;
  *paths=(char*)malloc(sizeof(char)*(pHtTree->leafCount));
  *pathsLength=(char*)malloc(sizeof(char)*(pHtTree->leafCount));
  for(i=0;i<pHtTree->leafCount;i++){
    tmpLength=0;j=i;
    while(pHtTree->nodes[j].parent!=-1){
      tmpLength+=1;
      (*paths)[i]<<=1;
      (*paths)[i]|=(pHtTree->nodes[pHtTree->nodes[j].parent].left==j);
      j=pHtTree->nodes[j].parent;
    }
    (*pathsLength)[i]=tmpLength;
  }
}

void printHtTree(HtTree* pHtTree){
  int i;
  HtNode node;
  printf(" index | weight | parent | left | right\n");
  for(i=0;i<pHtTree->leafCount*2-1;i++){
    node=pHtTree->nodes[i];
    printf("%6d |%7d |%7d |%5d |%6d\n",i,node.weight,node.parent,node.left,node.right);
  }
}

int getHtTreeWPL(PHtTree pHtTree,char *pathsLength){
  int i,wpl=0;
  for(i=0;i<pHtTree->leafCount;i++){
    wpl+=pHtTree->nodes[i].weight*pathsLength[i];
  }
  return wpl;
}

//*************************HtTree End*************************

//*************************BitBuffer Start*************************

PBitBuffer buildBitBuffer(int bits){
  int byteSize;
  PBitBuffer pBitBuffer;
  void *buffer;
  if(bits > 0){
    pBitBuffer=(PBitBuffer)malloc(sizeof(BitBuffer));
    byteSize=bits/BITS_OF_CHAR+(bits%BITS_OF_CHAR!=0);
    buffer=malloc(byteSize);
    if(pBitBuffer && buffer){
      pBitBuffer->byteSize=byteSize;
      pBitBuffer->buffer=buffer;
      pBitBuffer->cursor=0;
      return pBitBuffer;
    }
    free(pBitBuffer);
    free(buffer);
  }
  printf("Fail to build BitBuffer..");
  return NULL;
}

int appendBitBufferByChar(PBitBuffer pBitBuffer,char charData,int bitLengthData){
  int bitLengthCurrent;
  int maxBits=pBitBuffer->byteSize*BITS_OF_CHAR;
  char* current;
  if(pBitBuffer->cursor+bitLengthData>maxBits){
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

//*************************BitBuffer END*************************

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
  printf("*************** Separator **************\n");
  for(i=0,j=0;i<uniqCount;i++){
    printf(" %c %3d | ",occurs[i],occursCount[i]);
    if(++j%3==0){
      printf("\n");
    }
  }
  printf("\n");
  printf("*************** Separator **************\n");
  printBitBuffer(pBitBuffer,4,8);
  printf("%s\n",str);
#endif

  free(maps);free(occursCount);
  free(pHtTree->nodes);free(pHtTree);
  free(pBitBuffer);

  return pEncodedString;
}

char* decodeString(PEncodedString p){
  int i,j,count=0,cursor=0,max=p->dataSize*BITS_OF_CHAR;
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

int readTextFile(char* fileName,char** str){
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

int main(){


  PEncodedString pEncodedString;
  char *a="Hello World";
  //readTextFile("article.txt",&a);

  pEncodedString=encodeString(a);
  decodeString(pEncodedString);

}

