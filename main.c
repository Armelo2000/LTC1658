#include "LTC1658.h"

#define TEMPERATUR_CELSIUS    40

//Temperaturkoeffizient für NTC Thermistor
#define B_Wert 3988
//Referenzspannung
#define Uref 2.7
//Widerstand von R1
#define R1 10000

int main()
{
  
   //Widerstandswert von Thermistor (RN) bei der Bezugstemperatur 25°C
   //Bezugstemperatur (T25) 25°C in Kelvin. T25 = 298,15 °K
   //Temperatur von Thermistor in Kelvin
   //x ist der temporäre wert für der Exponent in den Formel RT = RN * EXP(B(1/T - 1/T25))
   //Widerstandswert von Thermistor (RT) bei der Temperatur T
   //Spannungsabfall von Thermistor (UT) bei der Temperatur T
   float RN, T25, temperatur, x, RT, UT; 

   //Digital wert der über SPI gesendet werden soll.
   uint16_t digit;
	
  /*
    Die Initialisierung von SPI kommt hier 
  */
  
  /******************************************************************************************
   * Hier wird der Widerstandswert von Thermistor bei der Temperatur T berechnet 
   *
  ******************************************************************************************/
  //Das ist der Widerstandswert von Thermistor bei 25°C der Wert ist 10 KOhm => 10000 Ohm
  RN = 10000;
  
  //Umrechnung von Celsius zu Kelvin (T25 = 298,15 °K)
  T25 = 25 + 273.15;

  //Die folgende Zeile macht die Berechnung von Celsius zu Kelvin
  temperatur = TEMPERATUR_CELSIUS + 273.15;

  // x wird in dem exponent verwendet.
  x = B_Wert * ( 1/temperatur - 1/T25);

  //Der Widerstandswert bei der gegebene Temperatur wird in RT gespeichert.
  RT = RN * exp(x);

  //Mit dem Spannungsteiler wird die Spannung von Thermistor berechnet
  UT = RT * (Uref/(R1 + RT));
 
 /*************************************************************************************************	
  * Formel 
  *	Digitalwert = (2^n - 1) * Analogwert / Referenzwert
  *
  *	n ist der Anzahl von bit (bei 14Bit, 2^14 - 1 = 16383)
  *	Referenzwert ist der maximal Analogwert (in diesem fall 2.7V)
  ************************************************************************************************/
  digit = 16383 * U/Uref;

  
  
  while(1){
    LTC1658_WriteData(digit);
  }
  
  
}
