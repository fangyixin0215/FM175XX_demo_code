#include "MIFARE.h"
#include "FM175XX.h"
#include "Reader_API.h"
#include "string.h"
#include "FM175XX_REG.h"
#include "DEVICE_CFG.h"
unsigned char SECTOR,BLOCK,BLOCK_NUM;
unsigned char BLOCK_DATA[16];
unsigned char BLOCK_VAULE[4];
unsigned char INCREMENT_VALUE[4];
unsigned char DECREMENT_VALUE[4];
unsigned char KEY_A[16][6]=
							{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//0
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//2
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//3
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//4
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//5
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//6
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//7
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//10
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//11
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//12
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//13
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//14
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};//15
							 
unsigned char KEY_B[16][6]=
							{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//0
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//2
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//3
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//4
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//5
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//6
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//7
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//10
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//11
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//12
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//13
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//14
							 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};//15
							
/*****************************************************************************************/
/*���ƣ�Mifare_Clear_Crypto																															 */
/*���ܣ�Mifare_Clear_Crypto�����֤��־																 									*/
/*���룺																																								 */
/*																																						 					*/
/*���:																																									 */
/*																																											 */
/*																																											 */
/*****************************************************************************************/							 
void Mifare_Clear_Crypto(void)
{
	ModifyReg(JREG_STATUS2,BIT3,RESET);
	return;
}	
/*****************************************************************************************/
/*���ƣ�Mifare_Auth																		 */
/*���ܣ�Mifare_Auth��Ƭ��֤																 */
/*���룺mode����֤ģʽ��0��key A��֤��1��key B��֤����sector����֤�������ţ�0~15��		 */
/*		*mifare_key��6�ֽ���֤��Կ���飻*card_uid��4�ֽڿ�ƬUID����						 */
/*���:																					 */
/*		FM175XX_SUCCESS    :��֤�ɹ�																	 */
/*		FM175XX_AUTH_ERR :��֤ʧ��																	 */
/*****************************************************************************************/
 unsigned char Mifare_Auth(unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{		
	unsigned char reg_data;
	SetReg(JREG_STATUS2,0);	//clear Crypto1On
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x80);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	if(mode == KEY_A_AUTH)
		SetReg(JREG_FIFODATA,0x60);//KEY A ��֤
	if(mode == KEY_B_AUTH)
		SetReg(JREG_FIFODATA,0x61);//KEY B ��֤
	SetReg(JREG_FIFODATA,sector * 4);//��֤����
	SetReg(JREG_FIFODATA,mifare_key[0]);//��֤��Կ
	SetReg(JREG_FIFODATA,mifare_key[1]);//��֤��Կ
	SetReg(JREG_FIFODATA,mifare_key[2]);//��֤��Կ
	SetReg(JREG_FIFODATA,mifare_key[3]);//��֤��Կ
	SetReg(JREG_FIFODATA,mifare_key[4]);//��֤��Կ
	SetReg(JREG_FIFODATA,mifare_key[5]);//��֤��Կ
	SetReg(JREG_FIFODATA,PICC_A.UID[0]);//��֤��ƬUID
	SetReg(JREG_FIFODATA,PICC_A.UID[1]);//��֤��ƬUID
	SetReg(JREG_FIFODATA,PICC_A.UID[2]);//��֤��ƬUID
	SetReg(JREG_FIFODATA,PICC_A.UID[3]);//��֤��ƬUID
	
	SetReg(JREG_COMMAND,CMD_AUTHENT);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_STATUS2,&reg_data);
	if(reg_data & JBIT_CRYPTO1ON)
				return FM175XX_SUCCESS;
	return FM175XX_AUTH_ERROR;			
}
/*****************************************************************************************/
/*���ƣ�Mifare_Blockset									 */
/*���ܣ�Mifare_Blockset��Ƭ��ֵ����							 */
/*���룺block_num����ţ�*buff����Ҫ���õ�4�ֽ���ֵ����					 */
/*											 */
/*���:											 */
/*		FM175XX_SUCCESS    :���óɹ�								 */
/*		FM175XX_COMM_ERR :����ʧ��								 */
/*****************************************************************************************/
 unsigned char Mifare_Blockset(unsigned char block_num,unsigned char *data_buff)
 {
  unsigned char block_data[16],result;
	block_data[0] = data_buff[0];
	block_data[1] = data_buff[1];
	block_data[2] = data_buff[2];
	block_data[3] = data_buff[3];
	block_data[4] = ~data_buff[0];
	block_data[5] = ~data_buff[1];
	block_data[6] = ~data_buff[2];
	block_data[7] = ~data_buff[3];
  block_data[8] = data_buff[0];
	block_data[9] = data_buff[1];
	block_data[10] = data_buff[2];
	block_data[11] = data_buff[3];
	block_data[12] = block_num;
	block_data[13] = ~block_num;
	block_data[14] = block_num;
	block_data[15] = ~block_num;
  result = Mifare_Blockwrite(block_num,block_data);
  return result;
 }
