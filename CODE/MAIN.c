#include "DEVICE_CFG.H"
#include "UART.h"
#include <string.h>
#include <stdio.h> 
#include "FM175XX.h"
#include "FM175XX_REG.h"
#include "READER_API.h"
#include "MIFARE.h"
#include "cpu_card.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_map.h"
void TYPE_A_CARD_EVENT(void);
void TYPE_B_CARD_EVENT(void);
void MIFARE_CARD_EVENT(void);
void CPU_CARD_EVENT(void);


void MIFARE_CARD_EVENT(void)
{
	unsigned char result;
		result = Mifare_Auth(KEY_A_AUTH,0,KEY_A[0],PICC_A.UID);
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);	
				Uart_Send_Msg("-> AUTH ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> AUTH SUCCESS!\r\n");	
				
		memcpy(BLOCK_VAULE,"\x00\x00\x00\x08",4);		
		result = Mifare_Blockset(1,BLOCK_VAULE);//Block 1 SET VALUE						
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);	
				Uart_Send_Msg("-> SET BLOCK VALUE ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> SET BLOCK VALUE SUCCESS!\r\n");
				
		result = Mifare_Blockread(1,BLOCK_DATA);//READ Block 1
					if(result != FM175XX_SUCCESS)
							{
							SetCW(TX1_TX2_CW_DISABLE);
							Uart_Send_Msg("-> READ BLOCK ERROR!\r\n");		
							return;
							}
					Uart_Send_Msg("-> READ BLOCK = ");Uart_Send_Hex(BLOCK_DATA,16); Uart_Send_Msg("\r\n");	

		memcpy(INCREMENT_VALUE,"\x00\x00\x00\x04",4);	

		result = Mifare_Blockinc(1,INCREMENT_VALUE);
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);	
				Uart_Send_Msg("-> INC VALUE ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> INC VALUE SUCCESS!\r\n");
								
		result = Mifare_Transfer(1);
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);	
				Uart_Send_Msg("-> TRANSFER ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> TRANSFER SUCCESS!\r\n");

		result = Mifare_Blockread(1,BLOCK_DATA);//READ Block 1
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);
				Uart_Send_Msg("-> READ BLOCK ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> READ BLOCK = ");Uart_Send_Hex(BLOCK_DATA,16); Uart_Send_Msg("\r\n");			
							
				
		memcpy(DECREMENT_VALUE,"\x00\x00\x00\x03",4);			
			
		result = Mifare_Blockdec(1,DECREMENT_VALUE);
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);	
				Uart_Send_Msg("-> INC VALUE ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> INC VALUE SUCCESS!\r\n");
								
		result = Mifare_Restore(1);		
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);	
				Uart_Send_Msg("-> RESTORE ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> RESTORE SUCCESS!\r\n");		
				
		result = Mifare_Blockread(1,BLOCK_DATA);//READ Block 1
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);
				Uart_Send_Msg("-> READ BLOCK ERROR!\r\n");		
				return;
				}
		Uart_Send_Msg("-> READ BLOCK = ");Uart_Send_Hex(BLOCK_DATA,16); Uart_Send_Msg("\r\n");
}

