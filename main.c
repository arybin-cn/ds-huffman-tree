#include "strencoder.h"
int main(){
  char *a="Hello World!!";
  PEncodedString pEncodedString;
//  readStringFromFile("article.txt",&a);
  pEncodedString=encodeString(a);
  decodeString(pEncodedString);
  releaseEncodedString(pEncodedString);
  return 0;
}
