/*********************************************************************
*                                                                    *
*   Copyright (c) 2010 Shanghai FuDan MicroElectronic Inc, Ltd.      *
*   All rights reserved. Licensed Software Material.                 *
*                                                                    *
*   Unauthorized use, duplication, or distribution is strictly       *
*   prohibited by law.                                               *
*                                                                    *
*********************************************************************/

/********************************************************************/
/* 	主要功能:						        																		*/
/* 	    TYPE A 和 TYPE B卡片Polling操作								        			*/
/*      支持身份证UID读取														      					*/
/* 	编制:宋耀海 																										*/
/* 	编制时间:2017年9月7日																						*/
/* 																																	*/
/********************************************************************/
#include "string.h"
#include "DEVICE_CFG.h"
#include "FM175XX_REG.h"
#include "READER_API.h"
#include "FM175XX.h"
//#include "MIFARE.h"
#include "UART.h"
struct picc_a_struct PICC_A; 
struct picc_b_struct PICC_B;


//*************************************
//函数  名：FM175XX_Initial_ReaderA
//入口参数：
//出口参数：
//*************************************

void FM175XX_Initial_ReaderA(void)
{	
	SetReg(JREG_MODWIDTH,MODWIDTH_106);	//MODWIDTH = 106kbps
	ModifyReg(JREG_TXAUTO,BIT6,SET);//Force 100ASK = 1
	SetReg(JREG_GSN,(GSNON_A<<4));//Config GSN; Config ModGSN 	
	SetReg(JREG_CWGSP,GSP_A);//Config GSP
	SetReg(JREG_CONTROL,BIT4);//Initiator = 1
	SetReg(JREG_RFCFG,RXGAIN_A<<4);//Config RxGain
	SetReg(JREG_RXTRESHOLD,(MINLEVEL_A<<4) | COLLLEVEL_A);//Config MinLevel; Config CollLevel	
	return;
}
//*************************************
//函数  名：FM175XX_Initial_ReaderB
//入口参数：
//出口参数：
//*************************************

void FM175XX_Initial_ReaderB(void)
{
	ModifyReg(JREG_STATUS2,BIT3,RESET);

	SetReg(JREG_MODWIDTH,MODWIDTH_106);//MODWIDTH = 106kbps	
	SetReg(JREG_TXAUTO,0);//Force 100ASK = 0		
	SetReg(JREG_GSN,(GSNON_B<<4)|MODGSNON_B);//Config GSN; Config ModGSN   
	SetReg(JREG_CWGSP,GSP_B);//Config GSP
	SetReg(JREG_MODGSP,MODGSP_B);//Config ModGSP
	SetReg(JREG_CONTROL,BIT4);//Initiator = 1
	SetReg(JREG_RFCFG,RXGAIN_B<<4);//Config RxGain
	SetReg(JREG_RXTRESHOLD,MINLEVEL_B<<4);//Config MinLevel;
	return;
}

//*************************************
//函数  名：ReaderA_Halt
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderA_Halt(void)
{	
	unsigned char reg_data;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x80);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0x50);
	SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(2);//Wait 2ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 0)
	{	
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;
	
}
//*************************************
//函数  名：ReaderA_Wakeup
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderA_Wakeup(void)
{
	unsigned char reg_data;
	SetReg(JREG_TXMODE,0);//Disable TxCRC
	SetReg(JREG_RXMODE,0);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,RF_CMD_WUPA);
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x87);//Start Send
	mDelay(1);//Wait 1ms
	GetReg(JREG_FIFOLEVEL,&reg_data);//
	if(reg_data == 2)
	{
		GetReg(JREG_FIFODATA,PICC_A.ATQA);
		GetReg(JREG_FIFODATA,PICC_A.ATQA+1);
		return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;
}
//*************************************
//函数  名：ReaderA_Request
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderA_Request(void)
{
	unsigned char reg_data;
	SetReg(JREG_TXMODE,0);//Disable TxCRC
	SetReg(JREG_RXMODE,0);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,RF_CMD_REQA);
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x87);//Start Send
	mDelay(2);//Wait 2ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 2)
	{
		GetReg(JREG_FIFODATA,PICC_A.ATQA);
		GetReg(JREG_FIFODATA,PICC_A.ATQA+1);
		return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}

//*************************************
//函数  名：ReaderA_AntiColl
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderA_AntiColl(void)
{
	unsigned char reg_data;
	SetReg(JREG_TXMODE,0);//Disable TxCRC
	SetReg(JREG_RXMODE,0);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,RF_CMD_ANTICOL);
	SetReg(JREG_FIFODATA,0x20);
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(2);//Wait 2ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 5)
	{
		GetReg(JREG_FIFODATA,PICC_A.UID);
		GetReg(JREG_FIFODATA,PICC_A.UID+1);
		GetReg(JREG_FIFODATA,PICC_A.UID+2);
		GetReg(JREG_FIFODATA,PICC_A.UID+3);
		GetReg(JREG_FIFODATA,&PICC_A.BCC);
		if( (PICC_A.UID[0] ^ PICC_A.UID[1] ^ PICC_A.UID[2] ^ PICC_A.UID[3]) == PICC_A.BCC)
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}

