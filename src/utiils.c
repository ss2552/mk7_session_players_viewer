#include "utils.h"

void     Flash(bool isTop, u32 color)
{
    u32 *addr;

    if(isTop){
        addr = 0x10202204;
    }else{
        addr = 0x10202208;
    }

    color |= 0x01000000;
    for (u32 i = 0; i < 64; i++)
    {
        REG32(addr) = color;
        svcSleepThread(5000000);
    }

    REG32(addr) = 0;

}