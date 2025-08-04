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
/*名称：Mifare_Clear_Crypto																															 */
/*功能：Mifare_Clear_Crypto清除认证标志																 									*/
/*输入：																																								 */
/*																																						 					*/
/*输出:																																									 */
/*																																											 */
/*																																											 */
/*****************************************************************************************/							 
void Mifare_Clear_Crypto(void)
{
	ModifyReg(JREG_STATUS2,BIT3,RESET);
	return;
}	
/*****************************************************************************************/
/*名称：Mifare_Auth																		 */
/*功能：Mifare_Auth卡片认证																 */
/*输入：mode，认证模式（0：key A认证，1：key B认证）；sector，认证的扇区号（0~15）		 */
/*		*mifare_key，6字节认证密钥数组；*card_uid，4字节卡片UID数组						 */
/*输出:																					 */
/*		FM175XX_SUCCESS    :认证成功																	 */
/*		FM175XX_AUTH_ERR :认证失败																	 */
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
		SetReg(JREG_FIFODATA,0x60);//KEY A 认证
	if(mode == KEY_B_AUTH)
		SetReg(JREG_FIFODATA,0x61);//KEY B 认证
	SetReg(JREG_FIFODATA,sector * 4);//认证扇区
	SetReg(JREG_FIFODATA,mifare_key[0]);//认证密钥
	SetReg(JREG_FIFODATA,mifare_key[1]);//认证密钥
	SetReg(JREG_FIFODATA,mifare_key[2]);//认证密钥
	SetReg(JREG_FIFODATA,mifare_key[3]);//认证密钥
	SetReg(JREG_FIFODATA,mifare_key[4]);//认证密钥
	SetReg(JREG_FIFODATA,mifare_key[5]);//认证密钥
	SetReg(JREG_FIFODATA,PICC_A.UID[0]);//认证卡片UID
	SetReg(JREG_FIFODATA,PICC_A.UID[1]);//认证卡片UID
	SetReg(JREG_FIFODATA,PICC_A.UID[2]);//认证卡片UID
	SetReg(JREG_FIFODATA,PICC_A.UID[3]);//认证卡片UID
	
	SetReg(JREG_COMMAND,CMD_AUTHENT);//command = Transceive
	SetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	GetReg(JREG_STATUS2,&reg_data);
	if(reg_data & JBIT_CRYPTO1ON)
				return FM175XX_SUCCESS;
	return FM175XX_AUTH_ERROR;			
}
/*****************************************************************************************/
/*名称：Mifare_Blockset									 */
/*功能：Mifare_Blockset卡片数值设置							 */
/*输入：block_num，块号；*buff，需要设置的4字节数值数组					 */
/*											 */
/*输出:											 */
/*		FM175XX_SUCCESS    :设置成功								 */
/*		FM175XX_COMM_ERR :设置失败								 */
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
/*名称：Mifare_Blockread																 */
/*功能：Mifare_Blockread卡片读块操作													 */
/*输入：block_num，块号（0x00~0x3F）；buff，16字节读块数据数组								 */
/*输出:																					 */
/*		FM175XX_SUCCESS    :成功																		 */
/*		FM175XX_COMM_ERR :失败																		 */
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
/*名称：mifare_blockwrite																 */
/*功能：Mifare卡片写块操作																 */
/*输入：block_num，块号（0x00~0x3F）；buff，16字节写块数据数组								 */
/*输出:																					 */
/*		FM175XX_SUCCESS    :成功																		 */
/*		FM175XX_COMM_ERR :失败																		 */
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
/*名称：																				 */
/*功能：Mifare 卡片增值操作																 */
/*输入：block_num，块号（0x00~0x3F）；buff，4字节增值数据数组								 */
/*输出:																					 */
/*		FM175XX_SUCCESS    :成功																		 */
/*		FM175XX_COMM_ERR :失败																		 */
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
/*名称：mifare_blockdec																	 */
/*功能：Mifare 卡片减值操作																 */
/*输入：block_num，块号（0x00~0x3F）；buff，4字节减值数据数组								 */
/*输出:																					 */
/*		FM175XX_SUCCESS    :成功																		 */
/*		FM175XX_COMM_ERR :失败																		 */
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
/*名称：mifare_transfer																	 */
/*功能：Mifare 卡片transfer操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		FM175XX_SUCCESS    :成功																		 */
/*		FM175XX_COMM_ERR :失败																		 */
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
/*名称：mifare_restore																	 */
/*功能：Mifare 卡片restore操作															 */
/*输入：block_num，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		FM175XX_SUCCESS    :成功																		 */
/*		FM175XX_COMM_ERR :失败																		 */
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
