#include "mbed.h"
#include "SerialGPS.h"

Thread threadGPS(osPriorityNormal, 512); // 1K stack size

//Declare the global vars needed for communication with main thread
SerialGPS gps = SerialGPS (PA_9,PA_10,9600);
float lon, lat, hDop, height, geoide;
//int sats;

void gps_thread(); 

void gps_thread() {
		while (1) {
			if(gps.sample())
				lon, lat, hDop, height, geoide = gps.longitude, gps.latitude, gps.hdop, gps.alt,gps.geoid;
			
			Thread::wait(200);  
    }
}


