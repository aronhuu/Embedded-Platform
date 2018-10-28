

#include "communications.h"
#include "TCS34725.h"


//Constructor
TCS34725::TCS34725(){
}

//Destructor
TCS34725::~TCS34725(){
}

//Chech if the device is the correct one
bool TCS34725::check(){
	
	tx_buff[0] = ASK_DEVICE_ID_TCS34725;
	
	if(i2c.write(ADDR_TCS34725, (char*) tx_buff, 1, true) != 0) return 0;
	if(i2c.read(ADDR_TCS34725, (char*)rx_buff, 2, false) != 0) return 0;
 
  if (rx_buff[0]==DEVICE_ID_TCS34725) {
		return 1;
    }
	return 0;
}


//initialize method
bool TCS34725::initialize(){
	
	//Write in the time register
	char timing_register[2] = {129,0}; // ATIME (0x01) 
	tx_buff[0] = ATIME_REG;
	tx_buff[1] = ATIME_5;
	
	if(i2c.write(ADDR_TCS34725, (char *)tx_buff, 2, false) != 0) return 0;
	
	// Set the ADC gain to x1 factor.
	tx_buff[0] = ADC_GAIN_REG;
	tx_buff[1] = GAIN_X1;
	
	if(i2c.write(ADDR_TCS34725,(char *)tx_buff, 2, false) != 0) return 0;
	
	// Used to power the TCS3472 device on and off, and enable functions and interrupts.
	// in this case, 3 --> 0000 0011 means disble interruptions, enable RGBC sensor and enable oscillator.
	tx_buff[0] = ENA_REG;
	tx_buff[1] = ENA_MODE_3;
	if(i2c.write(ADDR_TCS34725, (char *)tx_buff, 2, false) != 0) return 0;
	
	return 1;
}


//To read the data of the sensor, all at te same time
void TCS34725::readData(){
	
	tx_buff[0] = RGBC_1REG;
	
	i2c.write(ADDR_TCS34725, (char *)tx_buff, 1, true);
	i2c.read(ADDR_TCS34725, (char *) rx_buff, 8, false);
	
	clear = ((int)rx_buff[1] << 8) | rx_buff[0];
	red = ((int)rx_buff[3] << 8) | rx_buff[2];
	green = ((int)rx_buff[5] << 8) | rx_buff[4];
	blue = ((int)rx_buff[7] << 8) | rx_buff[6];

}

//Getters
int TCS34725::getClear(){
	return clear;
}

int TCS34725::getRed(){
	return red;
}

int TCS34725::getGreen(){
	return green;
}

int TCS34725::getBlue(){
	return blue;
}
