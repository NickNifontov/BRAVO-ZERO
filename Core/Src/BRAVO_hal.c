/*
 * BRAVO_hal.c
 *
 *  Created on: Mar 21, 2020
 *      Author: Nick Nifontov
 */

#include "BRAVO_hal.h"

// *** POLKA *** //
volatile uint8_t  POLKA_MODE=0;
volatile uint16_t  POLKA_LEVEL=POLKA_LEVEL_MAX;
volatile uint16_t  POLKA_ROLLBACK=POLKA_ROLLBACK_MAX_CNT;

// *** SYSTEM *** //
void Enable_SH_DEBUG(void) {
			//__HAL_RCC_DBGMCU_CLK_ENABLE();
		    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_WWDG_STOP;
		    DBGMCU->APB2FZ = 0xFFFFFFFF;
		    DBGMCU->APB1FZ = 0xFFFFFFFF;
		    DBGMCU->CR |=DBGMCU_CR_DBG_STOP;
		    __HAL_DBGMCU_FREEZE_IWDG();
}

void HAL_HRTIM_Capture2EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                              uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_TIMER_B) { // Trig B, END OF IN2 OUTPUT
		A_DUTY_NORMAL=HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CPT2xR-PULSE_CENTER;
		CHECK_PULSE_A();
		//HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR=A_DUTY_NORMAL;
	}

	if (TimerIdx==HRTIM_TIMERINDEX_TIMER_D) {  //COMP_I IT - A or B
			if (POLKA_MODE==0) {
				POLKA_MODE=1;
				POLKA_LEVEL=Curr_Sinus_Data;
			}
			PULSE_COMP_I=1;
			/*if (HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].CPT2xR>PULSE_CENTER) { //B pulse COMP_V
				PULSE_B_COMP_I=1;
			} else { //A pulse COMP_V
				PULSE_A_COMP_I=1;
			}*/
	}
}


void HAL_HRTIM_Capture1EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                              uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_TIMER_B) { // Trig A, END OF IN1 OUTPUT
		    B_DUTY_NORMAL=PULSE_CENTER+(HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CPT1xR);
		    CHECK_PULSE_B();
		    HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].CMP1xR=B_DUTY_NORMAL;
	}

	if (TimerIdx==HRTIM_TIMERINDEX_TIMER_D) {  //COMP_V IT - A or B
		if (HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].CPT1xR>PULSE_CENTER) { //B pulse COMP_V
			PULSE_B_COMP_V=1;
		} else { //A pulse COMP_V
			PULSE_A_COMP_V=1;
		}
	}

}

void  HAL_HRTIM_RepetitionEventCallback(HRTIM_HandleTypeDef * hhrtim,
                                              uint32_t TimerIdx)
{
	//
	if (BRAVO_SINUS_IND>=PULSE_IN_WAVE) { // out of wave pulses
		BRAVO_SINUS_IND=0; // reset to start

		// check TOP-BOTTOM Wave
		if (WAVE_TOP_BOTTOM==0) {
			WAVE_TOP_BOTTOM=1;
		} else {
			WAVE_TOP_BOTTOM=0;
		}

		// check softstart state
		if (SoftStart_V==1) {
			SoftStart_CNT++;
			if (SoftStart_CNT>SOFTSTART_V_MAX_CNT) { // softstart complele
				BRAVO_SoftStart_OFF();
			} else {
					SoftStart_Coeff=(float)SoftStart_CNT/(float)SOFTSTART_V_MAX_CNT; //calc new coeff
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


	}

	if ((BRAVO_SINUS_IND<=DEATH_PULSE_IND_LEFT) || (BRAVO_SINUS_IND>=DEATH_PULSE_IND_RIGHT)) {
				SET_ALL_ZERO_PULSES();
				HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR=A_DUTY_NORMAL;
				HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].CMP1xR=B_DUTY_NORMAL;
				_BRAVO_50HZ_OFF();
				_BRAVO_DAC_ZERO(); // reset DAC to ZERO
	} else  {
				if ((BRAVO_SINUS_IND==DEATH_PULSE_IND_LEFT+1) || (BRAVO_SINUS_IND==DEATH_PULSE_IND_RIGHT+1)) {
							if (WAVE_TOP_BOTTOM==0) {
								HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin, GPIO_PIN_RESET); // OFF 50Hz_1
								HAL_GPIO_WritePin(GPIOA, MCU_50HZ2_Pin, GPIO_PIN_SET); // ON 50Hz_2
							} else {
								HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin, GPIO_PIN_SET); // ON 50Hz_1
								HAL_GPIO_WritePin(GPIOA, MCU_50HZ2_Pin, GPIO_PIN_RESET); // OFF 50Hz_2
							}
							SET_ALL_PULSES_TO_MAX();
				}

						Curr_Sinus_Data=Sinus_Data[BRAVO_SINUS_IND];

						if ((POLKA_MODE==1) && (Curr_Sinus_Data>=POLKA_LEVEL)) {
							Curr_Sinus_Data=POLKA_LEVEL;
						}

						if (POLKA_MODE==0) {
							POLKA_LEVEL=POLKA_LEVEL_MAX;
						}

						if (SoftStart_V==1) {
							Curr_Sinus_Data=(float)SoftStart_Coeff*(float)Curr_Sinus_Data;
						}

						HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,Curr_Sinus_Data+DAC_PWM_LEVEL); //Current Value of Sinus

						HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,NORMAL_CURRENT+DAC_PWM_LEVEL_I);

						/*if ((BRAVO_SINUS_IND<=SMALL_CURRENT_LEFT) || (BRAVO_SINUS_IND>=SMALL_CURRENT_RIGTH)) {
							HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,SMALL_CURRENT); //Current Value I for this Sinus
						} else {
							HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,NORMAL_CURRENT); //Current Value I for this Sinus
						}*/

						if (PULSE_B_COMP_V==0) {
							SET_ALL_PULSES_TO_MAX();
						}

							// reset blocked by COMPs flags for A and B
							CLEAR_COMPV_UNBLOCK();
							CLEAR_COMPI_UNBLOCK();

							HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR=A_DUTY_NORMAL;
							HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].CMP1xR=B_DUTY_NORMAL;
	}

	BRAVO_SINUS_IND++;
}

