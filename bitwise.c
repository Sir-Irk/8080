#include "types.h"
#include <stdio.h>

void print_bits(u8 v)
{
    printf("%d, %u, %x : ", (i8)v, v, v);
    for (int i = 7; i >= 0; i--) {
        printf("%d", (v & (1 << i)) != 0);
    }
    puts("");
}

int main(void)
{
#if 0
    u8 a = ((u8)128);
    print_bits(a);
    u8 prev = a & 0x80;
    print_bits(prev);
    a >>= 1;
    print_bits(a);
    u8 r = a | prev;
    print_bits(r);
#endif
    // RRC (A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0)
    u8 a = ((u8)128);
    print_bits(a);
    u8 prev = a & 1;
    print_bits(prev);
    a >>= 1;
    print_bits(a);
    u8 r = a | (prev << 7);
    print_bits(r);

    return 0;
}