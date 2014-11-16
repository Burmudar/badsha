#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
const int BLOCK_SIZE_IN_BITS = 512;
const int WORD_SIZE_IN_BITS = 32;
const int TEXT_LENGTH_BIT_SIZE = 64;
const int BYTE_LENGTH = 8;
const int BLOCK_SIZE_IN_HEX = 512 / 8;
const int WORD_SIZE_IN_HEX = 32 / 8;
const int TEXT_LENGTH_HEX_SIZE = 64 / 8;

uint32_t K[] = {
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

void zeroOut(uint8_t* array, int startPos, int endPos) {
    for (int i = startPos; i < endPos; i++ ) {
        array[i] = 0x00;
    }
}

void addMsgLength(uint8_t* array, int textLength) {
    int positionIn64LengthBlock = textLength / 255;
    int lengthPos = (BLOCK_SIZE_IN_HEX - positionIn64LengthBlock) - 1;
    int lengthInBits = textLength * 8;
    array[lengthPos] = lengthInBits;
    return;
}

uint8_t* toBlock(char* text) {
    int textLength = strlen(text);
    printf("Length of Text: %i\n", textLength);
    uint8_t* hexArray = (uint8_t*)malloc(sizeof(uint8_t) * BLOCK_SIZE_IN_HEX);
    memcpy(hexArray, text, sizeof(uint8_t) * textLength);
    int zeroCount = determineHexZeros((const char *)hexArray);
    printf("Zeros = %i\n", zeroCount);
    hexArray[textLength] = 0x80;
    zeroOut(hexArray, textLength + 1, zeroCount);
    addMsgLength(hexArray, textLength);
    return hexArray;
}

uint32_t CH(uint32_t x, uint32_t y, uint32_t z) {
    uint32_t answ = (x & y) ^ (~x & z);
    return answ;
}

uint32_t MAJ(uint32_t x, uint32_t y, uint32_t z) {
    uint32_t answ = (x & y) ^ (x & z) ^ (y & z);
    return answ;
}

uint32_t ROTL(uint32_t x, short n, short bitAmount) {
    return (x << n) | (x>>(bitAmount - n));
}

uint32_t ROTR(uint32_t x, short n, short bitAmount) {
    return (x >> n) | (x<<(bitAmount - n));
}

uint32_t BIGS0(uint32_t x) {
    return ROTR(x,2,32) ^ ROTR(x,13,32) ^ ROTR(x,22,32);
}

uint32_t BIGS1(uint32_t x) {
    return ROTR(x,6,32) ^ ROTR(x,11,32) ^ ROTR(x,25,32);
}

uint32_t SSIG0(uint32_t x) {
    return ROTR(x,7,32) ^ ROTR(x,18,32) ^ (x >> 3);
}

uint32_t SSIG1(uint32_t x) {
    return ROTR(x,17,32) ^ ROTR(x,19,32) ^ (x >> 10);
}

uint32_t toInt(uint8_t *t) {
    return (t[0] << 24) | (t[1] << 16) | (t[2] << 8) | t[3];
}

uint8_t *toHex(uint32_t num) {
    printf("converting int: %i", num);
    uint8_t *hex = (uint8_t*)malloc(sizeof(uint8_t) * 4);
    hex[0] = num >> 24;
    hex[1] = (num & 0x00ff0000) >> 16;
    hex[2] = (num  & 0x0000ff00) >> 8;
    hex[3] = (num & 0x000000ff);
    printf("Hex should be: %02x",num);
    return hex;
}

uint8_t *process(uint8_t *block) {
    uint32_t H[] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };
    uint32_t *W = (uint32_t *) malloc (sizeof(uint32_t*) * 64);
    for (int t = 0; t < 16; t++) {
        W[t] = toInt(&block[t * 4]);
    }
    for (int t = 16; t < 64; t++) {
        W[t] = SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];
    }
    uint32_t a = H[0];
    uint32_t b = H[1];
    uint32_t c = H[2];
    uint32_t d = H[3];
    uint32_t e = H[4];
    uint32_t f = H[5];
    uint32_t g = H[6];
    uint32_t h = H[7];
    uint32_t T1 = 0;
    uint32_t T2 = 0;
    for(int t =0; t < 64 ; t++) {
        T1 = h + BIGS1(e) + CH(e,f,g) + K[t] + W[t];
        T2 = BIGS0(a) + MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }
    H[0] = a + H[0];
    H[1] = b + H[1];
    H[2] = c + H[2];
    H[3] = d + H[3];
    H[4] = e + H[4];
    H[5] = f + H[5];
    H[6] = g + H[6];
    H[7] = h + H[7];
    uint8_t *hash = (uint8_t*)malloc(sizeof(uint8_t) * 64);
    for(int i =0; i < 4; ++i){
        hash[i]      = (H[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4]  = (H[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8]  = (H[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (H[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (H[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (H[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (H[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (H[7] >> (24 - i * 8)) & 0x000000ff;
    }
    return hash;
}

void printBlock(uint8_t *block) {
    for (int i = 0; i < 64; i++) {
        printf("%02x", block[i]);
        if((i+1) % 4 == 0) {
            printf("\n");
        }
    }
}

void test(){
    uint32_t d = 0x87912990;
    uint32_t e = 0x07590dcd;
    uint32_t f = 0xb092f20c;
    uint32_t g = 0x745a48de;
    uint32_t h = 0x1e578218;
    uint32_t answ = h + BIGS1(e) + CH(e,f,g) + 0xe49b69c1  + 0x6362a380;
    printf("test: %02x", d + answ);
}

int main(void)
{
    uint8_t *block = toBlock("abc");
    uint8_t *hash = process(block);
    for (int j = 0; j < 32; j++ ) {
        printf("%02x", hash[j] );
    }
    free(block);
    free(hash);
    return 0;
}
