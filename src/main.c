#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include "csvc.h"
#include "plgldr.h"

static Handle       thread;
static Handle       memLayoutChanged;
static u8           stack[STACK_SIZE] ALIGN(8);

void init_libs(){
	irrstInit();
}

static PluginMenu   menu;

void main(){

    svcControlProcess(CUR_PROCESS_HANDLE, PROCESSOP_GET_ON_MEMORY_CHANGE_EVENT, (u32)&memLayoutChanged, 0);
    while(true){
        
        if(svcWaitSynchronization(memLayoutChanged, 10000000ULL) == 0x09401BFE){

            s32 event = PLGLDR__FetchEvent();

            if (event == PLG_SLEEP_ENTRY)
            {
                PLGLDR__Reply(event);
            }
            else if (event == PLG_SLEEP_EXIT)
            {
                PLGLDR__Reply(event);
            }
            else if (event == PLG_ABOUT_TO_SWAP)
            {
                PLGLDR__Reply(event);
            }
            else if (event == PLG_ABOUT_TO_EXIT)
            {
                PLGLDR__Reply(event);
            }
            if (HID_PAD & BUTTON_SELECT)
                PLGLDR__DisplayMenu(&menu);
            ApplyCheat();

            continue;
        }
    }

    memset(&menu, 0, sizeof(menu));

    init_libs();

    u32 inputkey = 0;

    while(aptMainLoop()){
        irrstWaitForEvent(true);
        irrstScanInput();
        inputkey = irrstKeyshold();
        if(inputkey & KEY_ZL || inputkey & KEY_ZR){
            PLGLDR__DisplayMenu(menu);
        }
    }

	deinit_libs();
}

void deinit_libs(){
	irrstExit();
}

int __entrypoint(int arg, void* temporaryStack){
	
	// PluginHeader *header = (PluginHeader *)0x07000000;

    // if(header->magic != HeaderMagic) return;
    // __system_allocateHeaps(header);
	
    srvInit();
    plgLdrInit();
    
    svcCreateEvent(&thread, RESET_ONSHOT);
    svcCreateThread(&thread, main, 0, (u32 *)(stack + STACK_SIZE), 30, -1);
    svcWaitSynchronization(thread, U64_MAX);
	svcCloseHandle(thread);

    srvExit();
    plgLdrExit();

    return 0;

}
