#ifndef TCS34725_H
#define TCS34725_H

#include "mbed.h"

//Device slave address
#define ADDR_TCS34725  					0x53
#define ASK_DEVICE_ID_TCS34725 	0x92 //100 + 0x16
#define DEVICE_ID_TCS34725 			0x44 //there is a difference between variants
#define ATIME_REG 							0x81
#define ATIME_5 								0x00 //It's the maximun value 700ms
#define ADC_GAIN_REG 						0x8F
#define GAIN_X1 								0x00 // x1 gain
#define ENA_REG									0x80 //Access to enable register
#define ENA_MODE_3							0x03 //RGBC enable, INterrut disabled and oscillator enabled
#define RGBC_1REG								0x94 //The direction of the first register address


class TCS34725{
	private:
			uint8_t  rx_buff[8];
			uint8_t  tx_buff[2];
	
			int red, green, blue, clear;
		
	public:
		//Constructor
		TCS34725();
		
		//Destructor
		~TCS34725();
	
		//To check the DEVICE_ID
		bool check();
		
		//Te initialize the device
		bool initialize();
		
		//Read data
		void readData();
		
		//Get the measured data
		int getClear();
	
		int getRed();
	
		int getGreen();
	
		int getBlue();
	
	
};

#endif