#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
 const char* text = "0x61626364";
 printf("%lx\n", strtol(text, NULL, 16));
 printf("%lu", strtol(text, NULL, 16));
}
