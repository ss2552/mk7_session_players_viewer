#include <3ds.h>
#include "utils.h"
#include "types.h"

void Flash(bool isTop, u8 r, u8 g, u8 b){
    volatile u32 *addr;

    if(isTop){
        addr = (volatile u32 *)0x10202204;
    }else{
        addr = (volatile u32 *)0x10202208;
    }

    u32 color = 0x01000000U | ((u32)b << 16) | ((u32)g << 8) | (u32)r;
    u32 value = (1U << 31) | color;

    for (u32 i = 0; i < 64; i++){
        *addr = value;
        svcSleepThread(5000000);
    }

    *addr = 0;

}