//*************************************
//函数  名：ReaderA_Select
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderA_Select(void)
{
	unsigned char reg_data;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x80);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0x93);
	SetReg(JREG_FIFODATA,0x70);
	SetReg(JREG_FIFODATA,PICC_A.UID[0]);
	SetReg(JREG_FIFODATA,PICC_A.UID[1]);
	SetReg(JREG_FIFODATA,PICC_A.UID[2]);
	SetReg(JREG_FIFODATA,PICC_A.UID[3]);
	SetReg(JREG_FIFODATA,PICC_A.BCC);		
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(2);//Wait 2ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		GetReg(JREG_FIFODATA,&PICC_A.SAK);		
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}

//*************************************
//函数  名：ReaderA_CardActivate
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderA_CardActivate(void)
{
unsigned char  result;	
		result = ReaderA_Wakeup();//	
		if (result != FM175XX_SUCCESS)
		{			
			return FM175XX_COMM_ERROR;
		}
		result = ReaderA_AntiColl();//
		if (result != FM175XX_SUCCESS)
						return FM175XX_COMM_ERROR;
					
		result = ReaderA_Select();//
		if (result != FM175XX_SUCCESS)
						return FM175XX_COMM_ERROR;
								
			
		return result;
}
//*************************************
//函数  名：ReaderB_Wakeup
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderB_Wakeup(void)
{
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0x05);
	SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_FIFODATA,0x08);
	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 12)
	{
		for(i = 0;i < 12;i++)
		GetReg(JREG_FIFODATA,&PICC_B.ATQB[i]);
		memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
		memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
		memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}
//*************************************
//函数  名：ReaderB_Request
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderB_Request(void)
{	
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0x05);
	SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_FIFODATA,0x00);
	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 12)
	{
		for(i = 0;i < 12;i++)
		GetReg(JREG_FIFODATA,&PICC_B.ATQB[i]);
		memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
		memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
		memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}
//*************************************
//函数  名：ReaderB_Attrib
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderB_Attrib(void)
{
	unsigned char reg_data;
	SetReg(JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0x1D);
	SetReg(JREG_FIFODATA,PICC_B.PUPI[0]);
	SetReg(JREG_FIFODATA,PICC_B.PUPI[1]);
	SetReg(JREG_FIFODATA,PICC_B.PUPI[2]);
	SetReg(JREG_FIFODATA,PICC_B.PUPI[3]);
	SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_FIFODATA,0x08);
	SetReg(JREG_FIFODATA,0x01);
	SetReg(JREG_FIFODATA,0x01);
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{		
		GetReg(JREG_FIFODATA,PICC_B.ATTRIB);
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}
//*************************************
//函数  名：ReaderB_GetUID
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char ReaderB_GetUID(void)
{
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_FIFODATA,0x36);
	SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_FIFODATA,0x08);
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 10)
	{		
		for(i=0;i<8;i++)
		GetReg(JREG_FIFODATA,&PICC_B.UID[i]);
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}
//*************************************
//函数  名：Reader_Polling
//入口参数：
//出口参数：FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
unsigned char Reader_Polling(unsigned char *polling_card)
{
unsigned char result;
	*polling_card = 0;
//	FM175XX_SoftReset();
	FM175XX_Initial_ReaderA();
	SetCW(TX1_TX2_CW_ENABLE);
	result = ReaderA_Wakeup();//
		if (result == FM175XX_SUCCESS)
		{			
			*polling_card |= BIT0;
		}		
	FM175XX_Initial_ReaderB();
	result = ReaderB_Wakeup();//
		if (result == FM175XX_SUCCESS)
		{			
			*polling_card |= BIT1;
		}
		SetCW(TX1_TX2_CW_DISABLE);
if (*polling_card != 0)		
	return FM175XX_SUCCESS;	
else
	return FM175XX_COMM_ERROR;
}

unsigned char Reader_TPDU(transmission_struct *tpdu)
{
	unsigned char result;
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	result = WriteFIFOData(tpdu->pSendBuffer,tpdu->SendLength);
	if(result != FM175XX_SUCCESS)
		return result;
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	ModifyReg(JREG_BITFRAMING,0x80,SET);//Start Send
	mDelay(tpdu->Timeout);//
	GetReg(JREG_FIFOLEVEL,&tpdu->ReceiveLength);
	if(tpdu->ReceiveLength > 0)
		result = ReadFIFOData(tpdu->pReceiveBuffer,tpdu->ReceiveLength);
	else
		result = FM175XX_TIMER_ERROR;//未在timeout时间内接收到数据	
	return result;	
}



