#include <3ds.h>
#include "csvc.h"
#include "plgldr.h"

void init_libs(){
	srvInit();
    plgLdrInit();
}

void main(){
	
}

void deinit_libs(){
	
}


extern char* fake_heap_start;
extern char* fake_heap_end;
extern u32 __ctru_heap;
extern u32 __ctru_linear_heap;

u32 __ctru_heap_size        = 0;
u32 __ctru_linear_heap_size = 0;

inline void __system_allocateHeaps(PluginHeader *header){
    __ctru_heap_size = header->heapSize;
    __ctru_heap = header->heapVA;

    fake_heap_start = (char *)__ctru_heap;
    fake_heap_end = fake_heap_start + __ctru_heap_size;
}

int __entrypoint(int arg, void* temporaryStack){
	
	PluginHeader *header = (PluginHeader *)0x07000000;

    if (header->magic != HeaderMagic)
        return;

    __system_allocateHeaps(header);
	
    init_libs();

    svcControlProcess(CUR_PROCESS_HANDLE, PROCESSOP_GET_ON_MEMORY_CHANGE_EVENT, (u32)&memLayoutChanged, 0);
    svcCreateThread(&thread, main, 0, (u32 *)(stack + STACK_SIZE), 30, -1);

	deinit_libs();
	
}
