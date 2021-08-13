#include "kernel_helper.h"

mach_port_t get_kernel_task_port(){
    mach_port_t kernel_task;
    kern_return_t kr;
    if ((kr = task_for_pid(mach_task_self(), 0, &kernel_task)) != KERN_SUCCESS){
        if ((kr = host_get_special_port(mach_host_self(), HOST_LOCAL_NODE, 4, &kernel_task)) != KERN_SUCCESS){
          return -1;
        }
    }
    return kernel_task;
}

uint64_t get_kaslr_slide(mach_port_t kernel_task){
    //
	// This requires checkra1n
    // -> https://github.com/checkra1n/BugTracker/issues/1070
    //
	task_t tfp0;
    kern_return_t ret = task_for_pid(mach_task_self(), 0, &tfp0);
    if (ret != KERN_SUCCESS) return 0;

    task_dyld_info_data_t info;
    uint32_t count = TASK_DYLD_INFO_COUNT;
    
    ret = task_info(tfp0, TASK_DYLD_INFO, (task_info_t)&info, &count);
    if (ret != KERN_SUCCESS) return 0;
    
    return info.all_image_info_size;
}

uint64_t do_kernel_read(mach_port_t kernel_task, uint64_t addr){
	// try to check if this is a real address, to prevent panics
	if (((addr & 0xffffff0000000000) != 0xffffff0000000000) || (addr == -1)) {
		return -1;
	}	

    size_t size = 8;
    uint64_t data = 0;

    kern_return_t kr = vm_read_overwrite(kernel_task,
                                         (vm_address_t)addr,
                                         size,
                                         (vm_address_t)&data,
                                         &size);
    if (kr != KERN_SUCCESS){
        return -1;
    }
    
    return data;
}

char *do_kernel_read_string(mach_port_t kernel_task, uint64_t addr) {
	char *str = (char*)malloc(256);
    
    int end = 0;
    int counter = 0;
    while (!end) {
        uint64_t chars = do_kernel_read(kernel_task, addr + counter);
        
        for (int i = counter; i < (counter+8); i++) {
            char c = (chars >> (i*8)) & 0xff;
            if (c == 0x00) {
                str[i] = 0x00;
                return str;
            }
            str[i] = c;
        }
        
        counter += 8;
        if (counter >= 256) end = 1;
    }
    
    return str;
}