/*****************************************************************************************/
/*���ƣ�Mifare_Blockread																 */
/*���ܣ�Mifare_Blockread��Ƭ�������													 */
/*���룺block_num����ţ�0x00~0x3F����buff��16�ֽڶ�����������								 */
/*���:																					 */
/*		FM175XX_SUCCESS    :�ɹ�																		 */
/*		FM175XX_COMM_ERR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block_num,unsigned char *data_buff)
{	
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x80);//Enable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0x30);
	SetReg(JREG_FIFODATA,block_num);
	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 16)
	{
		for(i=0;i<16;i++)
		GetReg(JREG_FIFODATA,&data_buff[i]);		
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
	
}
/*****************************************************************************************/
/*���ƣ�mifare_blockwrite																 */
/*���ܣ�Mifare��Ƭд�����																 */
/*���룺block_num����ţ�0x00~0x3F����buff��16�ֽ�д����������								 */
/*���:																					 */
/*		FM175XX_SUCCESS    :�ɹ�																		 */
/*		FM175XX_COMM_ERR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block_num,unsigned char *data_buff)
{	
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x00);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0xA0);
	SetReg(JREG_FIFODATA,block_num);	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		GetReg(JREG_FIFODATA,&reg_data);
		if(reg_data != 0x0A)
			return FM175XX_COMM_ERROR;
	}
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	for(i=0;i<16;i++)
		SetReg(JREG_FIFODATA,data_buff[i]);
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		GetReg(JREG_FIFODATA,&reg_data);
		if(reg_data == 0x0A)
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;		
}

/*****************************************************************************************/
/*���ƣ�																				 */
/*���ܣ�Mifare ��Ƭ��ֵ����																 */
/*���룺block_num����ţ�0x00~0x3F����buff��4�ֽ���ֵ��������								 */
/*���:																					 */
/*		FM175XX_SUCCESS    :�ɹ�																		 */
/*		FM175XX_COMM_ERR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block_num,unsigned char *data_buff)
{		
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x00);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0xC1);
	SetReg(JREG_FIFODATA,block_num);	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		GetReg(JREG_FIFODATA,&reg_data);
		if(reg_data != 0x0A)
			return FM175XX_COMM_ERROR;
	}
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	for(i=0;i<4;i++)
		SetReg(JREG_FIFODATA,data_buff[i]);
	SetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transmit
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	
	return FM175XX_SUCCESS;	
}
/*****************************************************************************************/
/*���ƣ�mifare_blockdec																	 */
/*���ܣ�Mifare ��Ƭ��ֵ����																 */
/*���룺block_num����ţ�0x00~0x3F����buff��4�ֽڼ�ֵ��������								 */
/*���:																					 */
/*		FM175XX_SUCCESS    :�ɹ�																		 */
/*		FM175XX_COMM_ERR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block_num,unsigned char *data_buff)
{	
	
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x00);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0xC0);
	SetReg(JREG_FIFODATA,block_num);	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		GetReg(JREG_FIFODATA,&reg_data);
		if(reg_data != 0x0A)
			return FM175XX_COMM_ERROR;
	}
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	for(i=0;i<4;i++)
		SetReg(JREG_FIFODATA,data_buff[i]);
	SetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transmit
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	
	return FM175XX_SUCCESS;	
}
/*****************************************************************************************/
/*���ƣ�mifare_transfer																	 */
/*���ܣ�Mifare ��Ƭtransfer����															 */
/*���룺block����ţ�0x00~0x3F��														 */
/*���:																					 */
/*		FM175XX_SUCCESS    :�ɹ�																		 */
/*		FM175XX_COMM_ERR :ʧ��																		 */
/*****************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block_num)
{		
	unsigned char reg_data;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x00);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0xB0);
	SetReg(JREG_FIFODATA,block_num);	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		GetReg(JREG_FIFODATA,&reg_data);
		if(reg_data == 0x0A)
			return FM175XX_SUCCESS;
	}
	return FM175XX_COMM_ERROR;	
}
/*****************************************************************************************/
/*���ƣ�mifare_restore																	 */
/*���ܣ�Mifare ��Ƭrestore����															 */
/*���룺block_num����ţ�0x00~0x3F��														 */
/*���:																					 */
/*		FM175XX_SUCCESS    :�ɹ�																		 */
/*		FM175XX_COMM_ERR :ʧ��																		 */
/*****************************************************************************************/

unsigned char Mifare_Restore(unsigned char block_num)
{	
	unsigned char reg_data,i;
	SetReg(JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(JREG_RXMODE,0x00);//Disable RxCRC
	SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(JREG_FIFODATA,0xC2);
	SetReg(JREG_FIFODATA,block_num);	
	SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		GetReg(JREG_FIFODATA,&reg_data);
		if(reg_data != 0x0A)
			return FM175XX_COMM_ERROR;
	}
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	for(i=0;i<4;i++)
		SetReg(JREG_FIFODATA,0x00);
	SetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transmit
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data != 0)
			return FM175XX_COMM_ERROR;
	return FM175XX_SUCCESS;
	
}
