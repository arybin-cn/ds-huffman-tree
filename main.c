#include <stdio.h>
#include <malloc.h>

#define BITS_OF_CHAR 8
#define OUT

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
    byteSize=bits/BITS_OF_CHAR+(bits%BITS_OF_CHAR==0);
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

//Make sure that one char eight bits!
int bitLengthOfChar(char charData){
  int length;
  for(length=BITS_OF_CHAR-1;!(charData&(1<<length))&&length>-1;length--);
  return length+1;
}
int appendBitBufferByChar(PBitBuffer pBitBuffer,char charData){
  int bitLengthData;
  int bitLengthCurrent;
  int maxBits=pBitBuffer->byteSize*BITS_OF_CHAR;
  char* current;
  bitLengthData=bitLengthOfChar(charData);
  if(pBitBuffer->cursor+bitLengthData>maxBits){
    printf("Insufficient buffer..\n");
    return 0;
  }
  current=((char*)pBitBuffer->buffer)+pBitBuffer->cursor/BITS_OF_CHAR;
  bitLengthCurrent=bitLengthOfChar(*current);
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
    printf("%d",(int)(((*current)>>j)&(15<<j)!=0));
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

int strLen(char* str){
  int i=0;
  while(str[i]!='\0') i++;
  return i;
}

int analyseString(char* str,OUT char** occurs,OUT int **occursCount,OUT char** maps){
  //assume that str is made up with 128 printable ascii chars
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

int main(){
  PHtTree pHtTree;
  int i,uniqCount;
  unsigned long strLength,bitLength;
  char *occurs,*maps,*paths,*pathsLength;
  int *occursCount;
  char str[]="FFF!Hello";

  uniqCount=analyseString(str,&occurs,&occursCount,&maps);
  for(i=0;i<uniqCount;i++){
    printf("%c:%d\n",occurs[i],occursCount[i]);
  }

  strLength=strLen(str);
  printf("Raw string length(bytes/bits): %lu/%lu\n",
      strLength,strLength*sizeof(char)*BITS_OF_CHAR);

  pHtTree=buildHtTree(occursCount,uniqCount);
  buildHtTreePathInChar(pHtTree,&paths,&pathsLength);

  bitLength=getHtTreeWPL(pHtTree,pathsLength);
  printf("Encoded string length(bytes/bits): %lu/%lu\n",
      bitLength/BITS_OF_CHAR+(bitLength%BITS_OF_CHAR!=0),bitLength);

  printf("\n");


  //PBitBuffer pBitBuffer = buildBitBuffer(160);
  //appendBitBufferByChar(pBitBuffer,1);
  //appendBitBufferByChar(pBitBuffer,11);
  //appendBitBufferByChar(pBitBuffer,12);
  //char shit=123;
  //printBuffer(&shit,8,4,2);
  //printBitBuffer(pBitBuffer,4,8);
  // int byteSize=5;
  // int i;
  // int data[byteSize];
  // for(i=0;i<byteSize;i++){
  //   data[i]=(i+1)*(i+1);
  // }
  // HtTree* pHtTree =buildHtTree(data,byteSize);
  // printHtTree(pHtTree);
  // printf("%d\n",getHtTreeWPL(pHtTree));
  // char* paths;
  // char* pathsLength;
  // buildHtTreePathInChar(pHtTree,&paths,&pathsLength);
  // for(i=0;i<1;i++){
  //   printf("%d:%d\n",paths[i],pathsLength[i]);
  // }
}

