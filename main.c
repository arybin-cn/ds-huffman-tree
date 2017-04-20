#include <stdio.h>

#include "bitbuffer.h"
#include "httree.h"
#include "htencoder.h"

#define DBG

int main(){

  PEncodedString pEncodedString;
  char a[]="Hello World!!!";
  pEncodedString=encodeString(a);
  decodeString(pEncodedString);

}

