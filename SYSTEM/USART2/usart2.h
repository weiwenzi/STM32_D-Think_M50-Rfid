#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////
// �޸�������ԭ�ӵķ�������
//////////////////////////////////////////////////////////////////////////////////  
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART2_RX_BUF[30]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART2_RX_STA;         		//����״̬���
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart2_init(u32 bound);
void uart2_send_mydata(u8 *data,u8 len);
void uart2_receive_mydata(u8 *data,u8 *len);

#endif