void CPU_CARD_EVENT(void)
{
unsigned char result;
unsigned char SendBuffer[255];	
unsigned char ReceiveBuffer[255];
		transmission_struct APDU;
		APDU.pSendBuffer = SendBuffer;//设置发送数组
		APDU.pReceiveBuffer = ReceiveBuffer;//设置接收数组
		result = CPU_Rats(&CPU_CARD.ATS.Length,CPU_CARD.ATS.Ats_Data);
		if(result != FM175XX_SUCCESS)
			{
				SetCW(TX1_TX2_CW_DISABLE);
				Uart_Send_Msg("-> RATS ERROR!\r\n");		
				return;
			}
		Uart_Send_Msg("-> ATS = ");Uart_Send_Hex(CPU_CARD.ATS.Ats_Data,CPU_CARD.ATS.Length); Uart_Send_Msg("\r\n");
//		CPU_CARD.PCB = 0x02;//PCB初始值为0x02
//		
//		memcpy(APDU.pSendBuffer,"\x00\xA4\x04\x00\x09\xF0\x77\x61\x6C\x6C\x65\x74\x00\x01\x01\0B",16);	
//		APDU.SendLength = 16;		
//		result = CPU_APDU(&APDU);		
//		if(result != FM175XX_SUCCESS)
//				{
//				SetCW(TX1_TX2_CW_DISABLE);
//				Uart_Send_Msg("-> APDU ERROR!\r\n");		
//				return;
//				}		
//		Uart_Send_Msg("-> APDU = ");Uart_Send_Hex(APDU.pReceiveBuffer,APDU.ReceiveLength); Uart_Send_Msg("\r\n");	
//						
//			
//			
//				memcpy(APDU.pSendBuffer,"\x80\x06\x00\x00\x08\x77\x61\x6C\x6C\x65\x74\x49\x44\x08",14);	
//		APDU.SendLength = 14;		
//		result = CPU_APDU(&APDU);		
//		if(result != FM175XX_SUCCESS)
//				{
//				SetCW(TX1_TX2_CW_DISABLE);
//				Uart_Send_Msg("-> APDU ERROR!\r\n");		
//				return;
//				}		
//		Uart_Send_Msg("-> APDU = ");Uart_Send_Hex(APDU.pReceiveBuffer,APDU.ReceiveLength); Uart_Send_Msg("\r\n");	
//						
//			
//			
//				
			
			
//			
//		memcpy(APDU.pSendBuffer,"\x00\xA4\x00\x00\x02\x3F\x00",7);	
//		APDU.SendLength = 7;		
//		result = CPU_APDU(&APDU);		
//		if(result != FM175XX_SUCCESS)
//				{
//				SetCW(TX1_TX2_CW_DISABLE);
//				Uart_Send_Msg("-> APDU ERROR!\r\n");		
//				return;
//				}		
//		Uart_Send_Msg("-> APDU = ");Uart_Send_Hex(APDU.pReceiveBuffer,APDU.ReceiveLength); Uart_Send_Msg("\r\n");	
//			
//		memcpy(APDU.pSendBuffer,"\x00\xA4\x04\x00\x0E\x32\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31",19);	
//		APDU.SendLength = 19;		
//		result = CPU_APDU(&APDU);		
//		if(result != FM175XX_SUCCESS)
//				{
//				SetCW(TX1_TX2_CW_DISABLE);
//				Uart_Send_Msg("-> APDU ERROR!\r\n");		
//				return;
//				}		
//		Uart_Send_Msg("-> Response = ");Uart_Send_Hex(APDU.pReceiveBuffer,APDU.ReceiveLength); Uart_Send_Msg("\r\n");
//				
//		memcpy(APDU.pSendBuffer,"\x00\xA4\x04\x00\x08\xA0\x00\x00\x03\x33\x01\x01\x01\x00",14);	
//		APDU.SendLength = 14;		
//		result = CPU_APDU(&APDU);		
//		if(result != FM175XX_SUCCESS)
//				{
//				SetCW(TX1_TX2_CW_DISABLE);
//				Uart_Send_Msg("-> APDU ERROR!\r\n");		
//				return;
//				}		
//		Uart_Send_Msg("-> Response = ");Uart_Send_Hex(APDU.pReceiveBuffer,APDU.ReceiveLength); Uart_Send_Msg("\r\n");	
//			
//		memcpy(APDU.pSendBuffer,"\x80\xA8\x00\x00\x24\x83\x22\x28\x00\x00\x80\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x01\x56\x00\x00\x00\x00\x00\x01\x56\x13\x08\x28\x82\x12\x34\x56\x78\x00\x00",42);	
//		APDU.SendLength = 42;		
//		result = CPU_APDU(&APDU);		
//		if(result != FM175XX_SUCCESS)
//				{
//				SetCW(TX1_TX2_CW_DISABLE);
//				Uart_Send_Msg("-> APDU ERROR!\r\n");		
//				return;
//				}		
//		Uart_Send_Msg("-> Response = ");Uart_Send_Hex(APDU.pReceiveBuffer,APDU.ReceiveLength); Uart_Send_Msg("\r\n");
//						
				
}

