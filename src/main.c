#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include "menu.h"
#include "csvc.h"
#include "types.h"
#include "utils.h"
#include "plgldr.h"

#define MAIN_THREAD_STACK_SIZE 0xF000
#define MONITOR_THREAD_STACK_SIZE 0x8000
Handle   g_ThreadHandle, g_continueGameEvent, g_monitor_ThreadHandle, memLayoutChanged;
u8 mainstack[MAIN_THREAD_STACK_SIZE] ALIGN(8),  monitorstack[MONITOR_THREAD_STACK_SIZE] ALIGN(8);

#define SEC(x) ((u64)(x) * 1000000000ULL)

s32     PLGLDR__FetchEvent(void);
void    PLGLDR__Reply(s32 event);

void init_libs(){
    // aptInit();
    irrstInit();
}

PluginMenu   menu;

volatile bool LOCK = false;
volatile bool is_monitorring = false;

void MonitorDeamon_Thread(void *arg){

    (void)arg;

    svcSleepThread(SEC(1));

    // volatile arg = *arg;

    // Flash(false ,0xFF, 0x00, 0x00);

    is_monitorring = true;

    s32 event;

    Result   res;

    while(true){
        res = svcWaitSynchronization(memLayoutChanged, 10000000ULL);
        if(res == 0x09401BFE){
            event = PLGLDR__FetchEvent();
            switch(event){
                case PLG_SLEEP_ENTRY:
                case PLG_SLEEP_EXIT:
                case PLG_ABOUT_TO_SWAP:
                    LOCK = true;
                    Flash(true ,0xFF, 0x00, 0x00);
                    PLGLDR__Reply(event);
                    continue;
                case PLG_ABOUT_TO_EXIT:;
                    goto e;
                default:
                    LOCK = false;
            }
        }else{
            LOCK = true;
        }
    }

e:
    is_monitorring = false;
    Flash(false ,0xFF, 0x00, 0x00);
    svcExitThread();

}

void __main(){

    // Flash(false ,0x00, 0xFF, 0x00);

    entry_menu(&menu);

    u32 inputkey = 0;

    while(is_monitorring){

        svcSleepThread(10000000ULL);

        if(LOCK){
            continue;
        }

        irrstScanInput();
        inputkey = irrstKeysHeld();
        if(inputkey & KEY_ZL || inputkey & KEY_ZR){
            PLGLDR__DisplayMenu(&menu);
        }
    }
}

void deinit_libs(){
    // aptExit();
    irrstExit();
}

void mainThread(void *arg){

    (void)arg;

    init_libs();

    memset(&menu, 0, sizeof(menu));
    
    svcCreateThread(&g_monitor_ThreadHandle, MonitorDeamon_Thread, 0x2BED, (u32 *)(monitorstack + MONITOR_THREAD_STACK_SIZE), 0x1A, 0);

    svcSignalEvent(g_continueGameEvent);

    // Flash(false ,0xFF, 0x00, 0xFF);

    svcSleepThread(SEC(0xF));

    __main();

    deinit_libs();

    Flash(false ,0xFF, 0x00, 0xFF);

    svcExitThread();

}

void __entrypoint(int arg, void* temporaryStack){

    (void)temporaryStack;

    srvInit();
    plgLdrInit();

    svcControlProcess(CUR_PROCESS_HANDLE, PROCESSOP_GET_ON_MEMORY_CHANGE_EVENT, (u32)&memLayoutChanged, 0);

    svcCreateEvent(&g_continueGameEvent, RESET_ONESHOT);
    svcCreateThread(&g_ThreadHandle, mainThread, 0x1BED, (u32 *)(mainstack + MAIN_THREAD_STACK_SIZE), 0x1A, 0);
    svcWaitSynchronization(g_continueGameEvent, U64_MAX);
    svcCloseHandle(g_continueGameEvent);

    srvExit();
    plgLdrExit();

}
