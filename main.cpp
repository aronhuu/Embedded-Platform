#include "mbed.h"
#include "rtos.h"


Serial pc(USBTX,USBRX,19200);

extern Thread threadANALOG;

extern void ANALOG_thread();

extern float valueSM, valueLS; 



// main() runs in its own thread in the OS
int main() {


    threadANALOG.start(ANALOG_thread);

    pc.printf("mbed-os-rev: %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);	
	
    while (true) {

			  pc.printf("\n\r");
			
				// SOIL MOISTURE
			  pc.printf("\n\rSOIL MOISTURE: %.1f%%",valueSM);
			
				pc.printf("\n\rLIGHT SENSOR: %.1f%%",valueLS);
			
        wait(2);
			
    }
}