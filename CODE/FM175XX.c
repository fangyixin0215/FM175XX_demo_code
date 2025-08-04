#include "DEVICE_CFG.h"
#include "FM175XX_REG.h"
#include "stm32f10x_type.h"
#include "FM175XX.h"
#include "uart.h"
void FM175XX_HPD(unsigned char mode)//mode = 1 退出HPD模式 ，mode = 0 进入HPD模式
{
	if(mode == 0x0)
		{
		mDelay(1);//延时1ms,等待之前的操作结束
			
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
			
		GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0	进入HPD模式

		GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MISO | PIN_MOSI; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //GPIO配置为输出状态
		GPIO_Init(PORT_SPI, &GPIO_InitStructure); 
			
		GPIO_ResetBits(PORT_SPI,PIN_SCK | PIN_MISO | PIN_MOSI);//SCK = 0；MISO = 0；MOSI = 0
			
		GPIO_InitStructure.GPIO_Pin = PIN_NSS;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;			
		GPIO_Init(PORT_SPI, &GPIO_InitStructure);	//GPIOB_NSS
		GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;	
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);//关闭UART3
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOC,GPIO_Pin_14);//关闭OSC32				
		
		RCC_HSICmd(ENABLE); //打开HSI
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);	/* Select HSI as system clock source */

    while(RCC_GetSYSCLKSource() != 0x00){}		/* Wait till HSI is used as system clock source */
			
		RCC_HSEConfig(RCC_HSE_OFF);	   //关闭HSE OSC		
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOD, &GPIO_InitStructure);	
		GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1);//关闭OSC Pin
				
		}
	else
		{
		GPIO_SetBits(PORT_NRST,PIN_NRST);	//NPD = 1 退出HPD模式
		mDelay(1);//延时1ms，等待FM175XX启动
			
		GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MISO | PIN_MOSI; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//GPIO配置为SPI功能 
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
//函数名称：Read_Reg(unsigned char reg_address,unsigned char *reg_data)
//函数功能：读取寄存器值
//入口参数：addr:目标寄存器地址   regdata:读取的值
//出口参数：unsigned char  TRUE：读取成功   FALSE:失败
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
//函数名称：Write_Reg(unsigned char reg_address,unsigned char* reg_data)
//函数功能：写寄存器
//入口参数：addr:目标寄存器地址   regdata:要写入的值
//出口参数：unsigned char  TRUE：写成功   FALSE:写失败
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
//函数名称：ModifyReg(unsigned char addr,unsigned char* mask,unsigned char set)
//函数功能：写寄存器
//入口参数：addr:目标寄存器地址   mask:要改变的位  
//         set:  0:标志的位清零   其它:标志的位置起
//出口参数：
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
//函数名称：FM175XX_HardReset()
//函数功能：硬件复位
//入口参数：
//         
//出口参数：
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
//函数名称：FM175XX_SoftReset()
//函数功能：软件复位
//入口参数：
//         
//出口参数：
//********************************************************


unsigned char FM175XX_SoftReset(void)
{	
	unsigned char reg_data;
	SetReg(JREG_COMMAND,CMD_SOFT_RESET);
	mDelay(1);//FM175XX芯片复位需要1ms
	GetReg(JREG_COMMAND,&reg_data);
	if(reg_data == 0x20)
		return FM175XX_SUCCESS;
	else
		Uart_Send_Msg("FM175XX SoftReset Error\r\n");
	return FM175XX_RESET_ERROR;
}

//*************************************
//函数  名：SetCW 载波设置
//入口参数：
//出口参数：
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
//函数  名：ReadFIFOData
//入口参数：
//出口参数：
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
//函数  名：FM175XX_Initial_ReaderA
//入口参数：
//出口参数：
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




