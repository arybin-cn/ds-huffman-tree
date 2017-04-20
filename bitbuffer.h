#define OUT

typedef struct{
  void* buffer;
  int cursor;
  int byteSize;
} BitBuffer,*PBitBuffer;

PBitBuffer buildBitBuffer(int bits);
int appendBitBufferByChar(PBitBuffer pBitBuffer,char charData,int bitLengthData);
void printBuffer(void* buffer,int bitSize,int bitsPerGroup,int groupsPerLine);
void printBitBuffer(PBitBuffer pBitBuffer,int bitsPerGroup,int groupsPerLine);
