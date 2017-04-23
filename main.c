#include <stdio.h>
#include "strencoder.h"
int main(){
  char *str="Hello World!! Hello Huffman??";
  PEncodedString pEncodedString;
  readStringFromFile("article.txt",&str);
  pEncodedString=encodeString(str);
  printf("%s\n",decodeString(pEncodedString));
  releaseEncodedString(pEncodedString);
  return 0;
}
