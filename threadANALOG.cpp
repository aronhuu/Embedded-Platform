#include "mbed.h"

AnalogIn soil_moisture(PA_0);
AnalogIn light_sensor(PA_4);

float valueSM, valueLS=0.0;


Thread threadANALOG(osPriorityNormal, 512); // 1K stack size

void ANALOG_thread(); 

void ANALOG_thread() {
		while (true) {

				Thread::wait(200);
				valueSM = soil_moisture*100;
				valueLS = light_sensor*100;
    }
}

