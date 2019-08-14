#include "usart2.h"
#include "sys.h"


u8 ii=0;
extern u8 receive_data[8];//followline����
//////////////////////////////////////////////////////////////////////////////////
// �޸�������ԭ�ӵķ�������
//////////////////////////////////////////////////////////////////////////////////  

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE
{
	int handle;

};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
_sys_exit(int x)
{
	x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
    USART_SendData(USART2,(uint8_t)ch);
	return ch;
}
#endif



#if EN_USART2_RX   //���ʹ���˽���
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART2_RX_BUF[30];     //���ջ���,���30���ֽ�.
u8 len=0;      //���ݵ���λ�жϱ��ν��ܵ�������
//����״̬
//bit15��	������ɱ�־
//bit14��	���ڽ��ձ�־
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���

//��ʼ��IO ����1
//bound:������
void uart2_init(u32 bound){
    //GPIO�˿�����
  	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART2��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 	USART_DeInit(USART2);  //��λ����1
	 //USART2_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9

    //USART2_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
#if EN_USART2_RX		  //���ʹ���˽���
   //USART2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
#endif
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���

}

void USART2_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{

		Res = (u8)USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		if(Res==0xAA&&USART2_RX_STA==0)  //�ж�һ�δ���Ŀ�ʼ
		{
			USART2_RX_STA|=0x4000;      //����״̬
			USART2_RX_BUF[USART2_RX_STA&0X00FF]=Res ;

		}
		if(USART2_RX_STA)
		{
			USART2_RX_BUF[USART2_RX_STA&0X00FF]=Res ;

			if((USART2_RX_STA&0x00FF)==(len+3))
				USART2_RX_STA|=0x8000;      //�������
		}
		if((USART2_RX_STA&0x00FF)==2)
			len=USART2_RX_BUF[2];        //��ý��ճ���

		USART2_RX_STA++;


	}
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();
#endif
}

void uart2_send_mydata(u8 *data,u8 len)
{
	int i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART2,data[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}

void uart2_receive_mydata(u8 *data,u8 *len)
{
	u8 i=0;
	*len = (u8)(USART2_RX_STA&0xFF);
	for(i=0;i<*len;i++)
		data[i]=USART2_RX_BUF[i];


}

#endif	






