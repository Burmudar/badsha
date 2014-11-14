#!/usr/bin/python3

BLOCK_SIZE = 512
WORD_SIZE = 32

def zero_count(msg_size):
    modulus = BLOCK_SIZE - 64
    diff = (modulus - msg_size) % BLOCK_SIZE
    return diff

def bin_size(num):
    if isinstance(num, str) and num.startswith('0b'):
        return len(num) - 2
    else:
        return len(bin(num)) - 2

def pad_till(m, s):
    size = bin_size(m)
    pad_count = s - (size % s)
    return pad_count

def pad_to_size(num, size):
    return num.rjust(size, '0')

def pad_8_bit(num):
    return pad_to_size(num,8)

def pad_64_bit(num):
    return pad_to_size(num,64)

def to_binary_msg(msg):
    binary = '';
    for i in msg:
        binary = binary + bin(ord(i))[2:]
    return binary

def pad(msg):
    binary_msg = to_binary_msg(msg)
    pad_msg = binary_msg + '1'
    zeros = zero_count(len(pad_msg))
    pad_msg = pad_msg + (zeros * '0')
    pad_msg = pad_msg + pad_64_bit(bin(len(binary_msg))[2:])
    return pad_msg


def badd(n1, n2):
    x = int('0b' + n1, 2)
    y = int('0b' + n2, 2)
    z = (x + y) % 2**WORD_SIZE
    return pad_to_size(bin(z)[2:], WORD_SIZE)

def binary_32(num):
    return pad_to_size(bin(num)[2:], 32)

def rotary_r_shift(num, shift):
    return num[shift:] + num[0:shift]

def xor(a,b):
    diff = lambda a,b : '0' if a == b else '1'
    c = [diff(a[i],b[i]) for i in range(len(a))]
    return ''.join(c)

def shift_right(num, shift):
    return num[:len(num) - shift].rjust(32,'0')

def shift_left(num, shift):
    return num[shift:] + (shift * '0')

def SSIG0(num):
    a = rotary_r_shift(num,7)
    b = rotary_r_shift(num,18)
    c = shift_right(num, 3)
    return xor(xor(a,b),c)

def SSIG1(num):
    a = rotary_r_shift(num,17)
    b = rotary_r_shift(num,19)
    c = shift_right(num, 10)
    return xor(xor(a,b),c)

def word_list_16_64(words):
    for t in range(16,64):
        t1 = badd(SSIG1(words[t-2]), words[t-7])
        t2 = badd(SSIG0(t - 15), words(t-16))
        words.append(badd(t1,t2))

def inititial_word_list(bin_msg):
    return [bin_msg[i*WORD_SIZE:WORD_SIZE*(i + 1)] for i in range(16)]

def hash(N):
    H = [[
        binary_32(int('6a09e667',16)),
        binary_32(int('bb67ae85',16)),
        binary_32(int('3c6ef372',16)),
        binary_32(int('a54ff53a',16)),
        binary_32(int('510e527f',16)),
        binary_32(int('9b05688c',16)),
        binary_32(int('1f83d9ab',16)),
        binary_32(int('5be0cd19',16)),
    ]
    ]
    for i in range(N):
        W = inititial_word_list(N[i])
        word_list_16_64(W)
        a = H[i-1][0]
        b = H[i-1][1]
        c = H[i-1][2]
        d = H[i-1][3]
        e = H[i-1][4]
        f = H[i-1][5]
        g = H[i-1][6]
        h = H[i-1][7]

        for t in range(64):
            T1 = T1Calc()



def main():
    bin_msg = pad("William")
    count = 1
    word = '';
    for i in bin_msg:
        if count % 8 == 0:
            word = word
            ordVal = int('0b' + word, 2)
            c = chr(ordVal)
            print("{} {} {}".format(word, ordVal, c))
            word = ''
        else:
            word = word + i
        count = count+1
    #extract 16 32 bit words
    inititial_word_list(bin_msg)


if __name__=="__main__":
    main()
