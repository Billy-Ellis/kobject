#include "arm_parse.h"

uint64_t reverse_endian(uint64_t x) {
    x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
    x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
    x = (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;
    return x;
}

uint64_t pull_address_from_arm_instructions(uint64_t instructions,
                                            uint64_t base_addr,
                                            uint64_t slide) {
    // first, reverse the bytes
	instructions = reverse_endian(instructions);
    
	// instruction 1
	// (the page address)
	uint32_t ins1 = (instructions >> 32) & 0xffffffff;

	// the address info is the first 2 bytes of this instruction
	uint32_t addr = (ins1 >> 16) & 0xffff;

	// but we then need to swap the bytes over
	uint8_t temp = (addr >> 8) & 0xff;
	addr = ((addr << 8) | temp) & 0xffff;
    
	// instruction 2
	// (the page offset)
	uint32_t ins2 = instructions & 0xffffffff;

	// we need the two middle bytes, and to swap them
	uint32_t page_offset = (((ins2 << 8) & 0xff0000) >> 8) | ((ins2 >> 16) & 0xff);

    // construct the full address
	base_addr &= 0xfffffffffffff000;
	base_addr -= slide;

    // option value is last byte
    uint32_t option = (ins1 & 0xff);
    
	switch (option) {
	case 0xf0:
		base_addr += 0x3000;
		break;
    case 0xd0:
        base_addr += 0x2000;
        break;
	case 0xb0:
		base_addr += 0x1000;
		break;
	default:
        // do nothing
		break;
	}

    // addr needs to be shifted by 9 to get the real value
    return ((addr << 9) + base_addr) + (page_offset >> 2);
}
