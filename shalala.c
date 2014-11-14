#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

const int BLOCK_SIZE_IN_BITS = 512;
const int WORD_SIZE_IN_BITS = 32;
const int TEXT_LENGTH_BIT_SIZE = 64;
const int BYTE_LENGTH = 8;
const int BLOCK_SIZE_IN_HEX = 512 / 8;
const int WORD_SIZE_IN_HEX = 32 / 8;
const int TEXT_LENGTH_HEX_SIZE = 64 / 8;

unsigned int K[] = {
0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2,
};

int determineHexZeros(const char* text) {
    int textLengthInBits = strlen(text) * BYTE_LENGTH;
    printf("Text Length in Bits: %i", textLengthInBits);
    int modulus = BLOCK_SIZE_IN_BITS - TEXT_LENGTH_BIT_SIZE;
    int zeros = (modulus - textLengthInBits - 1) % BLOCK_SIZE_IN_BITS;
    printf("Bit Zeros: %i\n", zeros);
    return zeros / BYTE_LENGTH;
}

void zeroOut(unsigned char* array, int startPos, int endPos) {
    for (int i = startPos; i < endPos; i++ ) {
        array[i] = 0x00;
    }
}

void addMsgLength(unsigned char* array, int textLength) {
    int positionIn64LengthBlock = textLength / 255;
    int lengthPos = (BLOCK_SIZE_IN_HEX - positionIn64LengthBlock) - 1;
    int lengthInBits = textLength * 8;
    array[lengthPos] = lengthInBits;
    return;
}

unsigned char* toBlock(char* text) {
    int textLength = strlen(text);
    printf("Length of Text: %i\n", textLength);
    unsigned char* hexArray = (unsigned char *)malloc(sizeof(unsigned char) * BLOCK_SIZE_IN_HEX);
    memcpy(hexArray, text, sizeof(unsigned char) * textLength);
    int zeroCount = determineHexZeros((const char *)hexArray);
    printf("Zeros = %i\n", zeroCount);
    hexArray[textLength] = 0x80;
    zeroOut(hexArray, textLength + 1, zeroCount);
    addMsgLength(hexArray, textLength);
    return hexArray;
}

unsigned long* toIntBlock(unsigned char* block, int size) {
    unsigned long* newBlock = (unsigned long *)malloc(sizeof(unsigned long) * (size / 4));
    unsigned char* hexVal = (unsigned char *)malloc(sizeof(unsigned char) * 4);
    for (int i = 0; i < (size / 4); i++){
        for (int j = 0; j < 4; j++) {
            int index = i * 4 + j;
            hexVal[j] = block[index];
            printf("%02x",hexVal[j]);
        }
        printf("\n");
        newBlock[i] = (unsigned long)strtoul((char*)hexVal, NULL, 16);
        printf("%lx",strtol((char*)hexVal, NULL, 16));
        printf("%s\n", strerror(errno));
    }
    return newBlock;
}

int CH(int* x, int* y, int* z) {
    int answ = (*x & *y) ^ (~*x & *z);
    return answ;
}

unsigned int MAJ(unsigned int* x, unsigned int* y, unsigned int* z) {
    unsigned int answ = (*x & *y) ^ (*x & *z) ^ (*y & *z);
    return answ;
}

unsigned int ROTL(unsigned int* x, short n, short bitAmount) {
    return (*x << n) | (*x>>(bitAmount - n));
}

unsigned int ROTR(unsigned int* x, short n, short bitAmount) {
    return (*x >> n) | (*x<<(bitAmount - n));
}

unsigned int BIGS0(unsigned int* x) {
    return ROTR(x,2,32) ^ ROTR(x,13,32) ^ ROTR(x,22,32);
}

unsigned int BIGS1(unsigned int* x) {
    return ROTR(x,6,32) ^ ROTR(x,12,32) ^ ROTR(x,25,32);
}

unsigned int SSIG0(unsigned int* x) {
    return ROTR(x,7,32) ^ ROTR(x,18,32) ^ (*x >> 3);
}

unsigned int SSIG1(unsigned int* x) {
    return ROTR(x,17,32) ^ ROTR(x,19,32) ^ (*x >> 10);
}

int main(void)
{
    unsigned char *a = toBlock("William Bezuidenhout");
    for (int i = 0; i < BLOCK_SIZE_IN_HEX; i++) {
        printf("%02x", a[i]);
        if ((i + 1) % 4 == 0) {
            printf(" ");
        }
        if ((i + 1) % WORD_SIZE_IN_HEX == 0) {
            printf("\n");
        }
    }
    printf("\n");
    unsigned long* intBlock = toIntBlock(a, BLOCK_SIZE_IN_HEX);
    for(int i = 0; i < 16; i++) {
        printf("%lx\n", intBlock[i]);
    }
    free(a);
    free(intBlock);
    return 0;
}
