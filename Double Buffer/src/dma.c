/*
 * dma.c
 *
 *  Created on: Dec 5, 2018
 *      Author: nachi
 */
#include "dma.h"


extern int16_t dma_buffer[128];
extern int buffer_no;
uint32_t dma_address, dma_address1;

uint32_t a=10;
//extern int16_t source_buffer[64];

void dma_init()
{
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;					// Clock to DMA mux
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;					// Clock to DMA module
	//SIM->SOPT2 |= 0x4000000u;

	DMAMUX0_CHCFG0 = 0x00;								// Disable DMA

	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;				// Clearing previous errors
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;				// Ensuring the clear

	DMA_SAR0 = 0x4003B010;//ADC0_BASE;								// Source Address

	DMA_DSR_BCR0 |=DMA_DSR_BCR_BCR(128);					// No of bytes to be transferred = (4 * 64)= 256 bytes

	DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK); //clear source size and destination size fields

	//DMA_DCR0 |= DMA_DCR_EINT_MASK 							//DMA_DCR_DINC_MASK |;//| DMA_DCR_SINC_MASK; // DMA_DCR_EINT_MASK | 	// Enable the DMA interrupts, increment DMA destination pointer after every transfer
	//DMA_DCR0 |= DMA_DCR_DSIZE(2);							// 16-bit destination
	//DMA_DCR0 |= DMA_DCR_DMOD(5);//| DMA_DCR_SMOD(5); 		//256 Bytes circular buffer modulo size for source, 128 bytes modulo buffer size for destination.
	DMA_DCR0 |= (DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) | DMA_DCR_D_REQ_MASK  | DMA_DCR_DINC_MASK | DMA_DCR_CS_MASK  | DMA_DCR_ERQ_MASK  | DMA_DCR_EADREQ_MASK  | DMA_DCR_EINT_MASK);

	dma_address= (uint32_t)&dma_buffer;
	dma_address1= (uint32_t)&dma_buffer[64];
	DMA_DAR0 = dma_address;								// Destination address
	//DMA_DCR0 |= DMA_DCR_DMOD(5);

	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_SOURCE(40);			// Select source as ADC0
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK;			// Enable DMA MUX

	__enable_irq();

	NVIC_EnableIRQ(DMA0_IRQn);



}

void dma_start()
{
	DMA_DCR0 |= DMA_DCR_ERQ_MASK ;//| DMA_DCR_START_MASK ;//DMA_DCR_ERQ_MASK;//DMA_DCR_START_MASK ;DMA_DCR_ERQ_MASK ;						// Start the DMA transfer, Enable dma through peripheral
}

float getlog(float input)
{
	float check1=1;
	int i=0;
	for(i=0;i<20;i++)
	{
		if(input<=lookup[i].max_val && input>=lookup[i].min_val)
		{
			check1=lookup[i].log_output;
		}
	}
return check1;
}





	/*if(input>=0 && input <=1638.4)
		equi_val=0.05;
	else if(input>1638.4 && input <=3276.8)
		equi_val=1;
	else if(input>3276.8 && input <=4915.2)
			equi_val=1;
	else if(input>4915.2 && input <=6553.6)
			equi_val=1;
	else if(input>6553.6 && input <=8192)
			equi_val=1;
	else if(input>8192 && input <=9830.4)
			equi_val=1;
	else if(input>9830.4 && input <=11468.8)
			equi_val=1;
	else if(input>11468.8 && input <=13107.2)
			equi_val=1;
	else if(input>13107.2 && input <=14745.6)
			equi_val=1;
	else if(input>14745.6 && input <=16384)
			equi_val=1;
	else if(input>16384 && input <=18022.4)
			equi_val=1;
	else if(input>18022.4 && input <=19660.8)
			equi_val=1;
	else if(input>1638.4 && input <=3276.8)
			equi_val=1;
	else if(input>1638.4 && input <=3276.8)
			equi_val=1;*/




void DMA0_IRQHandler(void)
{

//	int peak=0;
//	int integer_log_return1;
//
//	float log_return,log_return1;
//	static int peak_prev,flag=0;

	//PRINTF("\n\rentered interrupt");
	GPIOE_PSOR |= (1<<22);
	if(DMA_DSR_BCR0 & DMA_DSR_BCR_CE_MASK)
	{
		PRINTF("\n\rinterrupt due to configuration error");
	}


	if(DMA_DSR_BCR0 & DMA_DSR_BCR_BES_MASK)
	{
		PRINTF("\n\rinterrupt due to bus error on source");
	}
	if(DMA_DSR_BCR0 & DMA_DSR_BCR_BED_MASK)
	{
		PRINTF("\n\rinterrupt due to bus error in destination");
	}

	/*
	peak=(dma_buffer[0]);
	for(i=0;i<64;i++)
	{
		PRINTF("\n\r%d",dma_buffer[i]);
		if(abs(dma_buffer[i])>abs(peak))
		{
			peak=dma_buffer[i];

		}

	}
	//PRINTF("\n\rpeak before peak_Val is%d",peak);

	if(flag==0)
	{
		flag=1;
		peak_prev=peak;
	}

	else if(abs(peak)<abs(peak_prev) && flag==1)
	{
		float alpha;
		if(abs(peak_prev-peak)<=1000)
		alpha=1;

		else if(abs(peak_prev-peak)<=2000 && abs(peak_prev-peak)>1000)
		alpha=0.97;

		else if(abs(peak_prev-peak)<=4000 && abs(peak_prev-peak)>2000)
		alpha=0.94;

		else if(abs(peak_prev-peak)<=10000 && abs(peak_prev-peak)>4000)
		alpha=0.85;

		else
		alpha=0.7;

		peak=alpha * peak_prev;

	}
	peak_prev=peak;

	log_return=(float)getlog(abs(peak_prev));
	log_return1=20*log_return;
	integer_log_return1=log_return1;
	PRINTF("\n\r Peak value in DBFS is");
	PRINTF("%d.",integer_log_return1);
	log_return1= ((log_return1-integer_log_return1)*(-1));

	for(int i=0;i<4;i++)
	{
		if(i!=0)
		log_return1-= integer_log_return1;
		log_return1*=10;
		integer_log_return1=log_return1;
		PRINTF("%d",integer_log_return1);
	}


	//PRINTF("\n\rThe peak value in DBFS is %1.2f", log_return1);
	PRINTF("\n\rThe Peak Value is %d ",peak_prev);
	*/


	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;				                    // Clearing previous errors
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;				                    // Clearing previous errors

	GPIOE_PCOR |= (1<<22);
	if(buffer_no==1)
	{
	DMA_DAR0 = dma_address;
	buffer_no=0;
	}
	else
	{

	DMA_DAR0= dma_address1;
	buffer_no=1;
	}
	DMA_DSR_BCR0 |=DMA_DSR_BCR_BCR(128);
	dma_start();
}
