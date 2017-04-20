#include <stdio.h>
#include "strencoder.h"

int main(){
  char a[]="Hello World";
  PEncodedString pEncodedString;
//readTextFile("article.txt",&a);
  pEncodedString=encodeString(a);
  decodeString(pEncodedString);
}

