/**
  ******************************************************************************
  * @file    System / stm32f30x_it.c
  * @author  P14345
  * @version V1.0.0
  * @date    10/9/14
  * @brief   Program body
  ******************************************************************************
  */ 

/*----------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/


	#include "main.h"
	#include "math.h"
	
	



/** @addtogroup STM32F3_Discovery_SD_Projects
  * @{
  */

/** @addtogroup System
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

	RCC_ClocksTypeDef RCC_Clocks;


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* RCC Clock Variables */

__IO uint16_t IC2Value = 0;
__IO uint16_t DutyCycle = 0;
__IO uint32_t Frequency = 0;


/* TIMER values */

uint16_t capture = 0;
extern __IO uint16_t CCR1_TIM3_Val;
extern __IO uint16_t CCR1_TIM2_Val;


/* Necessary system global variables */

extern	double		threshold;
extern	double 		attack;
extern	double 		release;
extern	double 		ratio;

extern	int 			Compress;
extern	int				Att;
extern	int				Rel;

/* Timer2 Variables */
				
				uint16_t	attackStep = 1;
				uint16_t	releaseStep = 1;
				int 			attackCounter = 1;
				int				releaseCounter = 1;
				
				
/* Timer3 Variables */

__IO 	uint16_t  	ADC1ConvertedValue = 0;

				double		SampledSignal = 0;
				double 		SampledSignalVoltage;
				uint32_t	SSint;
				uint32_t	SSint2;
				double		decimalVal;
				int 			i;
				int  			base2SSdB;
				double		SSdB = 0;
				double		SSdB1 = 0;
				double		SSdB2 = 0;
				double 		OutputSignal;
				double 		GainReduction;
				double 		OutputVoltage;
				uint16_t 	OutputData;
				
				uint32_t x = 0x80800000;
				int y = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/




