#ifndef __W25Q64_H
#define __W25Q64_H
#include "stm32f4xx_it.h"

//W25X系列/Q系列芯片列表:
#define W25Q80 0XEF13
#define W25Q16 0XEF14
#define W25Q32 0XEF15
#define W25Q64 0XEF16	
#define W25Q128 0XEF17	



//指令表
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData					0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_BlockErase				0xD8 
#define W25X_SectorErase			0x20 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown				0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID					0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 


void initW25x(void);

//读取状态寄存器
u8 readW25xSR(void);

//写状态寄存器
void writeW25xSR(u8 v);

//写使能
void enableW25xWrite(void);

//写保护
void disableW25xWrite(void);

//等待空闲
void waitW25xBusy(void);

//进入掉电模式
void pdW25q64(void);

//唤醒
void wpW25x(void);

//读取FLASH ID
u16 readW25xID(void);

//读取flash
void readW25x(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);

//写入flash
void writeW25x(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

//写一页flash
void writeW25xPage(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

//无检验写SPI FLASH 
void writeW25x_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

//扇区擦除
void eraseW25xSector(u32 Dst_Addr);

//整片擦除
void eraseW25xChip(void);

//获取大小
u32 getW25xSize(void);

#endif
