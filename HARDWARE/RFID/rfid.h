#ifndef __RFID_H
#define __RFID_H
#include "sys.h"

// ����usart1 �� RFID
u8 RFID_Init1(void);// ��ʼ����������ʹ������
u8 GET_UID1(u8 *uid);//��ȡUID
u8 RFID_READ1(u8 *uid,u8 *block);// ��ȡrfid��һ������
u8 RFID_DisableAntenna1(void);// �ر����ߣ����ٷ���

// ����usart2 �� RFID
u8 RFID_Init2(void);// ��ʼ����������ʹ������
u8 GET_UID2(u8 *uid);//��ȡUID
u8 RFID_READ2(u8 *uid,u8 *block);// ��ȡrfid��һ������
u8 RFID_DisableAntenna2(void);// �ر����ߣ����ٷ���

#endif