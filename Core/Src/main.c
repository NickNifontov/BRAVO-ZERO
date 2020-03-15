/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "comp.h"
#include "dac.h"
#include "dma.h"
#include "hrtim.h"
#include "iwdg.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define MAX_PULSE_END 47000
#define MIN_PULSE_BEGIN 3000

#define PULSE_IN_WAVE 461
#define DEATH_PULSE_IND_LEFT 3 // 4 pulse
#define DEATH_PULSE_IND_RIGHT 457 // 4 pulse

#define SMALL_CURRENT_LEFT 30
#define SMALL_CURRENT_RIGTH 431
#define SMALL_CURRENT 2500
#define NORMAL_CURRENT 3500

#define SOFTSTART_V_MAX_CNT 200

#define POLKA_ROLLBACK_MAX_CNT 4
#define POLKA_LEVEL_MAX 3500

struct BRAVO_PULSE {
   uint32_t CurrentDuty;
   uint8_t BlockedByV;
   uint8_t BlockedByI;
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t POLKA_MODE=0;
uint16_t POLKA_LEVEL=0;
uint16_t POLKA_ROLLBACK=POLKA_ROLLBACK_MAX_CNT;

struct BRAVO_PULSE PulseA;
struct BRAVO_PULSE PulseB;

uint8_t SoftStart_V=1;
uint16_t SoftStart_CNT=0;
float  SoftStart_Coeff=1;

uint16_t tmp_Pos=0;

uint8_t WAVE_TOP_BOTTOM=0;

uint8_t  BRAVO_FLAG_PEREK=0; //0 - NO PEREK , NO AC

uint16_t BRAVO_SINUS_IND=0;

uint16_t Curr_Sinus_Data=0;

uint16_t Sinus_Data[PULSE_IN_WAVE] ={
		0, 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 359, 383, 407, 431, 455,
		479, 502, 526, 550, 574, 597, 621, 645, 668, 692, 715, 739, 762, 786, 809, 832, 856, 879, 902,
		925, 948, 971, 994, 1017, 1040, 1063, 1086, 1109, 1132, 1154, 1177, 1200, 1222, 1245, 1267,
		1289, 1312, 1334, 1356, 1378, 1400, 1422, 1444, 1466, 1488, 1509, 1531, 1552, 1574, 1595, 1617,
		1638, 1659, 1680, 1701, 1722, 1743, 1764, 1785, 1805, 1826, 1846, 1866, 1887, 1907, 1927, 1947,
		1967, 1987, 2006, 2026, 2046, 2065, 2084, 2104, 2123, 2142, 2161, 2180, 2198, 2217, 2235, 2254,
		2272, 2290, 2308, 2326, 2344, 2362, 2380, 2397, 2415, 2432, 2449, 2466, 2483, 2500, 2517, 2534,
		2550, 2566, 2583, 2599, 2615, 2631, 2647, 2662, 2678, 2693, 2708, 2724, 2739, 2753, 2768, 2783,
		2797, 2812, 2826, 2840, 2854, 2868, 2881, 2895, 2908, 2922, 2935, 2948, 2961, 2974, 2986, 2999,
		3011, 3023, 3035, 3047, 3059, 3070, 3082, 3093, 3104, 3115, 3126, 3137, 3147, 3158, 3168, 3178,
		3188, 3198, 3208, 3217, 3227, 3236, 3245, 3254, 3263, 3271, 3280, 3288, 3296, 3304, 3312, 3320,
		3327, 3335, 3342, 3349, 3356, 3362, 3369, 3376, 3382, 3388, 3394, 3400, 3405, 3411, 3416, 3421,
		3426, 3431, 3436, 3440, 3444, 3449, 3453, 3457, 3460, 3464, 3467, 3470, 3473, 3476, 3479, 3481,
		3484, 3486, 3488, 3490, 3492, 3493, 3495, 3496, 3497, 3498, 3499, 3499, 3500, 3500, 3500, 3500,
		3500, 3499, 3499, 3498, 3497, 3496, 3495, 3493, 3492, 3490, 3488, 3486, 3484, 3481, 3479, 3476,
		3473, 3470, 3467, 3464, 3460, 3457, 3453, 3449, 3444, 3440, 3436, 3431, 3426, 3421, 3416, 3411,
		3405, 3400, 3394, 3388, 3382, 3376, 3369, 3362, 3356, 3349, 3342, 3335, 3327, 3320, 3312, 3304,
		3296, 3288, 3280, 3271, 3263, 3254, 3245, 3236, 3227, 3217, 3208, 3198, 3188, 3178, 3168, 3158,
		3147, 3137, 3126, 3115, 3104, 3093, 3082, 3070, 3059, 3047, 3035, 3023, 3011, 2999, 2986, 2974,
		2961, 2948, 2935, 2922, 2908, 2895, 2881, 2868, 2854, 2840, 2826, 2812, 2797, 2783, 2768, 2753,
		2739, 2724, 2708, 2693, 2678, 2662, 2647, 2631, 2615, 2599, 2583, 2566, 2550, 2534, 2517, 2500,
		2483, 2466, 2449, 2432, 2415, 2397, 2380, 2362, 2344, 2326, 2308, 2290, 2272, 2254, 2235, 2217,
		2198, 2180, 2161, 2142, 2123, 2104, 2084, 2065, 2046, 2026, 2006, 1987, 1967, 1947, 1927, 1907,
		1887, 1866, 1846, 1826, 1805, 1785, 1764, 1743, 1722, 1701, 1680, 1659, 1638, 1617, 1595, 1574,
		1552, 1531, 1509, 1488, 1466, 1444, 1422, 1400, 1378, 1356, 1334, 1312, 1289, 1267, 1245, 1222,
		1200, 1177, 1154, 1132, 1109, 1086, 1063, 1040, 1017, 994, 971, 948, 925, 902, 879, 856, 832,
		809, 786, 762, 739, 715, 692, 668, 645, 621, 597, 574, 550, 526, 502, 479, 455, 431, 407, 383,
		359, 336, 312, 288, 264, 240, 216, 192, 168, 144, 120, 96, 72, 48, 24, 0, 0
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Enable_SH_DEBUG(void) {
			//__HAL_RCC_DBGMCU_CLK_ENABLE();
		    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_WWDG_STOP;
		    DBGMCU->APB2FZ = 0xFFFFFFFF;
		    DBGMCU->APB1FZ = 0xFFFFFFFF;
		    DBGMCU->CR |=DBGMCU_CR_DBG_STOP;
		    __HAL_DBGMCU_FREEZE_IWDG();
}

void HAL_HRTIM_Fault1Callback(HRTIM_HandleTypeDef *hhrtim) {
	HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin|MCU_50HZ2_Pin, GPIO_PIN_RESET); // OFF 50Hz

	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TC1);
	HAL_HRTIM_WaveformCountStop_IT(&hhrtim1, HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_C
			  | HRTIM_TIMERID_TIMER_B | HRTIM_TIMERID_TIMER_D);

