#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include "menu.h"
#include "csvc.h"
#include "utils.h"
#include "plgldr.h"

Handle       g_ThreadHandle, g_continueGameEvent, g_monitor_ThreadHandle;
u8 mainstack[0x1000] ALIGN(8), monitorstack[0xFF] ALIGN(8);

Result      res;

void init_libs(){
   	irrstInit();
}

PluginMenu   menu;

bool LOCK = false;

void MonitorDeamon_Thread(){

    s32 event;

    while(true){

        Flash(false ,0x00, 0,0xFF);

        res = svcWaitSynchronization(memLayoutChanged, 10000000ULL);
        if(res == 0x09401BFE){

            LOCK = false;

            event = PLGLDR__FetchEvent();
            switch(event){
                case PLG_SLEEP_ENTRY:
                case PLG_SLEEP_EXIT:
                case PLG_ABOUT_TO_SWAP:
                    PLGLDR__Reply(event);
                    continue;
                case PLG_ABOUT_TO_EXIT:
                    break;
            }
        }else{

            LOCK = true;

        }
    }
}

inline void main(){

    init_menu();

    u32 inputkey = 0;

    while(aptMainLoop()){

        svcSleepThread(10000000ULL);

        irrstWaitForEvent(true);

        if(LOCK){
            continue;
        }

        Flash(false ,0x00, 0xFF, 0x00);

        irrstScanInput();
        inputkey = irrstKeyshold();
        if(inputkey & KEY_ZL || inputkey & KEY_ZR){
            PLGLDR__DisplayMenu(&menu);
        }
    }
}

void deinit_libs(){
    irrstExit();
}

void mainThread(){

    // __sync_init();
    // __system_initSyscalls();

    init_libs();

    svcControlProcess(CUR_PROCESS_HANDLE, PROCESSOP_GET_ON_MEMORY_CHANGE_EVENT, (u32)&memLayoutChanged, 0);

    svcSignalEvent(g_continueGameEvent);

    memset(&menu, 0, sizeof(menu));

    Flash(true, 0xFF, 0x00, 0xFF);

    main();

    deinit_libs();

    svcExitThread();

}

void __entrypoint(int arg, void* temporaryStack){

    (void)arg;
    (void)temporaryStack;

    srvInit();
    plgLdrInit();

    svcCreateEvent(&g_continueGameEvent, RESET_ONESHOT);
    svcCreateThread(&g_mainThreadHandle, mainThread, arg, (u32 *)(mainstack + 0x1000), 0x1A, 0);
    svcCreateThread(&g_monitor_ThreadHandle, MonitorDeamon_Thread, 0x00, (u32 *)(monitorstack + 0x1000), 0x1B, 0);
    svcWaitSynchronization(g_continueGameEvent, U64_MAX);
    svcCloseHandle(&g_continueGameEvent);
    svcCloseHandle(&g_monitor_ThreadHandle);
    svcCloseHandle(&g_mainThreadHandle);

    srvExit();
    plgLdrExit();

}
