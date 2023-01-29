
#define TEMPERATUR_CELSIUS    40

int main()
{
  /*
    Die Initialisierung von SPI kommt hier 
  */
  
  /******************************************************************************************
   * Hier wird der Widerstandswert von Thermistor bei der Temperatur T berechnet 
   *
  ******************************************************************************************/
  //Das ist der Widerstandswert von Thermistor bei 25°C der Wert ist 10 KOhm => 10000 Ohm
	float RN = 10000;
  
  //Das ist der Widerstandswert von Thermistor bei der Temperatur T
	float RT;

	//Umrechnung von Celsius zu Kelvin (T25 = 298,15 °K)
	float T25 = 25 + 273.15;
 
	//Die folgende Zeile macht die Berechnung von Celsius zu Kelvin
	float temperatur = TEMPERATUR_CELSIUS + 273.15;

	// x wird in dem exponent verwendet.
	float x = B_Wert * ( 1/temperatur - 1/T25);

	//Der Widerstandswert bei der gegebene Temperatur wird hier mit zurückgegeben
	RT = RN * exp(x);
  
  
  
  while(1){
    
  }
  
  
}
