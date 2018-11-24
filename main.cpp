
#include "mbed.h"
#include "definitions.h"

/**** HARDWARE RESOURCES, TIMERS AND TIMEOUTS ****/

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

//TIcker to present the information every hour
Ticker to;

//Eventflag
extern EventFlags event;

/**** GLOBAL AND LOCAL RESOURCES AND VARIABLES ****/

//Threads definition
extern Thread threadANALOG;
extern Thread threadI2C;
extern Thread threadGNSS;
extern Thread threadLimits;

//Code of the extern threads
extern void ANALOG_thread();
extern void i2c_thread();
extern void gnss_thread();
extern void limits_thread();

//Values of the sensors
extern float valueSM, valueLS, temperature, humidity, accX, accY, accZ, lon, lat, hDop, height, geoide, UTCtime;
extern int clear, red, green, blue;
//extern char *message;//valid to recover the nmea sentence of the GNSS receiver

//Global vars to control the delay of the different working modes
extern int delayThreadI2C, delayThreadAnalog, delayThreadGNSS;//For the threads
int delay;// for the main thread

//Arrays to store the values: used in normal mode 
float temperatureArray [MAX_LENGTH];
float humidityArray [MAX_LENGTH];
float lightArray [MAX_LENGTH];
int colorArray [MAX_LENGTH];
float moistureArray [MAX_LENGTH];
float accelerometerXArray [MAX_LENGTH];
float accelerometerYArray [MAX_LENGTH];
float accelerometerZArray [MAX_LENGTH];
int ind;//var to add values to the arrays

//Values of values
float meanTmp, maxTmp, minTmp, meanHum, maxHum, minHum, meanLS, maxLS, minLS, meanSM, maxSM, minSM;
float meanX, maxX, minX, meanY, maxY, minY, meanZ, maxZ, minZ;
int dominantColor;
int hour, minutes, seconds;

//flags
int flagChangeState;
int flagPresentNomalModeInfo;


/**** METHODS ****/

//Method to select the dominat Color
int selectDominantColor();

//States
enum State {TEST, NORMAL, ADVANCED};

//Declare the state
State state;

//Interrupt handler
void button_handler();

//Used when the machine is in NORMAL MODEL
void hour_handler();

//To determine the dominant color
int dominantColorCalculator(int data[]);

//To calculate the parameter that are needed when the machine is in NORMAL mode
void calculate(float data[], float *mean, float *max, float *min);

//To set the hardware and internal conditions of the state machine for the initial state
void setInitialState();

//To change the format of the hour
void UTC2LocalTime();

//To check the limits of the values
void checkLimits();

/***** MAIN THREAD: main() runs in its own thread in the OS *****/