	__HAL_HRTIM_CLEAR_IT(&hhrtim1, HRTIM_IT_FLT1); // reset IT Flag

	HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_SET); // OFF LED

	BRAVO_FLAG_PEREK=1; // set PEREK FLAG, AC IS ON

}

void HAL_HRTIM_Capture2EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                              uint32_t TimerIdx)
{
		if (TimerIdx==HRTIM_TIMERINDEX_TIMER_D) {
			//tmp_Pos=HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].CPT1xR;
			POLKA_MODE=1;
			POLKA_LEVEL=Curr_Sinus_Data;
			POLKA_ROLLBACK=1000;
		}
}


void HAL_HRTIM_Capture1EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                              uint32_t TimerIdx)
{
		if (TimerIdx==HRTIM_TIMERINDEX_TIMER_D) {
			//tmp_Pos=HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].CPT1xR;
			POLKA_MODE=1;
			POLKA_LEVEL=Curr_Sinus_Data;
			POLKA_ROLLBACK=1000;
		}
}

/*void HAL_HRTIM_Capture2EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_TIMER_B) { // Trig B, END OF IN2 OUTPUT
			PulseA.CurrentDuty=2*(HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CPT2xR-25000);

			if (PulseA.CurrentDuty<MIN_PULSE_BEGIN) {
				//PulseB.CurrentDuty=MIN_PULSE_BEGIN;
				PulseA.CurrentDuty=MIN_PULSE_BEGIN;
			}

			HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].CMP1xR=PulseA.CurrentDuty;
		}

}

void HAL_HRTIM_Capture1EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_TIMER_B) { // Trig A, END OF IN1 OUTPUT
		PulseB.CurrentDuty=2*(HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CPT1xR);

		if (PulseB.CurrentDuty<MIN_PULSE_BEGIN) {
			PulseB.CurrentDuty=MIN_PULSE_BEGIN;
			PulseA.CurrentDuty=MIN_PULSE_BEGIN;
		}

		//__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_2,PulseB.CurrentDuty);
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].CMP2xR=PulseB.CurrentDuty;
	}

	if (TimerIdx==HRTIM_TIMERINDEX_TIMER_D) {
		tmp_Pos=HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].CPT1xR;
		if (tmp_Pos<25000) {
			PulseA.BlockedByV=1;
		} else {
			PulseB.BlockedByV=1;
		}
	}

	if ( (TimerIdx!=HRTIM_TIMERINDEX_TIMER_D) && (TimerIdx=!HRTIM_TIMERINDEX_TIMER_B)) {
			PulseA.BlockedByV=0;
			PulseB.BlockedByV=0;
	}

}*/

