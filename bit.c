#include "bit.h"

uint32_t get_bits(uint64_t num, uint32_t start, uint32_t count){
	uint32_t mask = 0xffffffff;
	mask = mask >> (32 - count);
	return (num >> start) & mask;
}
int64_t sign_extend(uint64_t num, uint32_t start){
    uint32_t distance = 63 - start;
    int64_t imm;

    imm = num << distance;
    imm = imm >> distance;

    return imm;

}
bool get_bit(uint64_t num, uint32_t which){
	uint64_t res = (num >> which) & 0b1;
	return res;
}
