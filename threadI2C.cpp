#include "mbed.h"
#include "Si7021.h"
#include "definitions.h"
#include "MMA8451Q.h"
#include "TCS34725.h"

Thread threadI2C(osPriorityNormal); // 1K stack size

#define MMA8451_I2C_ADDRESS (0x1d<<1)

//Declare the global vars needed for communication with main thread
float temperature, humidity, accX, accY, accZ, clear, red, green, blue;

void i2c_thread(); 

bool flagColorSensor = false, flagTHSensor = false;

//DigitalOut ledColorSensor(PA_2);


void i2c_thread() {
		
		//Instanciation of Si7021, this clase instanciates the i2c communication
		Si7021 THsensor; //SDA_PIN, SCL_PIN
		MMA8451Q accelSensor(MMA8451_I2C_ADDRESS);//Accelerometer sensor
		TCS34725 colorSensor;
		
		if(THsensor.check()){
			flagTHSensor = true;
		}
	
		//Initialize the color sensor
		if(colorSensor.check()){
				if(colorSensor.initialize()){
					flagColorSensor = true;
				}
			}
		
		//ledColorSensor = 0;
			
		while (FOREVER) {
			
			//TEMPERATURE AND HUMIDITY SENSOR
			if(flagTHSensor){
				//Perform the measurement
				if(THsensor.measure()){
					//Get the values of the relative humidity and the temperature 
						temperature   =    THsensor.get_temperature();
						humidity   		=    THsensor.get_humidity();
						}
				}

				
			//Accelerometer sensor
			accX = 255*accelSensor.getAccX();
      accY = 255*accelSensor.getAccY();
      accZ = 255*accelSensor.getAccZ();
			
			//Color sensor
			//If the device ID is the correct one
			if(flagColorSensor){
				//ledColorSensor = 1;
				wait(0.1);
				colorSensor.readData();
				clear = colorSensor.getClear();
				red = colorSensor.getRed();
				green = colorSensor.getGreen();
				blue = colorSensor.getBlue();
				//ledColorSensor = 0;
			}
			
				
			Thread::wait(200);
				  
    }
}


/*

Notes:
I2C sensors: Acccelerometer, colour and Temp/humidity


*/