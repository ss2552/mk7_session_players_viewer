#include <3ds.h>
#include "utils.h"
#include "types.h"
#include "plgldr.h"

void Flash(bool isTop, u8 r, u8 g, u8 b){

    volatile u32 *addr;

    if(isTop){
        addr = (volatile u32 *)0x10202204;
    }else{
        addr = (volatile u32 *)0x10202208;
    }

    u32 color = (1U << 31) |  0x01000000U | ((u32)b << 16) | ((u32)g << 8) | (u32)r;

    char title[10];
    sprintf(title, "%x¥0", addr);
    const char body[10];
    sprintf(body, "%x¥0", color);

    PLGLDR__DisplayMessage(&title, &body);

    /*
        
    for (u32 i = 0; i < 64; i++){
        *addr = color;
        svcSleepThread(5000000);
    }

    *addr = 0;
    
    */

}
