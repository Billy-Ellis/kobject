// Created by Billy Ellis @bellis1000
// ZygoSec

#include <stdint.h>
#include <mach/mach_types.h>
#include <mach/vm_map.h>
#include <mach/mach_host.h>
#include <mach/mach.h>
#include <mach/message.h>
#include <mach/vm_region.h>
#include <mach/vm_statistics.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "kernel_helper.h"
#include "arm_parse.h"

#define WHITE "\x1b[37m"
#define CYAN "\x1b[36m"
#define MAGENTA "\x1b[35m"
#define PURPLE "\x1b[34m"
#define GREEN "\x1b[32m"
#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"

void failed() {
	printf("No kernel object at this address.\n");
	exit(0);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <kaddr>\n", argv[0]);
		return -1;
	}
    
    uint64_t kaddr = (uint64_t)strtoull(argv[1], NULL, 16);
    
	mach_port_t kernel_port = get_kernel_task_port();
	if (!kernel_port) {
		printf("Could not get kernel task port.\n");
		return -1;
	}

	uint64_t kslide = get_kaslr_slide(kernel_port);
	
    // First pull the vtable ptr (offset +0x0 in C++ objects)
	uint64_t vtable_addr = do_kernel_read(kernel_port, kaddr);
	if (vtable_addr == -1 || vtable_addr == 0x0 || (vtable_addr & 0xffffff0000000000) != 0xffffff0000000000) failed();
	
    // vtable method `getMetaClass` is at +0x38 in the vtable
    uint64_t getMetaClass_func_ptr = do_kernel_read(kernel_port, vtable_addr + 0x38);
    
    // pull the first 2 instructions from this function
    //
    // example:
    //      adrp       x0,0xFFFFFFF0089CB000    ; get the page addr
    //      add        x0,x0,#0xEF0             ; get the page offset
    //      ...
    uint64_t arm_instructions = do_kernel_read(kernel_port, getMetaClass_func_ptr);
    
    // calculate the metaClass addr based on the instructions
    uint64_t meta_class = pull_address_from_arm_instructions(arm_instructions, getMetaClass_func_ptr, kslide) + kslide;
    
	uint64_t sub_meta_class = do_kernel_read(kernel_port, meta_class + 0x18);
	if ((sub_meta_class & 0xffffff0000000000) != 0xffffff0000000000) failed();

	uint64_t name_ptr = do_kernel_read(kernel_port, sub_meta_class + 0x10);
	printf(BOLD CYAN"%s\n"RESET, do_kernel_read_string(kernel_port, name_ptr));
			
	return 0;
}
