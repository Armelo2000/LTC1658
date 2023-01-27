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



/************************************************************************************************
 * Diese Routine berechnet den Widerstand NTC Thermistor mit dem gegebener Temperatur.
 * @param: temperatur, das ist temperatur vom NTC Thermistor (in °C)
 * @return: das ist der Widerstandswert der zu der gegebene Temperatur passt.
 *
 * Hinweis
 *	Temperatur in Kelvin = Temperatur in Celsius + 273,15
 * Formel
 *	RT = RN * EXP(B(1/T - 1/TN))
 *	
 *	RN – Widerstandswert bei Bezugstemperatur TN (meistens 25°C).
 *	RT - Widerstandswert bei Temperatur T
 *	B – Thermistorkonstante (Datenblatt) in Kelvin - Der wert liegt bei ca. 3988 Kelvin
 ************************************************************************************************/
float Widerstand_NTC( float temperatur);

/************************************************************************************************
 * Diese Routine berechnet anhang den Widerstand von NTC Thermistor die Spannung an dem Thermistor.
 * @param: RT, Widerstand von NTC Thermistor bei der Temperatur T
 * @return: die Spannung an den Thermistor.
 *
 * Hinweis
 *	Es wird über ein Spannungsteiler berechnet.
 		      _______
 	 Uref <______|	 R1  |_______
		      *******	   |
		      	10KOhm	   |
				   |
				   ******** V
				   |
				   |
				 __|__
		     Thermistor	 |   | RT
				 |___| 10KOhm bei 25°C
				   |
				   |
				 
 * Formel
 *	UT = RT * (Uref/(R1 + RT))
 *	
 *	R1 – Widerstandswert von R1.
 *	RT - Widerstandswert von Thermistor bei Temperatur T
 *	Uref – Referenzspannung
 *	UT - Spannungsabfall am Thermistor
 ************************************************************************************************/
float Spannung (float RT);

/************************************************************************************************
 * Diese Routine berechnet der 14Bit Digitalwert von Spannung anhang der Analog Spannungswert
 * @param: U, Analog Spannungswert
 * @return: der 14Bit digital Spannungswert.
 *
 * Hinweise
 *	Es handelt sich um einen 14Bit ADC Berechnung
 *
 * Formel 
 *	Digitalwert = (2^n - 1)/Referenzwert * Analogwert
 *
 *	n ist der Anzahl von bit (bei 14Bit, 2^14 - 1 = 16383)
 *	Referenzwert ist der maximal Analogwert 
 ************************************************************************************************/
uint16_t U_digit( float U);

/************************************************************************************************
 * Diese Routine berechnet der 14Bit Digitalwert mit der gegebene Temperatur
 * @param: temperatur in °C
 * @return: der 14Bit digitalwert.
 ************************************************************************************************/
uint16_t getDigitBy(float temperatur);

#endif /* LTC1658_LTC1658_H_ */
