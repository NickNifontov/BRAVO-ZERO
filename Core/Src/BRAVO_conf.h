/*
 * BRAVO_conf.h
 *
 *  Created on: Mar 21, 2020
 *      Author: Nick Nifontov
 */

#ifndef SRC_BRAVO_CONF_H_
#define SRC_BRAVO_CONF_H_

// *** POLKA *** //
#define  POLKA_LEVEL_MAX 4095
#define POLKA_ROLLBACK_MAX_CNT 4

// *** PULSE SETTINGS *** //
#define  MAX_A_DUTY 23300
#define  MAX_B_DUTY 48300

#define B_PULSE_START 26700
#define A_PULSE_START 1700
#define PULSE_CENTER 25000

#define DAC_PWM_LEVEL 10
#define DAC_PWM_LEVEL_I 10

// *** CURRENT DAC *** //
#define NORMAL_CURRENT 4000


// *** SOFT START SETTINGS *** //
#define SOFTSTART_V_MAX_CNT 200 // num of waves for softstart from 0.1 to 1.0

// *** PULSE SETTINGS *** //
#define PULSE_IN_WAVE 461 // PULSE in WAVE - 461 pulse A+B
#define DEATH_PULSE_IND_LEFT 3  // HOW many Pulse will be NULL at wave start - DEATH ZONE, 4 pulse
#define DEATH_PULSE_IND_RIGHT 457  // HOW many Pulse will be NULL at wave end - DEATH ZONE, 4 pulse


#endif /* SRC_BRAVO_CONF_H_ */
