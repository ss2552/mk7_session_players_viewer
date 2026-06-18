#include "utils.h"
#include "types.h"

void Flash(bool isTop, u8 r, u8 g, u8 b){
    volatile u32 *addr;

    if(isTop){
        addr = (volatile u32 *)0x10202204;
    }else{
        addr = (volatile u32 *)0x10202208;
    }

    *addr |= 1 << 31;

    u8 color = 0x01000000 | (b << 16) | (g << 8) | r;

    for (u32 i = 0; i < 64; i++){
        *addr = color;
        svcSleepThread(5000000);
    }

    *addr = 0;

}
