#include "sd_spi.h"

static uint8_t SD_Type = 0;
  
void SPI_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
 
 
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE);//ʹ��SPI1ʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIObʱ��
  //GPIOFB3,4,5��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_MOSI|GPIO_SCK|GPIO_MISO;//PB3~5���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOʱ��
  GPIO_InitStructure.GPIO_Pin = GPIO_NSS;//NSS	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
  CS_H;
	
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); //PB3����Ϊ SD_SPI
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4����Ϊ SD_SPI
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); //PB5����Ϊ SD_SPI
	
  SPI_I2S_DeInit(SD_SPI);
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
  SPI_Init(SD_SPI, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
  SPI_Cmd(SD_SPI, ENABLE); //ʹ��SPI����
  SPI_ReadWrite_Byte(0xff);
}
//spi��дһ���ֽ�
uint8_t SPI_ReadWrite_Byte(uint8_t TxData)
{     
  uint16_t timeout = 0;
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)//�ȴ���������
  {
    timeout++;
    if(timeout>9999) return 0;
  }  
	SPI_I2S_SendData(SD_SPI, TxData); //ͨ������SPIx����һ��byte  ����
  timeout = 0;
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET) //�ȴ�������һ��byte  
  {
    timeout++;
    if(timeout>9999) return 0;
  }  
	return SPI_I2S_ReceiveData(SD_SPI); //����ͨ��SPIx������յ�����		
}
//����SPI���� ��ʼ���ʲ��ܸ���400k
void SD_SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
  SD_SPI->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
  SD_SPI->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
  SPI_Cmd(SD_SPI,ENABLE); //ʹ��SPI1
}

/*********************************************************************/
//SD����һЩ����������

