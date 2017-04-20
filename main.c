#include <stdio.h>

#include "bitbuffer.h"
#include "httree.h"
#include "htencoder.h"

int main(){

  PEncodedString pEncodedString;
  char a[]="Hello World!!!";
  //readTextFile("article.txt",&a);

  pEncodedString=encodeString(a);
  decodeString(pEncodedString);

}

