#include <stdio.h>
#include <malloc.h>

typedef struct{
  void* buffer;
  int cursor;
  int size;
} BitBuffer,*PBitBuffer;

typedef struct{
  int weight;
  int parent,left,right;
} HtNode;

typedef struct{
  int nodeCount;
  int root;
  HtNode* nodes;
} HtTree, *PHtTree;

//*************************HtTree Start*************************

PHtTree buildHuff(int *rawData, int length){
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
  pHtTree->nodeCount=length*2-1;
  pHtTree->root=pHtTree->nodeCount-1;
  return pHtTree;
}

void printHtTree(HtTree* pHtTree){
  int i;
  HtNode node;
  printf(" index | weight | parent | left | right\n");
  for(i=0;i<pHtTree->nodeCount;i++){
    node=pHtTree->nodes[i];
    printf("%6d |%7d |%7d |%5d |%6d\n",i,node.weight,node.parent,node.left,node.right);
  }
}

//*************************HtTree End*************************

//*************************BitBuffer Start*************************

PBitBuffer buildBitBuffer(int bits){
  int size;
  PBitBuffer pBitBuffer;
  void *buffer;
  if(bits > 0){
    pBitBuffer=(PBitBuffer)malloc(sizeof(BitBuffer));
    buffer=malloc(bits/8+(bits%8==0));
    if(pBitBuffer && buffer){
      pBitBuffer->size=bits;
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
  for(length=8-1;!(charData&(1<<length))&&length>-1;length--);
  return length+1;
}
int appendBitBufferByChar(PBitBuffer pBitBuffer,char charData){
  int bitLengthData;
  int bitLengthCurrent;
  char* current;
  bitLengthData=bitLengthOfChar(charData);
  if(pBitBuffer->cursor+bitLengthData>pBitBuffer->size){
    printf("Insufficient buffer..\n");
    return 0;
  }
  current=((char*)pBitBuffer->buffer)+pBitBuffer->cursor/8;
  bitLengthCurrent=bitLengthOfChar(*current);
  (*current)|=(charData<<bitLengthCurrent);
  (*(current+1))|=(charData>>(8-bitLengthCurrent));
  pBitBuffer->cursor+=bitLengthData;
  return bitLengthData;
}

void printBuffer(void* buffer,int length,int bitsPerGroup,int groupsPerLine){
  int i=0,j=0,k=0,l=0;
  char* current;
  for(;i<length;i++){
    current=((char*)buffer)+i/8;
    printf("%d",(int)(((*current)>>j)&(15<<j)!=0));
    if((++k%bitsPerGroup)==0){
      k=0;printf(" ");
      if((++l%groupsPerLine)==0){
        l=0;printf("\n");
      }
    }
    j++;j%=8;
  }
  printf("\n");
}

void printBitBuffer(PBitBuffer pBitBuffer,int bitsPerGroup,int groupsPerLine){
  printBuffer(pBitBuffer->buffer,pBitBuffer->size,bitsPerGroup,groupsPerLine);
}

//*************************BitBuffer END*************************

int main(){
  PBitBuffer pBitBuffer = buildBitBuffer(160);
  appendBitBufferByChar(pBitBuffer,1);
  appendBitBufferByChar(pBitBuffer,11);
  appendBitBufferByChar(pBitBuffer,12);
  char shit=123;
  printBuffer(&shit,8,4,2);
  //printBitBuffer(pBitBuffer,4,8);
  //int a=5;
  //int i;
  //int data[a];
  //for(i=0;i<a;i++){
  //  data[i]=(i+1)*(i+1);
  //}
  //HtTree* pHtTree =buildHuff(data,a);
  //printHtTree(pHtTree);
}





