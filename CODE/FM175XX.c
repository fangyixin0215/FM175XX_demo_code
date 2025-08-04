#include "DEVICE_CFG.h"
#include "FM175XX_REG.h"
#include "stm32f10x_type.h"
#include "FM175XX.h"
#include "uart.h"
void FM175XX_HPD(unsigned char mode)//mode = 1 �˳�HPDģʽ ��mode = 0 ����HPDģʽ
{
	if(mode == 0x0)
		{
		mDelay(1);//��ʱ1ms,�ȴ�֮ǰ�Ĳ�������
			
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOA,GPIO_Pin_All);
			
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9); 
			
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11); 
			
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOD, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);				
			
		GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0	����HPDģʽ

		GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MISO | PIN_MOSI; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //GPIO����Ϊ���״̬
		GPIO_Init(PORT_SPI, &GPIO_InitStructure); 
			
		GPIO_ResetBits(PORT_SPI,PIN_SCK | PIN_MISO | PIN_MOSI);//SCK = 0��MISO = 0��MOSI = 0
			
		GPIO_InitStructure.GPIO_Pin = PIN_NSS;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;			
		GPIO_Init(PORT_SPI, &GPIO_InitStructure);	//GPIOB_NSS
		GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;	
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);//�ر�UART3
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOC,GPIO_Pin_14);//�ر�OSC32				
		
		RCC_HSICmd(ENABLE); //��HSI
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);	/* Select HSI as system clock source */

    while(RCC_GetSYSCLKSource() != 0x00){}		/* Wait till HSI is used as system clock source */
			
		RCC_HSEConfig(RCC_HSE_OFF);	   //�ر�HSE OSC		
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOD, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1);//�ر�OSC Pin
				
		}
	else
		{
		GPIO_SetBits(PORT_NRST,PIN_NRST);	//NPD = 1 �˳�HPDģʽ
		mDelay(1);//��ʱ1ms���ȴ�FM175XX����
			
		GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MISO | PIN_MOSI; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//GPIO����ΪSPI���� 
		GPIO_Init(PORT_SPI, &GPIO_InitStructure);  
			
		GPIO_InitStructure.GPIO_Pin = PIN_NSS;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(PORT_SPI, &GPIO_InitStructure);	
		GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;

	/*USART3 Configuration*/
	
		// Configure USART3 Tx (PB.10) as alternate function push-pull 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	  
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		// Configure USART3 Rx (PB.11) as input floating
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);			
		}
	return;
}


//***********************************************
//�������ƣ�Read_Reg(unsigned char reg_address,unsigned char *reg_data)
//�������ܣ���ȡ�Ĵ���ֵ
//��ڲ�����addr:Ŀ��Ĵ�����ַ   regdata:��ȡ��ֵ
//���ڲ�����unsigned char  TRUE����ȡ�ɹ�   FALSE:ʧ��
//***********************************************
void GetReg(unsigned char reg_address,unsigned char *reg_data)
{	
		GPIO_ResetBits(PORT_SPI,PIN_NSS);	//NSS = 0;
		reg_address = (reg_address << 1) | 0x80;	   
		
		SPI_SendData(SPI2,reg_address);   /* Send SPI1 data */ 
		//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);	  /* Wait for SPI1 Tx buffer empty */ 
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
		SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
		*reg_data = SPI_ReceiveData(SPI2);		 /* Read SPI1 received data */
		SPI_ClearFlag(SPI2, SPI_FLAG_RXNE); 
		SPI_SendData(SPI2,0x00);   /* Send SPI1 data */
		//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET)      
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
		SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
		*reg_data = SPI_ReceiveData(SPI2);		 /* Read SPI1 received data */

		GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;	
	return ;
}

//***********************************************
//�������ƣ�Write_Reg(unsigned char reg_address,unsigned char* reg_data)
//�������ܣ�д�Ĵ���
//��ڲ�����addr:Ŀ��Ĵ�����ַ   regdata:Ҫд���ֵ
//���ڲ�����unsigned char  TRUE��д�ɹ�   FALSE:дʧ��
//***********************************************
void SetReg(unsigned char reg_address,unsigned char reg_data)
{	
			GPIO_ResetBits(PORT_SPI,PIN_NSS);	//NSS = 0;
			reg_address = (reg_address << 1) & 0x7F;	 	   
	
			SPI_SendData(SPI2,reg_address);   /* Send SPI1 data */ 
			//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);	  /* Wait for SPI1 Tx buffer empty */ 
			while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
			SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
			SPI_ReceiveData(SPI2);		 /* Read SPI1 received data */; 
			SPI_SendData(SPI2,reg_data);   /* Send SPI1 data */
			//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);		
    	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
			SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
			SPI_ReceiveData(SPI2);		 /* Read SPI1 received data */
	
			GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;
	
	return ;
}

