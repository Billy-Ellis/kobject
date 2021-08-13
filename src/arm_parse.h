#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

uint64_t pull_address_from_arm_instructions(uint64_t instructions, uint64_t base_addr, uint64_t slide);
