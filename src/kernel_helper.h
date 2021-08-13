#include <mach/mach.h>
#include <stdio.h>
#include <stdlib.h>

mach_port_t get_kernel_task_port();
uint64_t get_kaslr_slide(mach_port_t kernel_task);
uint64_t do_kernel_read(mach_port_t kernel_task, uint64_t addr);
char *do_kernel_read_string(mach_port_t kernel_task, uint64_t addr);
