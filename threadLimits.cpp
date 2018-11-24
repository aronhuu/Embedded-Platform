#include "mbed.h"
#include "definitions.h"

EventFlags event;

extern BusOut RGB;

Thread threadLimits(osPriorityNormal, 512); // 1K stack size

void limits_thread();

int determineWaitTime(uint32_t flags);

void limits_thread()
{
	uint32_t flags = 0x0;
	int wait_time;
	
	while (true) {
		flags = event.wait_any(0x7F, osWaitForever, false);
		
		//flags = event.get();
		wait_time = determineWaitTime(flags);
		
		if(flags & 64){//check accelerometer flag
			RGB = CYAN_ON;
			Thread::wait(wait_time);
		}
		if(flags & 32){//check color flag
			RGB = MAGENTA_ON;
			Thread::wait(wait_time);
		}
		if(flags & 16){//check light flag
			RGB = YELLOW_ON;
			Thread::wait(wait_time);
		}
		if(flags & 8){//check moisture flag
			RGB = BLUE_ON;
			Thread::wait(wait_time);
		}
		if(flags & 4){//check humidity flag
			RGB = GREEN_ON;
			Thread::wait(wait_time);
		}
		if(flags & 2){//check temperature flag
			RGB = RED_ON;
			Thread::wait(wait_time);
		}
		
	}
}

int	determineWaitTime(uint32_t flags){
	int wait_time = DEFAULT_WAIT_TIME_TL;
	int counter = 0;
	
	if(flags & 64){//check accelerometer flag
		counter++;
	}
	if(flags & 32){//check color flag
		counter++;
	}
	if(flags & 16){//check light flag
		counter++;
	}
	if(flags & 8){//check moisture flag
		counter++;
	}
	if(flags & 4){//check humidity flag
		counter++;
	}
	if(flags & 2){//check temperature flag
		counter++;
	}
	
	if (counter != 0) return wait_time/counter;

	return wait_time;
}