void HAL_HRTIM_Fault1Callback(HRTIM_HandleTypeDef *hhrtim)
{
	BRAVO_Stop();
}

void HAL_HRTIM_Compare1EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_MASTER) {
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,Curr_Sinus_Data); //Current Value of Sinus+SMALL VALUE
		HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,NORMAL_CURRENT);
	}
}

void HAL_HRTIM_Compare2EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_MASTER) {
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,Curr_Sinus_Data+DAC_PWM_LEVEL); //Current Value of Sinus+SMALL Value
		HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,NORMAL_CURRENT+DAC_PWM_LEVEL_I);
	}
}

void HAL_HRTIM_Compare3EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_MASTER) {
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,Curr_Sinus_Data); //Current Value of Sinus++SMALL Value
		HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,NORMAL_CURRENT);
	}
}

void HAL_HRTIM_Compare4EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_MASTER) {
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,Curr_Sinus_Data+DAC_PWM_LEVEL); //Current Value of Sinus+SMALL Value
		HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,NORMAL_CURRENT+DAC_PWM_LEVEL_I);
	}
}

// *** GLOBAL FLAGS *** //
volatile uint8_t BRAVO_ON=0;

// *** Pulse Section *** //
volatile uint8_t PULSE_A_COMP_V=0; // Volt,0-no block in current stage by A, 1- block
volatile uint8_t PULSE_B_COMP_V=0; // Volt,0-no block in current stage by B, 1- block

volatile uint8_t PULSE_COMP_I=0; // Current,0-no block in current stage by A or B, 1- block

volatile uint32_t A_DUTY_NORMAL=MAX_A_DUTY; // Duty by Standart close situation
volatile uint32_t A_DUTY_COMP=MAX_A_DUTY; // Duty by COMP close situation

volatile uint32_t B_DUTY_NORMAL=MAX_B_DUTY; // Duty by Standart close situation
volatile uint32_t B_DUTY_COMP=MAX_B_DUTY; // Duty by COMP close situation

void CHECK_PULSE_A(void) {
	if (A_DUTY_NORMAL>MAX_A_DUTY) {
		A_DUTY_NORMAL=MAX_A_DUTY;
	}

	if (A_DUTY_NORMAL<A_PULSE_START) {
		A_DUTY_NORMAL=A_PULSE_START;
	}
}

void CHECK_PULSE_B(void) {
	if (B_DUTY_NORMAL>MAX_B_DUTY) {
		B_DUTY_NORMAL=MAX_B_DUTY;
	}
	if (B_DUTY_NORMAL<B_PULSE_START) {
		B_DUTY_NORMAL=B_PULSE_START;
	}
}

void SET_ALL_ZERO_PULSES(void) {
	 B_DUTY_NORMAL=B_PULSE_START;

	 A_DUTY_NORMAL=A_PULSE_START;
}

void SET_ALL_PULSES_TO_MAX(void) {
	 B_DUTY_NORMAL=MAX_B_DUTY;

	 A_DUTY_NORMAL=MAX_A_DUTY;
}

void CLEAR_COMPV_UNBLOCK(void) {
	PULSE_A_COMP_V=0;
	PULSE_B_COMP_V=0;
}

void CLEAR_COMPI_UNBLOCK(void) {
	PULSE_COMP_I=0;
}

// *** SINUS SECTION *** //
volatile  uint16_t Curr_Sinus_Data=0;

volatile uint16_t BRAVO_SINUS_IND=0;

volatile uint8_t WAVE_TOP_BOTTOM=0;