void  HAL_HRTIM_RepetitionEventCallback(HRTIM_HandleTypeDef * hhrtim,
                                              uint32_t TimerIdx) {
	if (TimerIdx==HRTIM_TIMERINDEX_MASTER) {
		if (BRAVO_SINUS_IND>=PULSE_IN_WAVE) {
			BRAVO_SINUS_IND=0;

			// in softstart
			if (SoftStart_V==1) {
				SoftStart_CNT++;
				if (SoftStart_CNT>SOFTSTART_V_MAX_CNT) {
					SoftStart_V=0;
					SoftStart_CNT=0;
					SoftStart_Coeff=1;
				} else {
					SoftStart_Coeff=(float)SoftStart_CNT/(float)SOFTSTART_V_MAX_CNT;
				}
			}

			// Check Polka Mode
			if (POLKA_MODE==1) {
				if (POLKA_ROLLBACK<=0) {
					POLKA_MODE=0;
					POLKA_ROLLBACK=POLKA_ROLLBACK_MAX_CNT;
					POLKA_LEVEL=POLKA_LEVEL_MAX;
				} else {
					POLKA_ROLLBACK--;
				}
			}

			if (WAVE_TOP_BOTTOM==0) {
				WAVE_TOP_BOTTOM=1;
			} else {
				WAVE_TOP_BOTTOM=0;
			}
		} else {
			if ((BRAVO_SINUS_IND<=DEATH_PULSE_IND_LEFT) || (BRAVO_SINUS_IND>=DEATH_PULSE_IND_RIGHT)) {
				PulseA.CurrentDuty=MIN_PULSE_BEGIN;
				PulseB.CurrentDuty=MIN_PULSE_BEGIN;
				HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR=PulseA.CurrentDuty;
				HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].CMP2xR=PulseB.CurrentDuty;
				HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin|MCU_50HZ2_Pin, GPIO_PIN_RESET); // OFF 50Hz

				HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // ZERO V VALUE
				HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // ZERO I VALUE

			} else {
				if (WAVE_TOP_BOTTOM==0) {
					HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin, GPIO_PIN_RESET); // OFF 50Hz_1
					HAL_GPIO_WritePin(GPIOA, MCU_50HZ2_Pin, GPIO_PIN_SET); // ON 50Hz_2
				} else {
					HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin, GPIO_PIN_SET); // ON 50Hz_1
					HAL_GPIO_WritePin(GPIOA, MCU_50HZ2_Pin, GPIO_PIN_RESET); // OFF 50Hz_2
				}

				Curr_Sinus_Data=Sinus_Data[BRAVO_SINUS_IND];
				if ((POLKA_MODE==1) && (Curr_Sinus_Data>=POLKA_LEVEL)) {
					Curr_Sinus_Data=POLKA_LEVEL;
				}

				if (SoftStart_V==1) {
					Curr_Sinus_Data=(float)SoftStart_Coeff*(float)Curr_Sinus_Data;
				}

				HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,Curr_Sinus_Data); //Current Value of Sinus

				if ((BRAVO_SINUS_IND<=SMALL_CURRENT_LEFT) || (BRAVO_SINUS_IND>=SMALL_CURRENT_RIGTH)) {
					HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,SMALL_CURRENT); //Current Value I for this Sinus
				} else {
					HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,NORMAL_CURRENT); //Current Value I for this Sinus
				}



				PulseA.CurrentDuty=MAX_PULSE_END;
				PulseB.CurrentDuty=MAX_PULSE_END;
				//PulseA.CurrentDuty=MIN_PULSE_BEGIN+Sinus_Data[BRAVO_SINUS_IND]*(44000/3500);
				//PulseB.CurrentDuty=MIN_PULSE_BEGIN+Sinus_Data[BRAVO_SINUS_IND]*(44000/3500);
				HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR=PulseA.CurrentDuty;
				HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].CMP2xR=PulseB.CurrentDuty;

				//PulseA.CurrentDuty=MIN_PULSE_BEGIN+Sinus_Data[BRAVO_SINUS_IND]*(44000/3500);
				//HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR=PulseA.CurrentDuty;
							/*PulseA.CurrentDuty+=1000;
							if (PulseA.CurrentDuty>MAX_PULSE_END) {
								PulseA.CurrentDuty=MIN_PULSE_BEGIN+1000;
							}
							//__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_2,PulseA.CurrentDuty);
							HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR=PulseA.CurrentDuty;
							*/
			}
		}

		BRAVO_SINUS_IND++;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	Enable_SH_DEBUG();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_IWDG_Init();
  MX_ADC1_Init();
  MX_COMP2_Init();
  MX_COMP4_Init();
  MX_DAC1_Init();
  MX_DAC2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_ADC2_Init();
  MX_HRTIM1_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_RESET); // ON LED

  HAL_GPIO_WritePin(GPIOB, VCC_15V_Pin|VCC_15B_Pin, GPIO_PIN_SET); // ON VCC 15V

  HAL_GPIO_WritePin(GPIOB, DRV_SD_EX_Pin, GPIO_PIN_SET); // EXT DRV UNBLOCK
  HAL_GPIO_WritePin(GPIOB, DRV_SD_IN_Pin, GPIO_PIN_RESET); // INT DRV UNBLOCK

  HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin|MCU_50HZ2_Pin, GPIO_PIN_RESET); // OFF 50Hz

  // DAC1 and COMP2
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // default V is ZERO

  //HAL_COMP_Start(&hcomp2);

  // DAC2 and COMP4
    HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // default I is ZERO

    //HAL_COMP_Start(&hcomp4);

  //

  PulseA.CurrentDuty=MAX_PULSE_END;
  PulseA.BlockedByI=0;
  PulseA.BlockedByV=0;

  PulseB.CurrentDuty=MAX_PULSE_END;
  PulseB.BlockedByI=0;
  PulseB.BlockedByV=0;

  BRAVO_SINUS_IND=0; // default step

  BRAVO_FLAG_PEREK=0; // default no AC

  WAVE_TOP_BOTTOM=0; // TOP WAVE

  //reset Softstart
  SoftStart_V=1;
  SoftStart_CNT=0;
  SoftStart_Coeff=0;

  // Polka Settings
  POLKA_MODE=1;
  POLKA_LEVEL=3000;
  POLKA_ROLLBACK=POLKA_ROLLBACK_MAX_CNT;

  HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_RESET); // ON LED

  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TC1);
  HAL_HRTIM_WaveformCountStart_IT(&hhrtim1, HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_C
		  | HRTIM_TIMERID_TIMER_B | HRTIM_TIMERID_TIMER_D);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	HAL_IWDG_Refresh(&hiwdg); // RESET IWDG
	//if ( (BRAVO_FLAG_PEREK==1) && (__HAL_HRTIM_GET_FLAG(&hhrtim1, HRTIM_FLAG_FLT1) == SET)) {
	if (BRAVO_FLAG_PEREK==1) {

		BRAVO_FLAG_PEREK=0; // reset FLAG
		__HAL_HRTIM_CLEAR_IT(&hhrtim1, HRTIM_IT_FLT1); // reset IT Flag

		HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_RESET); // ON LED

		  PulseA.CurrentDuty=MAX_PULSE_END;
		  PulseA.BlockedByI=0;
		  PulseA.BlockedByV=0;

		  PulseB.CurrentDuty=MAX_PULSE_END;
		  PulseB.BlockedByI=0;
		  PulseB.BlockedByV=0;

		  BRAVO_SINUS_IND=0; // default step

		  WAVE_TOP_BOTTOM=0; // TOP WAVE

		  HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin|MCU_50HZ2_Pin, GPIO_PIN_RESET); // OFF 50Hz

		  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // default V is ZERO

		  HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // default I is ZERO

		  //skip Softstart
		  SoftStart_V=0;
		  SoftStart_CNT=0;
		  SoftStart_Coeff=1;

		// try to start
				HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TC1);
				HAL_HRTIM_WaveformCountStart_IT(&hhrtim1, HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_C
						  | HRTIM_TIMERID_TIMER_B | HRTIM_TIMERID_TIMER_D);
	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_HRTIM1|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Hrtim1ClockSelection = RCC_HRTIM1CLK_PLLCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
