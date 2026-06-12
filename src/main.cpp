#include <3ds.h>
#include "csvc.h"
#include "plgldr.h"
#define PA_PTR(addr)            (void *)((u32)(addr) | 1 << 31)
#define REG32(addr)             (*(vu32 *)(PA_PTR(addr)))

int main(){


	launchPlugin(mainStruct);

	exit:

	ndmuExit();
	nsExit();

	exitMainMenu();
	return (0);
}

int __entrypoint(int arg, void* temporaryStack)
{

	volatile PluginHeader* header = (volatile PluginHeader*)PA_FROM_VA_PTR(FwkSettings::Header);
	header->notifyHomeEvent = true;
	svcCreateEvent(&g_continueGameEvent, RESET_ONESHOT);
	svcCreateThread(&g_keepThreadHandle, KeepThreadMain, arg, (u32 *)&keepThreadStack[0x1000], 0x1A, 0);
	svcWaitSynchronization(g_continueGameEvent, U64_MAX);
	svcCloseHandle(g_continueGameEvent);

	return 0;
}
