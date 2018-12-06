/*
 * adc.c
 *
 *  Created on: Dec 5, 2018
 *      Author: nachi
 */
#include "adc.h"

void adc_init()
{
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;							// Clock Gate to Port E
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;							// ADC0 clk enable

	ADC0_CFG1 |= ADC_CFG1_ADICLK(2) | ADC_CFG1_MODE_MASK | ADC_CFG1_ADIV_MASK;		// Bus/2 clock selected, 16-bit, clock/8(3MHz)

	ADC0_SC1A |= ADC_SC1_ADCH_MASK;
	ADC0_SC1A &= ~(ADC_SC1_AIEN_MASK);							// Interrupts disable,
	ADC0_SC1A |= ADC_SC1_DIFF_MASK;			// Differential mode, Disable ADC

	ADC0_SC3 |= ADC_SC3_ADCO_MASK | ADC_SC3_AVGE_MASK;			// Continuous Conversion, Hardware avg

}

uint32_t adc_read()
{
	//while(!ADC_SC1_COCO_MASK);

	return ADC0_RA;
}
