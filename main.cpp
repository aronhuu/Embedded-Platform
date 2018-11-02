

#include "mbed.h"
#include "definitions.h"
	
//UART
Serial pc(USBTX,USBRX,9600);

//RGB Led
BusOut RGB(PB_13, PH_1, PH_0);//Blue, Green and Red: active with low level

//Button to control the working modes
InterruptIn userButton(PB_2);

//Leds to indicate the current working mode
DigitalOut normalModeLed(PA_5);
DigitalOut testModeLed(PB_5);
DigitalOut advancedModeLed(PB_6);

//BusOut ledState(PB_5, PA_5, PB_6);//LED1, LED2 and LED3: active with high level

//Threads definition
extern Thread threadANALOG;
extern Thread threadI2C;
extern Thread threadGNSS;

//Thread that acquire the light sensor value
extern void ANALOG_thread();
extern void i2c_thread();
extern void gnss_thread();

//FLOAT VALUES OF SEVERAL SENSORS
extern float valueSM, valueLS, temperature, humidity, accX, accY, accZ, lon, lat, hDop, height, geoide, UTCtime;
extern int clear, red, green, blue;
//extern char *message;//valid to recover the nmea sentence of the GNSS receiver

//Method to set the hardware of RGB led
void setRGBLed();

//States
enum State {TEST, NORMAL, ADVANCED};

//Declare the state
State state = TEST;

//Interrupt handler
void button_handler();


// main() runs in its own thread in the OS
int main() {
	
  pc.printf("mbed-os-rev: %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);	
	
	//User button interrup to select the current state
	userButton.fall(button_handler);
	
	//Start the threads employed
	threadI2C.start(i2c_thread);
	threadANALOG.start(ANALOG_thread);
	threadGNSS.start(gnss_thread);
	
	pc.printf("Start the monitorization\n\r");
	
	//Setting the initial conditions
	testModeLed = ON;
	normalModeLed = OFF;
	advancedModeLed = OFF;
	state = TEST;
	
	
	while (FOREVER) {
		
		//TEMPERATURE AND HUMIDITY
		pc.printf("Temp: %0.5f C, RH: %0.5f\r\n", temperature , humidity );
		
		//TEMPERATURE
		pc.printf("Soil moisture: %.1f%%\r\n", valueSM);
		
		//LIGHT SENSOR
		pc.printf("Light: %.1f%%\r\n", valueLS);
		
		//Accelerometer sensor
		pc.printf("Accelerometer => accX: %.1f, accY: %.1f accZ: %.1f\r\n", accX, accY, accZ);
		
		//Color Sensor
		pc.printf("Color sensor => Clear: %d, Red:  %d, Green: %d, Blue:  %d\r\n", clear, red, green, blue);
		setRGBLed();
		
		//GNSS Receiver
		pc.printf("UTCtime: %0.4f, Latitude: %0.5f, Longitud: %0.5f, hdop: %0.1f, Altura: %0.3f, geoide: %0.3f\r\n",UTCtime, lat, lon, hDop, height, geoide);
		//If there is fix: print I am here
		if (lat != 0){
			pc.printf("I am here: https://maps.google.com/?q=%.5f,%.5f\r\n\n", lat, lon);
		}
		else{
			pc.printf("NO FIX DATA\r\n\n");
		}
		
		if (state == TEST){
			pc.printf("Current state: TEST\r\n");
		}
		if (state == NORMAL){
			pc.printf("Current state: NORMAL\r\n");
		}
		
		pc.printf("_________________________________________________________");
		pc.printf("\n\n\r");
		
		wait(WAIT_TIME);
    }
}



//RGB led handler
void setRGBLed(){
	if ((red > blue) && (red > green)){
		RGB = RED_ON;
	}
	else{
		if((green > red) && (green > blue)){
			RGB = GREEN_ON;
		}
		else{
			if((blue > red) && (blue > green)){
				RGB = BLUE_ON;
			}
		}
	}
}



//Interrupt handler
void button_handler(){
	switch(state){
		case TEST:
			state = NORMAL;
			testModeLed = OFF;
			normalModeLed = ON;
			advancedModeLed = OFF;
			break;
		case NORMAL:
			state = TEST;
			testModeLed = ON;
			normalModeLed = OFF;
			advancedModeLed = OFF;
			break;
		case ADVANCED:
			state = TEST;
			testModeLed = ON;
			normalModeLed = OFF;
			advancedModeLed = OFF;
			break;
		default:
			break;
		}
}


