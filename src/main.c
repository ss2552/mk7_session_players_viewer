#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include "csvc.h"
#include "plgldr.h"

static Handle       g_ThreadHandle;
static Handle       g_continueGameEvent;
static u8           stack[STACK_SIZE] ALIGN(8);

void init_libs(){
	irrstInit();
}

static PluginMenu   menu;

inline s32 Thread(){
        
        if(svcWaitSynchronization(memLayoutChanged, 10000000ULL) == 0x09401BFE){
            s32 event = PLGLDR__FetchEvent();
            switch(event){
                case PLG_SLEEP_ENTRY:
                case PLG_SLEEP_EXIT:
                case PLG_ABOUT_TO_SWAP:
                    PLGLDR__Reply(event);
                case PLG_ABOUT_TO_EXIT:
                    break;
            }
    }
}

inline void main(){

    u32 inputkey = 0;

    irrstScanInput();
    inputkey = irrstKeyshold();
    if(inputkey & KEY_ZL || inputkey & KEY_ZR){
        PLGLDR__DisplayMenu(&menu);
    }

    // irrstWaitForEvent(true);

}

void deinit_libs(){
    irrstExit();
}

void mainThread(){

    // __sync_init();
    // __system_initSyscalls();

    init_libs();

    svcSignalEvent(g_continueGameEvent);

    memset(&menu, 0, sizeof(menu));

    // svcControlProcess(CUR_PROCESS_HANDLE, PROCESSOP_GET_ON_MEMORY_CHANGE_EVENT, (u32)&memLayoutChanged, 0);

    main();

	   deinit_libs();

    svcExitThread();

}

int __entrypoint(int arg, void* temporaryStack){
	
    srvInit();
    plgLdrInit();

    svcCreateEvent(&g_continueGameEvent, RESET_ONESHOT);
    svcCreateThread(&g_ThreadHandle, mainThread, 0, (u32 *)(monitorStack + STACK_SIZE), 30, -1);
    svcWaitSynchronization(g_continueGameEvent, U64_MAX);
	svcCloseHandle(g_continueGameEvent);

    srvExit();
    plgLdrExit();

    return 0;

}
