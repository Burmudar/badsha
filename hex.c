#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
 const char* text = "abcd";
 int shift = 24;
 int answ = 0;
 for(int i = 0; i < strlen(text); i++) {
    answ |= text[i] << shift;
    shift -= 8;
 }
 printf("answ=%i\n", answ);
}
