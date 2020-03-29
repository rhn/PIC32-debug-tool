#ifndef TRANSPORT_BITBANG_GPIO_H_25c6cbd1928b4ea9beff56454a5c9ee3
#define TRANSPORT_BITBANG_GPIO_H_25c6cbd1928b4ea9beff56454a5c9ee3

// Header file for devices that do not have two SPI interfaces.
// In that case, we can still bit-bang everything,
// just timings will be more uneven and slower
#include <inttypes.h>

void transportSetup(uint32_t mode);
uint64_t transportSendJTAG(	uint32_t tms_header_nbits, uint32_t tms_header_val,
							uint32_t data_nbits, uint64_t data_val,
							uint32_t tms_epilogue_nbits, uint32_t tms_epilogue_val);
uint64_t transportSendICSP(	uint32_t tms_header_nbits, uint32_t tms_header_val,
							uint32_t data_nbits, uint64_t data_val,
							uint32_t tms_epilogue_nbits, uint32_t tms_epilogue_val);


#endif