int main() {
	
	//Print the current version of the mbed OS
	pc.printf("mbed-os-rev: %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
	
	//User button interrup to select the current state
	userButton.fall(button_handler);
	
	//Setting the initial conditions: initial state, hardware, wait times and flags
	setInitialState();
	
	//Start the threads employed
	threadI2C.start(i2c_thread);
	threadANALOG.start(ANALOG_thread);
	threadGNSS.start(gnss_thread);
	threadLimits.start(limits_thread);
	
	pc.printf("Start the monitorization\n\r");
	
	while (FOREVER) {
		
		//if the button has been pressed: set the corresponding state
		if (flagChangeState){
			switch(state){
			case TEST:
				state = NORMAL;
				testModeLed = OFF;
				normalModeLed = ON;
				advancedModeLed = OFF;
				delay = WAIT_NORMAL_MODE;
				delayThreadAnalog = WAIT_NORMAL_MODE;
				delayThreadGNSS = WAIT_NORMAL_MODE;
				delayThreadI2C = WAIT_NORMAL_MODE;
				//Restar the counter of the arrays
				ind = 0;
				//Set the timeout 
				to.attach(hour_handler, MEASUREMENTS_PERIOD);
				break;
			case NORMAL: 
				state = TEST;
				testModeLed = ON;
				normalModeLed = OFF;
				advancedModeLed = OFF;
				delay = WAIT_TEST_MODE;
				delayThreadAnalog = WAIT_TEST_MODE;
				delayThreadGNSS = WAIT_TEST_MODE;
				delayThreadI2C = WAIT_TEST_MODE;
				//desactivate the timeout
				to.detach();
				event.clear();
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
			flagChangeState = OFF;
		}
		
		//Clear the screen
		pc.printf(CLEAR_SCREEN);
		
		//TEMPERATURE AND HUMIDITY
		pc.printf("Temp: %0.5f C, RH: %0.5f\r\n", temperature , humidity );
		
		//TEMPERATURE
		pc.printf("Soil moisture: %.1f%%\r\n", valueSM);
		
		//LIGHT SENSOR
		pc.printf("Light: %.1f%%\r\n", valueLS);
		
		//Accelerometer sensor
		pc.printf("Accelerometer => accX: %.1f, accY: %.1f accZ: %.1f\r\n", accX, accY, accZ);
		
		//Color Sensor
		pc.printf("Color sensor => Clear: %d, Red:  %d, Green: %d, Blue:  %d  ", clear, red, green, blue);
		if (state == TEST) RGB = selectDominantColor();
		if (selectDominantColor() == RED_ON)	pc.printf("DOMINANT: RED\r\n");
		else if(selectDominantColor() == GREEN_ON) pc.printf("DOMINANT: GREEN\r\n");
			else pc.printf("DOMINANT: BLUE\r\n");
		
		//GNSS Receiver
		UTC2LocalTime();
		pc.printf("LocalTime: %d:%d:%d, Latitude: %0.5f, Longitud: %0.5f, hdop: %0.1f, Altura: %0.3f, geoide: %0.3f\r\n",hour, minutes, seconds, lat, lon, hDop, height, geoide);
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
		
		
		//If the state is the normal one, then store the measured values
		if (state == NORMAL){
			//Save the values
			temperatureArray[ind] = temperature;
			humidityArray[ind] = humidity;
			moistureArray[ind] = valueSM;
			lightArray[ind] = valueLS;
			colorArray[ind] = RGB.read();
			accelerometerXArray[ind] = accX;
			accelerometerYArray[ind] = accY;
			accelerometerZArray[ind] = accZ;
			ind = (ind+1)%MAX_LENGTH;
			
			//Check the limits of parameters
			checkLimits();
		}
			
			
		
		if (flagPresentNomalModeInfo) {
			//Send information to serial port
			pc.printf("\n\n\r********************NORMAL MODE INFORMATION************************\r\n");
			pc.printf("\t\tMEAN \t\tMAX \t\tMIN\r\n");
			pc.printf("Temperature:\t%0.5f C \t%0.5f C \t %0.5f C\r\n", meanTmp , maxTmp ,minTmp);
			pc.printf("Humidity:\t%0.5f %% \t%0.5f %% \t %0.5f %%\r\n", meanHum , maxHum ,minHum);
			pc.printf("Light:\t\t%0.5f  \t%0.5f  \t %0.5f \r\n", meanLS , maxLS ,minLS);
			pc.printf("Soil moisture:\t%0.5f  \t%0.5f  \t %0.5f \r\n", meanSM , maxSM ,minSM);
			pc.printf("X axis:                 \t%0.5f  \t %0.5f \r\n", maxX ,minX);
			pc.printf("Y axis:                 \t%0.5f  \t %0.5f \r\n", maxY ,minY);
			pc.printf("Z axis:                 \t%0.5f  \t %0.5f \r\n", maxZ ,minZ);
			
			if (dominantColor == RED_ON) pc.printf("Dominant color: RED\r\n");
				else if (dominantColor == GREEN_ON) pc.printf("Dominant color: GREEN\r\n");
					else if (dominantColor == BLUE_ON) pc.printf("Dominant color: BLUE\r\n");
			pc.printf("***********************END OF INFORMATION**************************\n\n\r");
			flagPresentNomalModeInfo = OFF;
		}
		
		wait(delay);
    }
}



//RGB led handler
int selectDominantColor(){
	if ((red > blue) && (red > green)){
		return RED_ON;
	}
	else{
		if((green > red) && (green > blue)){
			return GREEN_ON;
		}
		else{
			if((blue > red) && (blue > green)){
				return BLUE_ON;
			}
		}
	}
}

//To set the hardware and internal conditions of the state machine for the initial state
void setInitialState(){
	//Hardware
	testModeLed = ON;
	normalModeLed = OFF;
	advancedModeLed = OFF;
	
	//Initial state
	state = TEST;
	
	//Time to wait
	delay = WAIT_TEST_MODE;
	delayThreadAnalog = WAIT_TEST_MODE;
	delayThreadGNSS = WAIT_TEST_MODE;
	delayThreadI2C = WAIT_TEST_MODE;
	
	//flags
	flagChangeState = OFF;
	flagPresentNomalModeInfo = OFF;
}

//Interrupt handler
void button_handler(){
	flagChangeState = ON;
}


//Timeout handler, to present the mean, max, min values of the different parameters that are monitorized
void hour_handler(){
	
	//Calculate the mean value, max value, min value of different data
	calculate(temperatureArray, &meanTmp, &maxTmp, &minTmp);
	calculate(humidityArray, &meanHum, &maxHum, &minHum);
	calculate(lightArray, &meanLS, &maxLS, &minLS);
	calculate(moistureArray, &meanSM, &maxSM, &minSM);
	calculate(accelerometerXArray, &meanX, &maxX, &minX);
	calculate(accelerometerYArray, &meanY, &maxY, &minY);
	calculate(accelerometerZArray, &meanZ, &maxZ, &minZ);

	//calculate the most repeated color
	dominantColor = dominantColorCalculator(colorArray);
	
	
	flagPresentNomalModeInfo = ON;
}


//To select the dominat color during the hour
int dominantColorCalculator(int data[]){
	int countRed=0, countGreen=0, countBlue=0;
	
	for (int i = 0; i<MAX_LENGTH; i++){
		if(data[i] == RED_ON) countRed++;
		else if(data[i]==GREEN_ON) countGreen++;
			else if(data[i]==BLUE_ON) countBlue++;
	}
	
	if ((countRed >= countBlue) && (countRed >= countGreen)) return RED_ON;
	else if((countGreen > countRed) && (countGreen > countBlue))	return GREEN_ON;
		else if((countBlue > countRed) && (countBlue > countGreen))	return BLUE_ON;
	
}

//Function that calculates the mean, max and min value of a set a numbers 
void calculate(float data[], float *mean, float *max, float *min){
	float sum = 0;
	(*mean) = 0;
	(*min) = data[0];
	(*max) = data[0];
	
	for(int i = 0; i<sizeof(*data); i++){
		if(&data[i]!=NULL){
			sum+=data[i];
			if(data[i]<*min)
				(*min)=data[i];
			else if(data[i]>*max)
				(*max)= data[i];
		}
	}
	(*mean)=sum/sizeof(*data);

}


void UTC2LocalTime(){
	
	hour = (int)UTCtime/10000;
	minutes = (int)UTCtime/100 - hour*100;
	seconds = (int)(UTCtime - hour*10000 - minutes*100);
	hour ++;
}


void checkLimits(){
	uint32_t flagsLimits = 0x0;
	
	if ((temperature >= MAX_TEMP) || (temperature <= MIN_TEMP)){pc.printf(COLOR_ANSI_RED "Temperature out of limits" COLOR_RESET "\r\n"); flagsLimits+=2;}			
	if ((humidity >= MAX_HUM) || (humidity <= MIN_HUM)) {pc.printf(COLOR_ANSI_GREEN "Humidity out of limits" COLOR_RESET "\r\n"); flagsLimits+=4;}
			if ((valueSM >= MAX_MOIS) || (valueSM < MIN_MOIS)) {pc.printf(COLOR_ANSI_BLUE "Moisture out of limits" COLOR_RESET "\r\n"); flagsLimits+=8;}
			if ((valueLS >= MAX_LIGHT) || (valueLS <= MIN_LIGHT)) {pc.printf(COLOR_ANSI_YELLOW "Light out of limits" COLOR_RESET "\r\n"); flagsLimits+=16;}
			if ((clear >= MAX_COLOR) || (clear <= MIN_COLOR) || 
					(red >= MAX_COLOR) || (red <= MIN_COLOR) ||
					(blue >= MAX_COLOR) || (blue <= MIN_COLOR) ||
					(green >= MAX_COLOR) || (green <= MIN_COLOR))  {pc.printf(COLOR_ANSI_MAGENTA "Color out of limits" COLOR_RESET "\r\n"); flagsLimits+=32;}
			if ((accX >= MAX_ACC) || (accX <= MIN_ACC) ||
					(accY >= MAX_ACC) || (accY <= MIN_ACC) ||
					(accZ >= MAX_ACC) || (accZ <= MIN_ACC) ) {pc.printf(COLOR_ANSI_CYAN "Acceleration out of limits" COLOR_RESET "\r\n"); flagsLimits+=64;}
	
	if (flagsLimits != 0) flagsLimits += 1;
	event.clear();
	event.set(flagsLimits);
}