 //2018年2月2日编辑，支持CPU卡片指令，链接方式接收
 #include "FM175XX.h"
 #include "CPU_CARD.h"
 #include <string.h>
 #include "READER_API.h"
 #include "FM175XX_REG.h"
 #include "DEVICE_CFG.h"
 #include "UART.h"
 struct CPU_CARD_STR CPU_CARD;  

/****************************************************************/
/*名称: Rats 																										*/
/*功能: Request for answer to select														*/
/*输入: 																												*/
/*																															*/
/*输出:																													*/
/*		ats_len：接收的ATS数据长度																*/
/*		*ats：接收到的ATS数据指针																	*/
/* OK: 应答正确						        															*/
/* ERROR: 应答错误																							*/
/****************************************************************/
unsigned char CPU_Rats(unsigned char *ats_len,unsigned char *ats)
{
	unsigned char result;
	unsigned char outbuffer[2],inbuffer[64];
	transmission_struct tpdu;
	tpdu.pSendBuffer = outbuffer;
	tpdu.pReceiveBuffer = inbuffer;	
	tpdu.pSendBuffer[0] = 0xE0;//Start byte
	tpdu.pSendBuffer[1] = 0x50;//default = 0x50
	tpdu.SendLength = 2;
	tpdu.Timeout = 100;
	result = Reader_TPDU(&tpdu);
	if(result == FM175XX_SUCCESS)
	{
		*ats_len = tpdu.ReceiveLength;
		memcpy(ats,tpdu.pReceiveBuffer,*ats_len);		
	}
	return result;	
}
/****************************************************************/
/*名称: CPU_APDU 							*/
/*功能: 该函数实现转入APDU命令格式处理	CPU card reset		*/
/*输入: 				*/
/*		*/
/*输出:								*/
/*		ats_len：接收的数据长度			*/
/*		ats：接收的数据指针				*/
/* OK: 应答正确						        */
/* ERROR: 应答错误						*/
/****************************************************************/
unsigned char CPU_APDU(transmission_struct *apdu)
{
	unsigned char result,pcb_byte,tpdu_buffer[64],unsent_length,i;
	transmission_struct tpdu;
	tpdu.pSendBuffer = tpdu_buffer;
	tpdu.pReceiveBuffer = tpdu_buffer;	
	tpdu.Timeout = 500;
	apdu->ReceiveLength = 0;
	unsent_length = apdu->SendLength;//设置待发送数据长度

	for(i=0;i<16;i++)//发送流程，按最小分组长度16，最多进行16分组发送
	{
			//Uart_Send_Msg("unsent_length = ");Uart_Send_Hex(&unsent_length,1);Uart_Send_Msg("\r\n");	
			if(unsent_length < FRAME_SIZE)
			{		
			//待发送数据长度小于最大帧长度，发送所有待发送数据
			tpdu.pSendBuffer[0] = CPU_CARD.PCB ;//PCB字节写入TPDU数组
			memcpy(tpdu.pSendBuffer + 1,apdu->pSendBuffer + apdu->SendLength - unsent_length,unsent_length);//APDU数据写入TPDU数组
			tpdu.SendLength = unsent_length + 1;//发送长度增加1
			Uart_Send_Msg("--> ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");		 
			result = Reader_TPDU(&tpdu);
			if((result != FM175XX_SUCCESS)||(tpdu.ReceiveLength == 0))  
				 return result;
			Uart_Send_Msg("<-- ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			unsent_length = 0;//待发送数据长度清0	
			break;//完成数据发送流程后退出循环
			}
			else
			{
			//待发送数据长度大于等于最大帧长度，分组发送，每组发送的数据长度为（FRAME_SIZE - 1）	
			tpdu.pSendBuffer[0]= CPU_CARD.PCB | 0x10 ;//PCB字节写入TPDU数组,设置链接标识
			memcpy(tpdu.pSendBuffer + 1,apdu->pSendBuffer + apdu->SendLength - unsent_length,FRAME_SIZE - 1);//APDU数据写入TPDU数组
			
			tpdu.SendLength = FRAME_SIZE ;//分组发送长度
			Uart_Send_Msg("..--> ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");		 
			result = Reader_TPDU(&tpdu);
			Uart_Send_Msg("<-- ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			 if((result != FM175XX_SUCCESS)&&(tpdu.ReceiveLength == 1)) 
				 return result; 
			 memcpy(&pcb_byte,tpdu.pReceiveBuffer,1);//
				if((pcb_byte == 0xA2)||(pcb_byte == 0xA3)) 
					{				
						unsent_length = unsent_length - (FRAME_SIZE - 1);//重新设置待发送数据长度	
						CPU_CARD.PCB = (pcb_byte & 0x0F) ^ 0x01;//重新设置PCB字节
						//Uart_Send_Msg("unsent_length = ");Uart_Send_Hex(&unsent_length,1);Uart_Send_Msg("\r\n");	
					} 
				else
				return FM175XX_COMM_ERROR;//				
			}
		}	
	for(i=0;i<255;i++)//接收流程，最多支持255次ACK或者WTX应答
	{
			if((result != FM175XX_SUCCESS)||(tpdu.ReceiveLength == 0)) 
				return FM175XX_COMM_ERROR;
			memcpy(&pcb_byte,tpdu.pReceiveBuffer,1);//获取PCB字节
			if((pcb_byte == 0x02)||(pcb_byte == 0x03))
			{
				CPU_CARD.PCB = pcb_byte ^ 0x01;//设置PCB字节
				memcpy(apdu->pReceiveBuffer + apdu->ReceiveLength,tpdu.pReceiveBuffer + 1,tpdu.ReceiveLength - 1);
				apdu->ReceiveLength = apdu->ReceiveLength + tpdu.ReceiveLength - 1;
				return FM175XX_SUCCESS;//完成数据接收流程
			}
			
			if((pcb_byte == 0x12)||(pcb_byte == 0x13))
			{
				memcpy(apdu->pReceiveBuffer + apdu->ReceiveLength,tpdu.pReceiveBuffer + 1,tpdu.ReceiveLength - 1);
				apdu->ReceiveLength = apdu->ReceiveLength + tpdu.ReceiveLength - 1;
				tpdu.pSendBuffer[0] = ((pcb_byte&0x03) |0xA0)^0x01;//收到连接帧，设置PCB字节
				tpdu.SendLength = 1;
				Uart_Send_Msg("...--> = ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");
				result = Reader_TPDU(&tpdu);//回发ACK
				Uart_Send_Msg("<-- = ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			}
			
			if((pcb_byte == 0xF2)||(pcb_byte == 0xF3))
			{
				tpdu.pSendBuffer[0] = pcb_byte;//收到WTX帧，设置PCB字节
				tpdu.SendLength = tpdu.ReceiveLength;
				memcpy(tpdu.pSendBuffer,tpdu.pReceiveBuffer,tpdu.ReceiveLength);
				Uart_Send_Msg("....--> = ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");
				result = Reader_TPDU(&tpdu);//回发WTX
				Uart_Send_Msg("<-- = ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			}
			
		}
	return FM175XX_COMM_ERROR;//接收错误	
}

