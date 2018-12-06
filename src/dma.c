/*
 * dma.c
 *
 *  Created on: Dec 5, 2018
 *      Author: nachi
 */
#include "dma.h"

extern int16_t dma_buffer[64];

void dma_init()
{
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;					// Clock to DMA mux
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;					// Clock to DMA module
	DMAMUX0_CHCFG0 = 0x00;								// Disable DMA
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;				// Clearing previous errors
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;				// Ensuring the clear
	DMA_SAR0 = ADC0_BASE;								// Source Address
	DMA_DAR0 = &dma_buffer;								// Destination address
	DMA_DCR0 |= DMA_DCR_EINT_MASK | DMA_DCR_DINC_MASK;	// Enable the DMA interrupts
	DMA_DCR0 |= DMA_DCR_DSIZE(2);						// 16-bit destination

	DMAMUX0_CHCFG0 |= 0x28;								// Select source as ADC0
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK;			// Enable DMA MUX
}

void dma_start()
{
	DMA_DCR0 |= DMA_DCR_START_MASK;						// Start the DMA transfer
}
