#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

//Definitions for main thread
#define FOREVER 	1
#define WAIT_NORMAL_MODE		5 //Must to be 30
#define WAIT_TEST_MODE			2
#define WAIT_ADVANCED_MODE	5

//For normal mode functionality
#define MAX_LENGTH 12//must to be 120
#define MEASUREMENTS_PERIOD 60//must to be 3600

//Definitions of salve address of I2C channel: temperature and humidity sensor
#define ADDR_TH 0x80

#define RED_ON 			3
#define GREEN_ON 		5
#define BLUE_ON 		6
#define YELLOW_ON 	1
#define CYAN_ON			4
#define MAGENTA_ON 	2
#define WHITE_ON 		0

#define ON 		1
#define OFF 	0

//LIMITS FOR NORMAL MOODE
#define MAX_TEMP 26
#define MIN_TEMP 15

#define MAX_LIGHT 80
#define MIN_LIGHT 5

#define MAX_HUM 70
#define MIN_HUM 30

#define MAX_MOIS 80
#define MIN_MOIS 0

#define MAX_COLOR 10000
#define MIN_COLOR 200

#define MAX_ACC 257
#define MIN_ACC -255


#define LONG_TIME 10

#define COLOR_RESET "\x1b[0m"
#define COLOR_ANSI_RED "\x1b[31m"
#define COLOR_ANSI_GREEN "\x1b[32m"
#define COLOR_ANSI_YELLOW "\x1b[33m"
#define COLOR_ANSI_BLUE "\x1b[34m"
#define COLOR_ANSI_MAGENTA "\x1b[35m"
#define COLOR_ANSI_CYAN "\x1b[36m"
#define CLEAR_SCREEN "\033[2J\033[1;1H"

#define DEFAULT_WAIT_TIME_TL 1000

#endif
