/*
 * BRAVO_hal.h
 *
 *  Created on: Mar 21, 2020
 *      Author: Nick Nifontov
 */

#ifndef SRC_BRAVO_HAL_H_
#define SRC_BRAVO_HAL_H_

#include "BRAVO_conf.h"

#include "main.h"
#include "adc.h"
#include "comp.h"
#include "dac.h"
#include "dma.h"
#include "hrtim.h"
#include "iwdg.h"
#include "tim.h"
#include "gpio.h"

//*** SYSTEM *** //
void Enable_SH_DEBUG(void);

// *** GLOBAL FLAGS *** //
extern volatile uint8_t BRAVO_ON;

// *** POLKA *** //
extern volatile uint8_t  POLKA_MODE;
extern volatile uint16_t  POLKA_LEVEL;
extern volatile uint16_t  POLKA_ROLLBACK;


// *** Pulse Section *** //
extern volatile uint8_t PULSE_A_COMP_V; // Volt,0-no block in current stage by A, 1- block
extern volatile uint8_t PULSE_B_COMP_V; // Volt,0-no block in current stage by B, 1- block

extern volatile uint8_t PULSE_COMP_I; // Current,0-no block in current stage by A or B, 1- block

extern volatile uint32_t A_DUTY_NORMAL; // Duty by Standart close situation

extern volatile uint32_t B_DUTY_NORMAL; // Duty by Standart close situation

void SET_ALL_ZERO_PULSES(void);
void SET_ALL_PULSES_TO_MAX(void);
void CLEAR_COMPV_UNBLOCK(void);
void CLEAR_COMPI_UNBLOCK(void);

void CHECK_PULSE_A(void);
void CHECK_PULSE_B(void);

// *** SINUS SECTION *** //
extern volatile  uint16_t Curr_Sinus_Data; // curent Sinus Table Value for A+B - DAC_V
extern volatile uint16_t BRAVO_SINUS_IND; // index of pulse in wave
extern volatile uint8_t WAVE_TOP_BOTTOM; // 0 - top, 1-bottom
extern volatile uint16_t Sinus_Data[PULSE_IN_WAVE]; // SINUS DATA TABLE

// *** START-STOP CODE *** //
void BRAVO_Stop(void);
void BRAVO_Start(void);
void _BRAVO_50HZ_OFF(void);
void _BRAVO_Stop_HRTIM(void);
void _BRAVO_Start_HRTIM(void);
void _BRAVO_Global_Def(void);
void _BRAVO_DAC_ZERO(void);

// *** Soft Start Section *** //
extern volatile uint8_t SoftStart_V;
extern volatile uint16_t SoftStart_CNT;
extern volatile float  SoftStart_Coeff;

void BRAVO_SoftStart_ON(void);
void BRAVO_SoftStart_OFF(void);

// *** LED SECTION *** //
void UpdateSD_LED(void);

// *** VCC and DRV SECTION *** //
void VCC15V_ON(void);
void VCC15V_OFF(void);
void DRV_OFF(void);
void DRV_ON(void);

// *** COMP and DAC Setion *** //
void COMP_DAC_Start(void);

#endif /* SRC_BRAVO_HAL_H_ */
