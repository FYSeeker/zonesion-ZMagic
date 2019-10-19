#ifndef _ST7789_H_
#define _ST7789_H_

#include "stdio.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "fsmc.h"
#include "delay.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
//lcd ʹ��PD7�� NE1���ܻ���ַΪ0x60000000,����A17 ��Ϊrsѡ��
#define ST7789_RS_PIN       17
#define ST7789_REG          (*((volatile uint16_t *)(0x60000000)))
#define ST7789_DAT          (*((volatile uint16_t *)(0x60000000 | (1<<(ST7789_RS_PIN+1)))))

#define St7789_WCMD(x)    (ST7789_REG=(x))
#define St7789_WDATA(x)       (ST7789_DAT=(x))
#define St7789_RDATA()         ST7789_DAT

#define ST7789_WRAM     0X2C 		  //��ʼдgramָ��	
#define ST7789_RRAM     0X2E 		  //��ʼ��gramָ��
#define ST7789_SETX     0X2A 		  //����x����ָ��
#define ST7789_SETY     0X2B 		  //����y����ָ��	 

#define ST7789_WIDE     320
#define ST7789_HIGH     240

void St7789_PrepareWrite(void);
void St7789_SetCursorPos(short x,short y);
void St7789_PrepareFill(short x1, short y1, short x2, short y2);

uint8_t St7789_init(void);
void St7789_DrawPoint(short x,short y,uint32_t color);
uint32_t St7789_ReadPoint(short x,short y);
void St7789_FillColor(short x1,short y1,short x2,short y2,uint32_t color);
void St7789_FillData(short x1,short y1,short x2,short y2,unsigned short* dat);

#endif