#include "VariableByteEncoding.h"

void __integer_encoding::VariableByte__Encode(uint32_t* source, uint8_t* target)
{
    if (*source < 0x00000080U) {
        goto jmp1;
    }
    if (*source < 0x00004000U) {
        goto jmp2;
    }
    if (*source < 0x00200000U) {
        goto jmp3;
    }
    if (*source < 0x10000000U) {
        goto jmp4;
    }

        *target++ = (*source & 0x7f) | 0x80;
        *source >>= 7;
    jmp4:
        *target++ = (*source & 0x7f) | 0x80;
        *source >>= 7;
    jmp3:
        *target++ = (*source & 0x7f) | 0x80;
        *source >>= 7;
    jmp2:
        *target++ = (*source & 0x7f) | 0x80;
        *source >>= 7;
    jmp1:
        *target = *source;
}

// Returns the position in the byte array after the last byte of the uint32_t we want to decode
int __integer_encoding::VariableByte__Decode(uint8_t* source, uint32_t* target)
{
    int nextPos;
    *target = 0x0;

    if ((*source & 0x80) == 0x80) {
        if ((*(source+1) & 0x80) == 0x80) {
            if ((*(source+2) & 0x80) == 0x80) {
                if ((*(source+3) & 0x80) == 0x80) {
                    source += 4;
                    nextPos = 5;
                    goto p5;
                }
                source += 3;
                nextPos = 4;
                goto p4;
            }
            source += 2;
            nextPos = 3;
            goto p3;
        }
        source += 1;
        nextPos = 2;
        goto p2;
    }
    nextPos = 1;
    goto p1;

    p5:
        *target += *source-- &  0x7f;*target<<=7;
    p4:
        *target+=*source--&0x7f;*target<<=7;
    p3:
        *target+=*source--&0x7f;*target<<=7;
    p2:
        *target+=*source--&0x7f;*target<<=7;
    p1:
        *target+=*source&0x7f;

    return nextPos;
}
