 //2018��2��2�ձ༭��֧��CPU��Ƭָ����ӷ�ʽ����
 #include "FM175XX.h"
 #include "CPU_CARD.h"
 #include <string.h>
 #include "READER_API.h"
 #include "FM175XX_REG.h"
 #include "DEVICE_CFG.h"
 #include "UART.h"
 struct CPU_CARD_STR CPU_CARD;  

/****************************************************************/
/*����: Rats 																										*/
/*����: Request for answer to select														*/
/*����: 																												*/
/*																															*/
/*���:																													*/
/*		ats_len�����յ�ATS���ݳ���																*/
/*		*ats�����յ���ATS����ָ��																	*/
/* OK: Ӧ����ȷ						        															*/
/* ERROR: Ӧ�����																							*/
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
/*����: CPU_APDU 							*/
/*����: �ú���ʵ��ת��APDU�����ʽ����	CPU card reset		*/
/*����: 				*/
/*		*/
/*���:								*/
/*		ats_len�����յ����ݳ���			*/
/*		ats�����յ�����ָ��				*/
/* OK: Ӧ����ȷ						        */
/* ERROR: Ӧ�����						*/
/****************************************************************/
unsigned char CPU_APDU(transmission_struct *apdu)
{
	unsigned char result,pcb_byte,tpdu_buffer[64],unsent_length,i;
	transmission_struct tpdu;
	tpdu.pSendBuffer = tpdu_buffer;
	tpdu.pReceiveBuffer = tpdu_buffer;	
	tpdu.Timeout = 500;
	apdu->ReceiveLength = 0;
	unsent_length = apdu->SendLength;//���ô��������ݳ���

	for(i=0;i<16;i++)//�������̣�����С���鳤��16��������16���鷢��
	{
			//Uart_Send_Msg("unsent_length = ");Uart_Send_Hex(&unsent_length,1);Uart_Send_Msg("\r\n");	
			if(unsent_length < FRAME_SIZE)
			{		
			//���������ݳ���С�����֡���ȣ��������д���������
			tpdu.pSendBuffer[0] = CPU_CARD.PCB ;//PCB�ֽ�д��TPDU����
			memcpy(tpdu.pSendBuffer + 1,apdu->pSendBuffer + apdu->SendLength - unsent_length,unsent_length);//APDU����д��TPDU����
			tpdu.SendLength = unsent_length + 1;//���ͳ�������1
			Uart_Send_Msg("--> ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");		 
			result = Reader_TPDU(&tpdu);
			if((result != FM175XX_SUCCESS)||(tpdu.ReceiveLength == 0))  
				 return result;
			Uart_Send_Msg("<-- ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			unsent_length = 0;//���������ݳ�����0	
			break;//������ݷ������̺��˳�ѭ��
			}
			else
			{
			//���������ݳ��ȴ��ڵ������֡���ȣ����鷢�ͣ�ÿ�鷢�͵����ݳ���Ϊ��FRAME_SIZE - 1��	
			tpdu.pSendBuffer[0]= CPU_CARD.PCB | 0x10 ;//PCB�ֽ�д��TPDU����,�������ӱ�ʶ
			memcpy(tpdu.pSendBuffer + 1,apdu->pSendBuffer + apdu->SendLength - unsent_length,FRAME_SIZE - 1);//APDU����д��TPDU����
			
			tpdu.SendLength = FRAME_SIZE ;//���鷢�ͳ���
			Uart_Send_Msg("..--> ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");		 
			result = Reader_TPDU(&tpdu);
			Uart_Send_Msg("<-- ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			 if((result != FM175XX_SUCCESS)&&(tpdu.ReceiveLength == 1)) 
				 return result; 
			 memcpy(&pcb_byte,tpdu.pReceiveBuffer,1);//
				if((pcb_byte == 0xA2)||(pcb_byte == 0xA3)) 
					{				
						unsent_length = unsent_length - (FRAME_SIZE - 1);//�������ô��������ݳ���	
						CPU_CARD.PCB = (pcb_byte & 0x0F) ^ 0x01;//��������PCB�ֽ�
						//Uart_Send_Msg("unsent_length = ");Uart_Send_Hex(&unsent_length,1);Uart_Send_Msg("\r\n");	
					} 
				else
				return FM175XX_COMM_ERROR;//				
			}
		}	
	for(i=0;i<255;i++)//�������̣����֧��255��ACK����WTXӦ��
	{
			if((result != FM175XX_SUCCESS)||(tpdu.ReceiveLength == 0)) 
				return FM175XX_COMM_ERROR;
			memcpy(&pcb_byte,tpdu.pReceiveBuffer,1);//��ȡPCB�ֽ�
			if((pcb_byte == 0x02)||(pcb_byte == 0x03))
			{
				CPU_CARD.PCB = pcb_byte ^ 0x01;//����PCB�ֽ�
				memcpy(apdu->pReceiveBuffer + apdu->ReceiveLength,tpdu.pReceiveBuffer + 1,tpdu.ReceiveLength - 1);
				apdu->ReceiveLength = apdu->ReceiveLength + tpdu.ReceiveLength - 1;
				return FM175XX_SUCCESS;//������ݽ�������
			}
			
			if((pcb_byte == 0x12)||(pcb_byte == 0x13))
			{
				memcpy(apdu->pReceiveBuffer + apdu->ReceiveLength,tpdu.pReceiveBuffer + 1,tpdu.ReceiveLength - 1);
				apdu->ReceiveLength = apdu->ReceiveLength + tpdu.ReceiveLength - 1;
				tpdu.pSendBuffer[0] = ((pcb_byte&0x03) |0xA0)^0x01;//�յ�����֡������PCB�ֽ�
				tpdu.SendLength = 1;
				Uart_Send_Msg("...--> = ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");
				result = Reader_TPDU(&tpdu);//�ط�ACK
				Uart_Send_Msg("<-- = ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			}
			
			if((pcb_byte == 0xF2)||(pcb_byte == 0xF3))
			{
				tpdu.pSendBuffer[0] = pcb_byte;//�յ�WTX֡������PCB�ֽ�
				tpdu.SendLength = tpdu.ReceiveLength;
				memcpy(tpdu.pSendBuffer,tpdu.pReceiveBuffer,tpdu.ReceiveLength);
				Uart_Send_Msg("....--> = ");Uart_Send_Hex(tpdu.pSendBuffer,tpdu.SendLength);Uart_Send_Msg("\r\n");
				result = Reader_TPDU(&tpdu);//�ط�WTX
				Uart_Send_Msg("<-- = ");Uart_Send_Hex(tpdu.pReceiveBuffer,tpdu.ReceiveLength);Uart_Send_Msg("\r\n");	
			}
			
		}
	return FM175XX_COMM_ERROR;//���մ���	
}

