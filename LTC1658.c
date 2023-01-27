/*
 * LTC1658.c
 *
 *  Created on: Jan 23, 2023
 *      Author: melah
 */

#include "LTC1658.h"
#include "stm32f103xb.h"

extern uint32_t SystemCoreClock;

/*****************************************************************
 * Timer Handler Funktion
 * Diese Funktion wird aufgerufen wenn der Timer ausgelöst ist
 *
 **********************************************************************/
void TIM2_IRQHandler()
{
	TIM2->SR&=~TIM_SR_UIF; //reset UIF
}

/*******************************************************************
 * Initialisierung von Timer 2
 *******************************************************************/
void TIM2_Init()
{
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;

	// 2. Init timer
	// CEN: Enables counter
	TIM2->CR1 |= (1<<0);
	// UDIS: Update event enabled
	TIM2->CR1 &= ~(1<<1);
	// URS: Update request source enabled for overflow/underflow only
	TIM2->CR1 |= (1<<2);
	// OPM: One Pulse Mode. Counter continues counting.
	TIM2->CR1 &= ~(1<<3);
	// Auto reload preload enabled
	TIM2->CR1 &= ~(1<<7);

	// UIE: Update interrupt enabled
	TIM2->DIER |= (1<<0);

	// UG: Update generation. Re-initializes the counter and updates registers
	TIM2->EGR |= (1<<0);
	//TIM2->CNT = 0x00FA;

	// prescaler setzen 8000. Timer clock is now 8MHz/8000=1000Hz => 1KHz
	TIM2->PSC = 8000;

	// Zähler für 1ms
	TIM2->ARR = 1000 - 1; //1KHz

  /* Enable the TIM2 global Interrupt */
	  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  /* Configure the TIM IRQ priority */
  HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0U);
}

/********************************************************************
 * Diese Routine aktiviert den SysTick Timer mit einem Zeitinterval
 *  von 1ms mit interrupt.
 *
 *  TODO Achtung diese Funktion wird nicht gebraucht
 ********************************************************************/
void sysTick_init()
{
	//damit wird ein 1 millisekunde Zähler realisiert
	SYSTICK->RVR = SystemCoreClock / 1000;

	//SysTick aktuelle register wert löschen
	SYSTICK->CVR= 0;

	//SysTick mit interne system source und interrupt aktivieren
	SYSTICK->CSR= (1 << 0) | (1 << 2) | (1 << 1);

}

/********************************************************************************
 * Diese Routine aktiviert den Chip select (Pin Low) um Daten transfert über
 * SPI zu erlauben
 ********************************************************************************/
void LTC1658_Enable_CS(){
	HAL_GPIO_WritePin(LTC1658_CS_GPIO_Port, LTC1658_CS_Pin, GPIO_PIN_RESET);
}

/********************************************************************************
 * Diese Routine deaktiviert den Chip select (Pin High) um Daten transfert über
 * SPI zu stoppen.
 ********************************************************************************/
void LTC1658_Disable_CS(){
	HAL_GPIO_WritePin(LTC1658_CS_GPIO_Port, LTC1658_CS_Pin, GPIO_PIN_SET);
}


/********************************************************************************
 * Diese Routine schreibt daten zu den DAC Wandler.
 * @param data, das ist ein 16bit daten. Es wird davon nur 14Bit vom DAC verwendet
 ********************************************************************************/
void LTC1658_WriteData(uint16_t data){

	HAL_StatusTypeDef status;
	//Die 2 letzte Bits werden nicht berücksichtigt. Data soll um 2 Bit nach
	//link verschoben werden.
	data <<= 2;

	//Aktiviere Das IC durch Chip select (Low)
	LTC1658_Enable_CS();

	status = HAL_SPI_Transmit(&hspi2, (uint8_t *)&data, 2, 100);
	if(status != HAL_OK){
		//Ein Fehler ist während Datentransfert aufgetreten
		//while(1);
	}

	//Deaktiviere Das IC durch Chip select (High)
	LTC1658_Disable_CS();

}

/********************************************************************************
 * Diese Routine liest daten vom DAC Wandler.
 * @return data, das ist der wert der vom DAC kommt.
 *
 * TODO Diese Funktion wird noch nicht verwendet
 ********************************************************************************/
uint16_t LTC1658_ReadData(){

	HAL_StatusTypeDef status;

	uint16_t result;
	uint16_t dummyData = 0x0000;

	//Aktiviere Das IC durch Chip select (Low)
	LTC1658_Enable_CS();

	//sende dummy daten um den shift register von LTC1658 mit neue Daten zu bereiten
	//status = HAL_SPI_Transmit(&hspi2, (uint8_t *)&dummyData, 2, 100);
	//if(status != HAL_OK){
		//Ein Fehler ist während Datentransfert aufgetreten
		//while(1);

	//}

	//status = HAL_SPI_Receive(&hspi2, (uint8_t *)&result, 2, 100);
	//if(status != HAL_OK){
		//Ein Fehler ist während Datentransfert aufgetreten
		//while(1);
	//}

	//Deaktiviere Das IC durch Chip select (High)
	LTC1658_Disable_CS();

	//Das Ergebnis liegt in den erste 14 Bits. So wird erst nach 2 Bits nach rechts verschoben
	//damit das ergebnis ausgegeben werden kann.
	result >>= 2;

	return result;
}

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
float Widerstand_NTC( float temperatur){
	
	//Das ist der Widerstandswert von Thermistor bei 25°C der Wert ist 10 KOhm => 10000 Ohm
	uint16_t RN = 10000;

	//Umrechnung von Celsius zu Kelvin (T25 = 298,15 °K)
	float T25 = 25 + 273.15;
	
	//Die Temperatur wird in Celsius eingegeben. 
	//Die folgende Zeile macht die Berechnung von Celsius zu Kelvin
	temperatur = temperatur + 273.15;
	
	// x wird in dem exponent verwendet.
	float x = B_Wert * ( 1/temperatur - 1/T25);

	//Der Widerstandswert bei der gegebene Temperatur wird hier mit zurückgegeben
	return RN * exp(x);

}

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
float Spannung (float RT){
	
	float UT = RT * (Uref/(R1 + RT));
	return UT;
}

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
uint16_t U_digit( float U){

	uint16_t Digit = 16383 * U/Uref;
	return Digit;

}

/************************************************************************************************
 * Diese Routine berechnet der 14Bit Digitalwert mit der gegebene Temperatur
 * @param: temperatur in °C
 * @return: der 14Bit digitalwert.
 ************************************************************************************************/
uint16_t getDigitBy(float temperatur){
	float R = Widerstand_NTC(temperatur);
	float U = Spannung(R);
	uint16_t digit = U_digit(U);
	return digit;
}