//ȡ��Ƭѡ �ͷ�SPI����
void SD_DisSelect(void)
{
  CS_H;
  SPI_ReadWrite_Byte(0xff);
}
//�ȴ���׼����
//׼�����˷���ֵ0�������������
uint8_t SD_WaitReady(void)
{
  uint32_t t=0;
  do
  {
	if(SPI_ReadWrite_Byte(0xff)==0XFF)return 0;//OK
	t++;		  	
  }while(t<0XFFFFFF);
  return 1;
}
//ѡ��SD�����ȴ���׼����
//����ֵ 0�ɹ���1ʧ��
uint8_t SD_Select(void)
{
  CS_L;
  if(SD_WaitReady()==0)return 0;
  SD_DisSelect();
  return 1;
}
//�����Ӧ
//Response Ҫ�õ��Ļ�Ӧ?
//������Ӧ����
uint8_t SD_GetResponse(uint8_t Response)
{
  u16 Count=0xFFFF;   		//�ȴ�����				  
  while ((SPI_ReadWrite_Byte(0xff)!=Response)&&Count)Count--;	  
  if (Count==0)return MSD_RESPONSE_FAILURE;  
  else return MSD_RESPONSE_NO_ERROR;
}
//��SD����ȡһ�����ݰ�������
//buf���ݻ�����
//len��ȡ�����ݳ���
uint8_t SD_RecvData(uint8_t*buf,u16 len)
{			  	  
  if(SD_GetResponse(0xFE))return 1;
  while(len--)
  {
    *buf=SPI_ReadWrite_Byte(0xff);
     buf++;
  }
  SPI_ReadWrite_Byte(0xff);
  SPI_ReadWrite_Byte(0xff);									  					    
  return 0;
}
//��SD��д��һ�����ݰ�������512�ֽ�
//buf���ݻ���
//����
//����ֵ0�ɹ� ����ʧ��
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{	
  u16 t;		  	  
  if(SD_WaitReady())return 1;
  SPI_ReadWrite_Byte(cmd);
  if(cmd!=0XFD)
  {
	for(t=0;t<512;t++)SPI_ReadWrite_Byte(buf[t]);
	SPI_ReadWrite_Byte(0xFF);
	SPI_ReadWrite_Byte(0xFF);
	t=SPI_ReadWrite_Byte(0xFF);
	if((t&0x1F)!=0x05)return 2;								  					    
   }						 									  					    
  return 0;
}
//��SD������һ������
//uint8_t cmd  ����
//uint32_t arg �������
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t r1;	
  uint8_t Retry=0; 
  SD_DisSelect();
  if(SD_Select())return 0XFF;
    SPI_ReadWrite_Byte(cmd | 0x40);
    SPI_ReadWrite_Byte(arg >> 24);
    SPI_ReadWrite_Byte(arg >> 16);
    SPI_ReadWrite_Byte(arg >> 8);
    SPI_ReadWrite_Byte(arg);	  
    SPI_ReadWrite_Byte(crc); 
  if(cmd==CMD12) SPI_ReadWrite_Byte(0xff);//Skip a stuff byte when stop reading
	Retry=0X1F;
  do
   {
	 r1=SPI_ReadWrite_Byte(0xFF);
   }while((r1&0X80) && Retry--);	 
  return r1;
}
//��ȡSD����CID��Ϣ���� ������������Ϣ
//uint8_t *cid_data	���CID���ڴ�����16���ֽ�
//���� 0:NO_ERR
//		 1:����														   
uint8_t SD_GetCID(uint8_t *cid_data)
{
  uint8_t r1;	   
  r1=SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
  {
	r1=SD_RecvData(cid_data,16);	//����16���ֽڵ����� 
  }
  SD_DisSelect();
  if(r1)return 1;
  else return 0;
}																				  
//��ȡSD����CSD��Ϣ���� �����������ٶ���Ϣ
//uint8_t *cid_data	���CsD���ڴ�����16���ֽ�
//���� 0:NO_ERR
//		 1:����														   
uint8_t SD_GetCSD(uint8_t *csd_data)
{
  uint8_t r1;	 
  r1=SD_SendCmd(CMD9,0,0x01);
  if(r1==0)
  {
     r1=SD_RecvData(csd_data, 16); 
   }
  SD_DisSelect();
  if(r1)return 1;
  else return 0;
}  
//��ȡSD����������
//����ֵ:0: ȡ����������
//      ����:SD����(������/512�ֽ�)														  
uint32_t SD_GetSectorCount(void)
{
  uint8_t csd[16];
  uint32_t Capacity;  
  uint8_t n;
  u16 csize;  					    
  //ȡCSD��Ϣ
  if(SD_GetCSD(csd)!=0) return 0;	    
    
  if((csd[0]&0xC0)==0x40)	 //V2.00��
  {	
	 csize = csd[9] + ((u16)csd[8] << 8) + 1;
	 Capacity = (uint32_t)csize << 10;//�õ�������		   
  }else//V1.XX��
   {	
	  n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
	  csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
	  Capacity= (uint32_t)csize << (n - 9);//�õ������� 
   }
   return Capacity;
}
//��ʼ��SD
uint8_t SD_Init_Config(void)
{
  uint8_t r1;      // 
  u16 retry;  // 
  uint8_t buf[4];  
  u16 i;
 
  SPI_init();	//��ʼ��IO
  for(i=0;i<10;i++)SPI_ReadWrite_Byte(0XFF);//��������74������
  retry=20;
  do
  {
	 r1=SD_SendCmd(CMD0,0,0x95);//����IDLE״̬
   }while((r1!=0X01) && retry--);
   SD_Type=0;//Ĭ���޿�
  if(r1==0X01)
   {
	  if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
	  {
		for(i=0;i<4;i++)buf[i]=SPI_ReadWrite_Byte(0XFF);	//Get trailing return value of R7 resp
		if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
		{
		  retry=0XFFFE;
		  do
		  {
			 SD_SendCmd(CMD55,0,0X01);	//����CMD55
			 r1=SD_SendCmd(CMD41,0x40000000,0X01);//����CMD41
		    }while(r1&&retry--);
		  if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//����SD2.0��ʼ
		  {
			 for(i=0;i<4;i++)buf[i]=SPI_ReadWrite_Byte(0XFF);//�õ�OCRֵ
			 if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //���CCS
			  else SD_Type=SD_TYPE_V2;   
		    }
		 }
		}else//SD V1.x/ MMC	V3
		{
		  SD_SendCmd(CMD55,0,0X01);		//����CMD55
		  r1=SD_SendCmd(CMD41,0,0X01);	//����CMD41
		  if(r1<=1)
		  {		
			SD_Type=SD_TYPE_V1;
			retry=0XFFFE;
			do 
			{
			  SD_SendCmd(CMD55,0,0X01);	//����CMD55
			  r1=SD_SendCmd(CMD41,0,0X01);//����CMD41
			}while(r1&&retry--);
		  }else
		     {
			   SD_Type=SD_TYPE_MMC;//MMC V3
			   retry=0XFFFE;
			   do 
			    {											    
				  r1=SD_SendCmd(CMD1,0,0X01);
			     }while(r1&&retry--);  
		   }
		  if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;
		}
    SD_DisSelect();
    SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);
   }
  if(SD_Type)return 0;
  else if(r1)return r1; 	   
  return 0xaa;
}
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint16_t cnt)
{
  uint8_t r1;
  if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;
  if(cnt==1)
  {
	r1=SD_SendCmd(CMD17,sector,0X01);
	if(r1==0)
	{
	  r1=SD_RecvData(buf,512);	   
	}
  }else
    {
	   r1=SD_SendCmd(CMD18,sector,0X01);
	   do
	   {
		  r1=SD_RecvData(buf,512);	 
		  buf+=512;  
		}while(--cnt && r1==0); 	
	   SD_SendCmd(CMD12,0,0X01);	
	 }   
  SD_DisSelect();
  return r1;
}
//uint8_t*buf���ݻ���
//sector��ʼ����
//������
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint16_t cnt)
{
  uint8_t r1;
  if(SD_Type!=SD_TYPE_V2HC)sector *= 512;
  if(cnt==1)
  {
	 r1=SD_SendCmd(CMD24,sector,0X01);
	 if(r1==0)
	 {
		r1=SD_SendBlock(buf,0xFE);  
	  }
   }else
    {
	  if(SD_Type!=SD_TYPE_MMC)
	   {
		  SD_SendCmd(CMD55,0,0X01);	
		  SD_SendCmd(CMD23,cnt,0X01);
		}
 	  r1=SD_SendCmd(CMD25,sector,0X01);
	  if(r1==0)
	  {
		do
		{
		  r1=SD_SendBlock(buf,0xFC);	 
		  buf+=512;  
		 }while(--cnt && r1==0);
		 r1=SD_SendBlock(0,0xFD);
		}
	}   
  SD_DisSelect();
  return r1;
}


