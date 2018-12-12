#include <stdint.h>
#include "board.h"

void adc_init();
uint32_t adc_read();
int adc_cal(void);
//unsigned short adc_read(unsigned char ch);
