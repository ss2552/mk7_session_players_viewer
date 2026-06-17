#include "utils.h"

void     Flash(bool isTop, u8 r, u8 g, u8 b)
{
    u32 *addr;

    if(isTop){
        addr = 0x10202204;
    }else{
        addr = 0x10202208;
    }

    u8 color = 0x01000000 | (b << 16) | (g << 8) | r;

    for (u32 i = 0; i < 64; i++)
    {
        REG32(addr) = color;
        svcSleepThread(5000000);
    }

    REG32(addr) = 0;

}