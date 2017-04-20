#define OUT

typedef struct{
  int uniqCount;
  int totalCount;
  int dataSize;
  void *data;
  char *chars;
  char *paths;
} EncodedString,*PEncodedString;

int strLen(char* str);
int analyseString(char* str,OUT char** occurs,OUT int **occursCount,OUT char** maps);
PEncodedString encodeString(char* str);
char* decodeString(PEncodedString p);
