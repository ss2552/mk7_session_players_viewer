#include <3ds.h>
#include <stdio.h>
#include "utils.h"
#include "types.h"
#include "plgldr.h"

void Flash(bool isTop, u8 r, u8 g, u8 b){

    volatile u32 *addr = isTop ? (volatile u32 *)0x10202204 : (volatile u32 *)0x10202208;

    u32 color = (1U << 31) |  0x01000000U | ((u32)b << 16) | ((u32)g << 8) | (u32)r;

    char title[32];
    char body[32];
    // snprintf(title, sizeof(title), "%p", (void*)addr);
    // snprintf(body, sizeof(body), "%08x", color);

    // PLGLDR__DisplayMessage(title, body);

    for (u32 i = 0; i < 64; i++){
        *addr = color;
        svcSleepThread(5000000);
    }

    *addr = 0;

}
