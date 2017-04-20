#define BITS_OF_CHAR 8
#define OUT

typedef struct{
  int weight;
  int parent,left,right;
} HtNode;

typedef struct{
  int leafCount;
  int root;
  HtNode* nodes;
} HtTree, *PHtTree;

PHtTree buildHtTree(int *rawData, int length);
void buildHtTreePathInChar(HtTree* pHtTree,OUT char** paths,OUT char** pathsLength);
int getHtTreeWPL(PHtTree pHtTree,char *pathsLength);
void printHtTree(HtTree* pHtTree);
