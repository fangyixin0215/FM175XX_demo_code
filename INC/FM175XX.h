
#ifndef _FM175XX_H
#define _FM175XX_H
#include "stm32f10x_map.h"

#define TX1_TX2_CW_DISABLE 0
#define TX1_CW_ENABLE 1
#define TX2_CW_ENABLE 2
#define TX1_TX2_CW_ENABLE 3
#define FM175XX_SUCCESS				0x00
#define FM175XX_FIFO_ERROR		0xC1
#define FM175XX_RESET_ERROR		0xC2

extern void FM175XX_HardReset(void);
extern unsigned char FM175XX_SoftReset(void);
extern void FM175XX_HPD(unsigned char mode);
extern void GetReg(unsigned char reg_address,unsigned char* reg_data);
extern void SetReg(unsigned char reg_address,unsigned char reg_data);
extern void ModifyReg(unsigned char reg_address,unsigned char mask,unsigned char set);
extern void SetCW(unsigned char mode);
extern unsigned char ReadFIFOData(unsigned char *fifo_data,unsigned char fifo_data_length);
extern unsigned char WriteFIFOData(unsigned char *fifo_data,unsigned char fifo_data_length);

#endif





