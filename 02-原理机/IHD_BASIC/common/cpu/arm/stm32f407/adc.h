#ifndef __ADC_H
#define __ADC_H	

void Bsp_ADCInit(void);                      //ADCͨ����ʼ��
unsigned short  Get_Adc(unsigned char ch); 				//���ĳ��ͨ��ֵ 

void Adc3_Init(void); 				                        //ADCͨ����ʼ��
unsigned short  Get_Adc3(unsigned char ch); 				//���ĳ��ͨ��ֵ 
#endif 
