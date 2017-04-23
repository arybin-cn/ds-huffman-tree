#define OUT

typedef struct{
  int uniqCount;
  int totalCount;
  int dataSize;
  void *data;
  char *chars;
  short *paths;
} EncodedString,*PEncodedString;

int readStringFromFile(char* filename,OUT char** str);
int lengthOfString(char* str);
int analyseString(char* str,OUT char** occurs,OUT int **occursCount,OUT char** maps);

PEncodedString encodeString(char* str);
char* decodeString(PEncodedString p);
void releaseEncodedString(PEncodedString pEncodedString);
