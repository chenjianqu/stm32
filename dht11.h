#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f4xx_it.h"

//请求信号状态
#define DHT11_REQUEST_OK 1
#define DHT11_REQUEST_FAIL 0

//请求数据状态
#define DHT11_GETDATA_OK 1
#define DHT11_GETDATA_FAIL 0

u8 initDHT11(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x);

u8 requestDHT11(void);

u8 readDHT11Bit(void);

u8 readDHT11Byte(void);

u8 readDHT11Data(u8 *temp,u8 *humi);

#endif

