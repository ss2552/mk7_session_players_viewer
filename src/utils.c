#include <3ds.h>
#include <stdio.h>
#include "utils.h"
#include "types.h"
#include "plgldr.h"

#define PA_PTR(addr) (void *)((u32)(addr) | 1 << 31)
#define REG32(addr) (*(vu32 *)(PA_PTR(addr)))

void Flash(bool isTop, u8 r, u8 g, u8 b){

    u8 p = isTop ? 0x4 : 0x8;

    u32 color = ((u32)b << 16) | ((u32)g << 8) | (u32)r;

    color |= 0x01000000;

    for (u32 i = 0; i < 64; i++){
        REG32(0x10202200 | p) = color;
        svcSleepThread(5000000);
    }

    REG32(0x10202200 | p) = 0;

/*
    // char title[32];
    // char body[32];
    // snprintf(title, sizeof(title), "%p", (void*)addr);
    // snprintf(body, sizeof(body), "%08x", color);

    // PLGLDR__DisplayMessage(title, body);

    for (u32 i = 0; i < 64; i++){
        *addr = color;
        svcSleepThread(5000000);
    }

    *addr = 0;

*/

}
