
#include "Si7021.h"
#include "communications.h"

Si7021::Si7021()
{
    i2c.frequency(FREQ);
}

Si7021::~Si7021()
{
    
}

float Si7021::get_temperature()
{
    return tData;
}

float Si7021::get_humidity()
{
    return rhData;
}

bool Si7021::measure()
{
    tx_buff[0] = READ_RH;
    if(i2c.write(ADDR, (char*)tx_buff, 1) != 0) return 0;
    if(i2c.read(ADDR, (char*)rx_buff, 2) != 0) return 0;
    
		rhData = (float)((uint32_t)rx_buff[0] << 8) +  rx_buff[1];
		rhData = (rhData*125/65536)-6;
    
    tx_buff[0] = READ_TEMP;
    if(i2c.write(ADDR, (char*)tx_buff, 1) != 0) return 0;
    if(i2c.read(ADDR, (char*)rx_buff, 2) != 0) return 0;
    
    tData = (float)((uint32_t)rx_buff[0] << 8) + (rx_buff[1]);
		tData = (tData *175.72/65536)-46.85;
    return 1;
}

bool Si7021::check()
{
    tx_buff[0] = READ_ID2_1;
    tx_buff[1] = READ_ID2_2;
    if(i2c.write(ADDR, (char*)tx_buff, 2) != 0) return 0;
    if(i2c.read(ADDR, (char*)rx_buff, 8) != 0) return 0;
    
    if(rx_buff[0] == DEVICE_ID)
        return 1;
    else return 0;
}

