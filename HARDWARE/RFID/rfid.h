#ifndef __RFID_H
#define __RFID_H	 
#include "sys.h"

// 接入usart1 的 RFID
u8 RFID_Init1(void);// 初始化读卡器，使能天线
u8 GET_UID1(u8 *uid);//获取UID
u8 RFID_READ1(u8 *uid,u8 *block);// 读取rfid第一块数据
u8 RFID_DisableAntenna1(void);// 关闭天线，减少发热

// 接入usart2 的 RFID
u8 RFID_Init2(void);// 初始化读卡器，使能天线
u8 GET_UID2(u8 *uid);//获取UID
u8 RFID_READ2(u8 *uid,u8 *block);// 读取rfid第一块数据
u8 RFID_DisableAntenna2(void);// 关闭天线，减少发热
		 				    
#endif