volatile uint16_t Sinus_Data[PULSE_IN_WAVE] ={
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

// *** Soft Start Section *** //
volatile uint8_t SoftStart_V=1;
volatile uint16_t SoftStart_CNT=0;
volatile float  SoftStart_Coeff=1;

void BRAVO_SoftStart_ON(void) {
	SoftStart_V=1;
	SoftStart_CNT=0;
	SoftStart_Coeff=0;
}

void BRAVO_SoftStart_OFF(void) {
	SoftStart_V=0;
	SoftStart_CNT=0;
    SoftStart_Coeff=1;
}

// *** LED SECTION *** //
void UpdateSD_LED(void) {
	if (BRAVO_ON==0) {
		HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_SET); // OFF LED
	} else {
		HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_RESET); // ON LED
	}
}

// *** COMP and DAC Setion *** //
void COMP_DAC_Start(void) {
	  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // default V is ZERO

	  HAL_COMP_Start(&hcomp2);

	  // DAC2 and COMP4
	  HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
	  HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, NORMAL_CURRENT); // default I is ZERO

	  HAL_COMP_Start(&hcomp4);

}

// *** VCC and DRV SECTION *** //
void VCC15V_ON(void) {
	HAL_GPIO_WritePin(GPIOB, VCC_15V_Pin|VCC_15B_Pin, GPIO_PIN_SET); // ON VCC 15V
}

void VCC15V_OFF(void) {
	HAL_GPIO_WritePin(GPIOB, VCC_15V_Pin|VCC_15B_Pin, GPIO_PIN_RESET); // OFF VCC 15V
}

void DRV_OFF(void) {
	  HAL_GPIO_WritePin(GPIOB, DRV_SD_EX_Pin, GPIO_PIN_RESET); //  OFF EXT DRV UNBLOCK
	  HAL_GPIO_WritePin(GPIOB, DRV_SD_IN_Pin, GPIO_PIN_SET); // OFF INT DRV UNBLOCK
}

void DRV_ON(void) {
	  HAL_GPIO_WritePin(GPIOB, DRV_SD_EX_Pin, GPIO_PIN_SET); // ON EXT DRV UNBLOCK
	  HAL_GPIO_WritePin(GPIOB, DRV_SD_IN_Pin, GPIO_PIN_RESET); // ON INT DRV UNBLOCK
}

// *** START-STOP CODE *** //
void BRAVO_Stop(void) {
	DRV_OFF(); // block DRIVER

	_BRAVO_50HZ_OFF(); // off 50Hz

	_BRAVO_Stop_HRTIM(); // stop HRTIM

	__HAL_HRTIM_CLEAR_IT(&hhrtim1, HRTIM_IT_FLT1); // reset IT Flag

	BRAVO_ON=0; // set flag GLOBAL

	_BRAVO_Global_Def(); // set all to Def value

	UpdateSD_LED(); // Check LEDs
}

void BRAVO_Start(void) {
	_BRAVO_Global_Def(); // set all to Def value

	_BRAVO_50HZ_OFF(); // off 50Hz

	DRV_ON(); // Unblock DRIVER

	__HAL_HRTIM_CLEAR_IT(&hhrtim1, HRTIM_IT_FLT1); // reset IT Flag

	_BRAVO_Start_HRTIM(); //start HRTIM

	BRAVO_ON=1; // set flag GLOBAL

	UpdateSD_LED(); // Check LEDs
}

void _BRAVO_50HZ_OFF(void) {
	HAL_GPIO_WritePin(GPIOA, MCU_50HZ1_Pin|MCU_50HZ2_Pin, GPIO_PIN_RESET); // OFF 50Hz
}

void _BRAVO_Start_HRTIM(void) {
	HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TC1);
	HAL_HRTIM_WaveformCountStart_IT(&hhrtim1, HRTIM_TIMERID_MASTER
					| HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_C
					| HRTIM_TIMERID_TIMER_B | HRTIM_TIMERID_TIMER_D);
}

void _BRAVO_Stop_HRTIM(void) {
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TC1);
	HAL_HRTIM_WaveformCountStop_IT(&hhrtim1, HRTIM_TIMERID_MASTER
					| HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_C
					| HRTIM_TIMERID_TIMER_B | HRTIM_TIMERID_TIMER_D);
}

void _BRAVO_DAC_ZERO(void) {
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); // default V is ZERO
	HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, NORMAL_CURRENT); // default I is ZERO
}

void _BRAVO_Global_Def(void) {
	// reset blocked by COMPs flags for A and B
	CLEAR_COMPV_UNBLOCK();
	CLEAR_COMPI_UNBLOCK();

	// Set A+B to MIN
	SET_ALL_ZERO_PULSES();

	Curr_Sinus_Data=0; // reset DAC_V

	_BRAVO_DAC_ZERO(); // reset DAC to ZERO

	WAVE_TOP_BOTTOM=0; // TOP WAVE

	BRAVO_SINUS_IND=0; // default step
}
