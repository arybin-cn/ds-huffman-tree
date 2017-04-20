#include <stdio.h>
#include <malloc.h>
#include "httree.h"

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
