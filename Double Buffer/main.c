/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
// SDK Included Files
#include "board.h"
#include "fsl_lptmr_driver.h"
#include "fsl_debug_console.h"
#include <stdint.h>
#include "adc.h"
#include "dma.h"

int16_t dma_buffer[128];
int buffer_no=0;

#define first_half 0
#define second_half 64

void process_info(int);

/*typedef struct
{
float input;
float log_output;
}logval;

logval lookup[]={ {0.05,-1.30}, {0.1,-1}, {0.15,-0.82}, {0.2,-0.70}, {0.25, -0.60},
						{0.3,-0.52}, {0.35,-0.46}, {0.4,-0.4}, {0.45,-0.34}, {0.50,-0.3},
						{0.55,-0.26}, {0.60,-0.22}, {0.65,-0.19}, {0.7,-0.15}, {0.75,-0.12},
						{0.8,-0.10}, {0.85,-0.07}, {0.9,-0.05}, {0.95, -0.02}, {1,0}};
*/





void main()
{

	//int16_t adc_value_read;

	hardware_init();
	PRINTF("\n\rWelcome to command line");
	adc_init();
	dma_init();
	//SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK ; /* PORT clock enablement */    LED0_EN; // (PORTA_PCR16 = PORT_PCR_MUX(1))  GPIOA_PDDR |= (1<<16);  LED0_ON;

	PORTE_PCR22 |= PORT_PCR_MUX(1);
	GPIOE_PDDR  |= 1<<22;

	ADC0_SC1A &= ~(ADC_SC1_ADCH_MASK); //adc start
	//dma_start();
	//ADC0_SC1A = ADC_SC1_ADCH(0);     //starting adc
//	PRINTF("\n\n\r%x",dma_buffer);
//	while(1)
//	{
//	adc_value_read = adc_read();
//	PRINTF("\n\r%d",adc_value_read);
//	}
	dma_start();

	while(1)
	{
		int buffer_no_now = buffer_no;
		if(buffer_no_now==1)
		{
			process_info(first_half);
		}
		else
		{
			process_info(second_half);
		}
		while(buffer_no_now==buffer_no);

	}
}

void process_info(int region)
{
	int i,peak=0;
		int integer_log_return1;
		float log_return,log_return1;
		static int peak_prev,flag=0;

		peak=(dma_buffer[region]);
		for(i=region;i<region+64;i++)
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

}


