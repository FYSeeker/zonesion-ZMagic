#ifndef _ILI93XX_H_
#define _ILI93XX_H_

#include "stdio.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "fsmc.h"
#include "delay.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
//lcd ʹ��PD7�� NE1���ܻ���ַΪ0x60000000,����A17 ��Ϊrsѡ��
#define ILI93XX_RS_PIN       17
#define ILI93XX_REG          (*((volatile uint16_t *)(0x60000000)))
#define ILI93XX_DAT          (*((volatile uint16_t *)(0x60000000 | (1<<(ILI93XX_RS_PIN+1)))))

#define ILI93XX_WCMD(x)       (ILI93XX_REG=(x))
#define ILI93XX_WDATA(x)      (ILI93XX_DAT=(x))
#define ILI93XX_RDATA()       ILI93XX_DAT

#define ILI93XX_WIDE     240
#define ILI93XX_HIGH     320

//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���	 

void Ili93xx_PrepareWrite(void);
void Ili93xx_SetCursorPos(short x,short y);
void Ili93xx_PrepareFill(short x1, short y1, short x2, short y2);
void Ili93xx_SetWindow(short x1, short y1, short x2, short y2);

uint8_t Ili93xx_init(void);
void Ili93xx_DrawPoint(short x,short y,uint32_t color);
uint32_t Ili93xx_ReadPoint(short x,short y);
void Ili93xx_FillColor(short x1,short y1,short x2,short y2,uint32_t color);
void Ili93xx_FillData(short x1,short y1,short x2,short y2,unsigned short* dat);

#endif