//*******************************************************
//�������ƣ�ModifyReg(unsigned char addr,unsigned char* mask,unsigned char set)
//�������ܣ�д�Ĵ���
//��ڲ�����addr:Ŀ��Ĵ�����ַ   mask:Ҫ�ı��λ  
//         set:  0:��־��λ����   ����:��־��λ����
//���ڲ�����
//********************************************************
void ModifyReg(unsigned char reg_address,unsigned char mask,unsigned char set)
{
	unsigned char reg_data;
	
	GetReg(reg_address,&reg_data);
	
		if(set)
		{
			reg_data |= mask;
		}
		else
		{
			reg_data &= ~mask;
		}

	SetReg(reg_address,reg_data);
	return ;
}
//*******************************************************
//�������ƣ�FM175XX_HardReset()
//�������ܣ�Ӳ����λ
//��ڲ�����
//         
//���ڲ�����
//********************************************************

void FM175XX_HardReset(void)
{	
	GPIO_SetBits(PORT_SPI,PIN_NSS);//NSS = 1
	GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0	
	mDelay(1);		
	GPIO_SetBits(PORT_NRST,PIN_NRST);//NPD = 1	
	mDelay(1);
	
	return;
}

//*******************************************************
//�������ƣ�FM175XX_SoftReset()
//�������ܣ������λ
//��ڲ�����
//         
//���ڲ�����
//********************************************************


unsigned char FM175XX_SoftReset(void)
{	
	unsigned char reg_data;
	SetReg(JREG_COMMAND,CMD_SOFT_RESET);
	mDelay(1);//FM175XXоƬ��λ��Ҫ1ms
	GetReg(JREG_COMMAND,&reg_data);
	if(reg_data == 0x20)
		return FM175XX_SUCCESS;
	else
		Uart_Send_Msg("FM175XX SoftReset Error\r\n");
	return FM175XX_RESET_ERROR;
}

//*************************************
//����  ����SetCW �ز�����
//��ڲ�����
//���ڲ�����
//*************************************

void SetCW(unsigned char cw_mode)
{
	if(cw_mode == TX1_TX2_CW_DISABLE)	
		{
		ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN|JBIT_TX2RFEN,RESET);
		}
	if(cw_mode == TX1_CW_ENABLE)	
		{
		ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN,SET);
		ModifyReg(JREG_TXCONTROL,JBIT_TX2RFEN,RESET);	
		}
	if(cw_mode == TX2_CW_ENABLE)	
		{
		ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN,RESET);
		ModifyReg(JREG_TXCONTROL,JBIT_TX2RFEN,SET);	
		}
	if(cw_mode == TX1_TX2_CW_ENABLE)	
		{
		ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN|JBIT_TX2RFEN,SET);
		}
		mDelay(30);
}

//*************************************
//����  ����ReadFIFOData
//��ڲ�����
//���ڲ�����
//*************************************
unsigned char ReadFIFOData(unsigned char *fifo_data,unsigned char fifo_data_length)
{
	unsigned char i,fifo_level;
	GetReg(JREG_FIFOLEVEL,&fifo_level);
	fifo_level = fifo_level & 0x3F;
	if (fifo_data_length > fifo_level)
		return FM175XX_FIFO_ERROR;
	for (i=0;i<fifo_data_length;i++)
		GetReg(JREG_FIFODATA,&fifo_data[i]);
	return FM175XX_SUCCESS;
}
//*************************************
//����  ����FM175XX_Initial_ReaderA
//��ڲ�����
//���ڲ�����
//*************************************
unsigned char WriteFIFOData(unsigned char *fifo_data,unsigned char fifo_data_length)
{
	unsigned char i,fifo_level;
	GetReg(JREG_FIFOLEVEL,&fifo_level);
	fifo_level = fifo_level & 0x3F;
	if (fifo_data_length > (64 - fifo_level))
		return FM175XX_FIFO_ERROR;
	for (i=0;i<fifo_data_length;i++)
		SetReg(JREG_FIFODATA,*(fifo_data + i));
	return FM175XX_SUCCESS;	
}