/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void NMI_Handler(void)
{
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void SVC_Handler(void)
{
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void DebugMon_Handler(void)
{
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void PendSV_Handler(void)
{
}






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void SysTick_Handler(void)
{
}






/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
/******************************************************************************/






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
	/*----------------------------------------------------------------------------*/
	
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	
		
		//update Step values
	
		attackStep = attack * 10;
		releaseStep = release * 10000;
	
		
		//Stop and clear Timer and counters if done
		if( attackCounter - 1 >= attackStep || releaseCounter - 1 >= releaseStep )
		{
			
			TIM_Cmd(TIM3, DISABLE);
			
			//If attack Timer done, Full Compression Enabled
			if( attackCounter - 1 >= attackStep )
			{
				Att = 0;
			}
			
			//disable compression if Release Timer done, Copression disabled
			if( releaseCounter - 1 >= releaseStep )
			{
				Compress = 0;
				Rel = 0;
			}
			
			attackCounter = 1;
			releaseCounter = 1;
			
		}
		
	
		//If timer enabled while not in Compress, Timer in Attack mode
		if( Att == 1 )
		{
			if( SSdB >= threshold )
			{
				attackCounter++;
			}
			if( SSdB < threshold && attackCounter > 0 )
			{
				attackCounter--;
			}
		}

		
		//else the Timer is in Release mode
		if( Rel == 1 )
		{
			if( SSdB <= threshold )
			{
				releaseCounter++;
			}
			if( SSdB > threshold && releaseCounter > 0 )
			{
				releaseCounter--;
			}
		}
		
		
		//If counters go to 0, disable Timer
		if( attackCounter < 1 && Att == 1 )
		{
			TIM_Cmd(TIM3, DISABLE);
			Compress = 0;
			Att = 0;
			
			attackCounter = 1;
			
		}
		
		if( releaseCounter < 1 && Rel == 1 )
		{
			TIM_Cmd(TIM3, DISABLE);
			Rel = 0;
			
			releaseCounter = 1;
		}

		
		capture = TIM_GetCapture1(TIM3);
		TIM_SetCompare1(TIM3, capture + CCR1_TIM3_Val);

	}
} //End Timer2 Interrupt






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    

		STM_EVAL_LEDToggle(LED6);
		
	
			
		/* Read ADC Every Time */
		if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET)
		{
			/* Get ADC1 converted data */
			SampledSignal = ADC_GetConversionValue(ADC1);
		}
			
		/* Compute the voltage */
		
		/*
		SampledSignalVoltage = (SampledSignal * 3000 )/0xFFF;
		
		SampledSignalVoltage = SampledSignalVoltage / 1000;
			
		SSint = SampledSignalVoltage;
		*/
		
		SSint2 = SampledSignal * 48;
		
		/* Convert voltage to 16q16 int */
		
		/*
		if( SampledSignalVoltage < 1 )
		{
			SSint = 0 ;
			decimalVal = SampledSignalVoltage;
			
		}
		
		if( SampledSignalVoltage >= 1 && SampledSignalVoltage < 2 )
		{
			SSint = 0 + 0x00010000;
			
			decimalVal = SampledSignalVoltage - 1;
		}

		if( SampledSignalVoltage >= 2 && SampledSignalVoltage < 3 )
		{
			SSint = 0 + 0x00020000;
			
			decimalVal = SampledSignalVoltage - 2;
		}
		
		if( SampledSignalVoltage >= 3 )
		{
			SSint = 0 + 0x00030000;
			
			decimalVal = SampledSignalVoltage - 3;
		}
		
		//Solve decimal val
		i = 32768;
		
		while( i >= 1 )
		{
			
			decimalVal = decimalVal*2;
			
			if( decimalVal > 1 )
			{
				
				SSint = SSint + i;
				decimalVal = decimalVal - 1 ;
			}
			
			if( decimalVal == 1 || decimalVal == 0 )
			{
				break;
			}
			
			i = i / 2;
		}
		*/
			
		/* Convert to dB */
		
		//SSdB1 = 20*log10( SampledSignalVoltage );
		
		
			base2SSdB = log2( SSint2 );
		
		
		//Decode SSdB2
		
		SSdB2 =   base2SSdB;
		SSdB =  SSdB2 / 22293082	;			// SSdB * 20 * log10(2) / 2^27
		
		if( SSint2 == 0 )
		{
			SSdB = -90;
		}
		
		//SSdB = SSdB1;
		//SSdB = SSdB2;
		//SSdB = -10;
		
		/* if signal above threshold, and compression not running, enable compression */
		if( SSdB >= threshold && Compress == 0 )
		{
			TIM_Cmd(TIM3, ENABLE );
			Compress = 1;
			Att = 1;
		}
		
		 /*if signal below threshold, and compression running, enable Release Timer */
		if( SSdB <= threshold && Compress > 0 && Rel == 0 )
		{
			TIM_Cmd(TIM3, ENABLE );
			Rel = 1 ;
		}
		
		
		
		/* Output 0 if not Compressing */
		if( Compress == 0 )
		{
			DAC_SetChannel1Data(DAC_Align_12b_R, 0x000);
		}
		
		
		/* Determine output if Compressing */
		if( Compress > 0 )
		{
			/* Compute Output Voltage*/
			OutputSignal = threshold + (( SSdB - threshold ) / ratio );
		
			GainReduction = OutputSignal - threshold;
			
			OutputData = 183.183 * GainReduction;
			
			/* 
						ADD CODE FOR LINEAR TIMER STEP HERE
			*/
		
			if( Att == 1 )
			{
				
				OutputData = OutputData * attackCounter / attackStep ;
																	
				//							2V			* 51						/ 150
				
			}
			
			if( Rel == 1 )
			{
				
				OutputData = OutputData * releaseCounter / releaseStep ;
				
				//					2V					*	2000					/ 6000
				
			}
			
			
			/*Convert and output to DAC */
			
			///OutputData = OutputVoltage;
			
			
			//OutputData = (OutputData*0xFFF)/3000;
			
			DAC_SetChannel1Data(DAC_Align_12b_R, OutputData);
		}
	
		
		STM_EVAL_LEDToggle(LED8);
		
    capture = TIM_GetCapture1(TIM2);
    TIM_SetCompare1(TIM2, capture + CCR1_TIM2_Val);
		
  }
} //End Timer3 Interrupt






/*----------------------------------------------------------------------------*/
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*----------------------------------------------------------------------------*/

/*void PPP_IRQHandler(void)
{
}*/






/**
  * @}
  */ 

/**
  * @}
  */ 


/*******************************    END OF FILE    ****************************/
