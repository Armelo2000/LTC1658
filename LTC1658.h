/*
 * LTC1658.h
 *
 *  Created on: Jan 23, 2023
 *      Author: melah
 */

#ifndef LTC1658_LTC1658_H_
#define LTC1658_LTC1658_H_

#include "main.h"

extern SPI_HandleTypeDef hspi2;

#define TIM_COUNT_UP  0x00000000
//Temperaturkoeffizient für NTC Thermistor
#define B_Wert 3988
//Referenzspannung
#define Uref 2.7
//Widerstand von R1
#define R1 10000

//Base Adresse von SysTick
#define SYSTICK_BASEADDR   0xE000E010


typedef struct
{
	volatile uint32_t CSR;  	//SysTick Control and Status Register
	volatile uint32_t RVR;  	//SysTick Reload Value Register
	volatile uint32_t CVR; 		//SysTick Current Value Register
	volatile uint32_t CALIB;    //SysTick Calibration Value Register

}SysTick_RegDef_t;


#define SYSTICK ((SysTick_RegDef_t*)SYSTICK_BASEADDR)

/********************************************************************
 * Routine aktiviert den SysTick (1ms) mit interrupt
 ********************************************************************/
void sysTick_init();

/********************************************************************************
 * Diese Routine aktiviert den Chip select (Pin Low) um Daten transfert über
 * SPI zu erlauben
 ********************************************************************************/
void LTC1658_Enable_CS();

/********************************************************************************
 * Diese Routine deaktiviert den Chip select (Pin High) um Daten transfert über
 * SPI zu stoppen.
 ********************************************************************************/
void LTC1658_Disable_CS();

/********************************************************************************
 * Diese Routine schreibt daten zu den DAC Wandler.
 * @param data, das ist ein 16bit daten. Es wird davon nur 14Bit vom DAC verwendet
 ********************************************************************************/
void LTC1658_WriteData(uint16_t data);

/********************************************************************************
 * Diese Routine liest daten vom DAC Wandler.
 * @return data, das ist der wert der vom DAC kommt.
 ********************************************************************************/
uint16_t LTC1658_ReadData();

/*******************************************************************
 * Initialisierung von Timer 2
 *******************************************************************/
void TIM2_Init();

#endif /* LTC1658_LTC1658_H_ */