void TYPE_A_CARD_EVENT(void)
{
unsigned char result;
		Uart_Send_Msg("\r\n-> TYPE A CARD!\r\n");	
//		FM175XX_SoftReset();
		FM175XX_HardReset();
		FM175XX_Initial_ReaderA();
		SetCW(TX1_TX2_CW_ENABLE);		
		result = ReaderA_CardActivate();
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);		
				return;
				}
		Uart_Send_Msg("-> ATQA = ");Uart_Send_Hex(PICC_A.ATQA, 2);Uart_Send_Msg("\r\n");					
		Uart_Send_Msg("-> UID = ");Uart_Send_Hex(PICC_A.UID, 4 );Uart_Send_Msg("\r\n");					
		Uart_Send_Msg("-> SAK = ");Uart_Send_Hex(&PICC_A.SAK, 1);Uart_Send_Msg("\r\n");		
		if(PICC_A.SAK == 0x08)	
				MIFARE_CARD_EVENT();
		if(PICC_A.SAK == 0x20)
				CPU_CARD_EVENT();
		SetCW(TX1_TX2_CW_DISABLE);	
		return;
}

void TYPE_B_CARD_EVENT(void)
{
unsigned char result;	
		Uart_Send_Msg("-> TYPE B CARD!\r\n");			
		FM175XX_Initial_ReaderB();
		SetCW(TX1_TX2_CW_ENABLE);					
		result = ReaderB_Wakeup();
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);		
				return;
				}		
		Uart_Send_Msg("-> ATQB = ");Uart_Send_Hex(PICC_B.ATQB, 12);Uart_Send_Msg("\r\n");		
					
		result = ReaderB_Attrib();
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);		
				return;
				}
		Uart_Send_Msg("-> ATTRIB = ");Uart_Send_Hex(PICC_B.ATTRIB,1);Uart_Send_Msg("\r\n");		
			
		result = ReaderB_GetUID();
		if(result != FM175XX_SUCCESS)
				{
				SetCW(TX1_TX2_CW_DISABLE);		
				return;
				}
		Uart_Send_Msg("-> UID = ");Uart_Send_Hex(PICC_B.UID, 8);Uart_Send_Msg("\r\n");			
				
		SetCW(TX1_TX2_CW_DISABLE);	
		return;
}



int main(void)
{	  
unsigned char polling_card,regdata;

	RCC_Config();			//时钟配置
	GPIO_Config();		//GPIO配置	
	USART3_Config();	//UART3配置	
	SPI_Config();			//SPI2配置		
	EXTI_Config();  	//外部中断配置
			
	UART_Com_Para_Init(&UART_Com3_Para);    //初始化Uart3的通讯参数

	Uart_Send_Msg("FM17550 READER STM32 DEMO\r\n");
	
	LED_ON;
	
	FM175XX_HardReset();	
	
	LED_OFF;	
		
	while(1)
	{		
		while(1)
		{
		mDelay(600);		
			FM175XX_HardReset();	
			GetReg(0x01,&regdata);
				Uart_Send_Msg("-> 0x01 = ");Uart_Send_Hex(&regdata, 1);Uart_Send_Msg("\r\n");		
		if(Reader_Polling(&polling_card)== FM175XX_SUCCESS)
				{
					if(polling_card & BIT0)//TYPE A
						{	
							TYPE_A_CARD_EVENT();							
						}
			
					if(polling_card & BIT1)
						{							
							TYPE_B_CARD_EVENT();
						}
				}					
		
		}
			
	}

}


