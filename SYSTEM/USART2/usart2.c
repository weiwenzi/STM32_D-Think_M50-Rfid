#include "usart2.h"
#include "sys.h"


u8 ii=0;
extern u8 receive_data[8];//followline里面
//////////////////////////////////////////////////////////////////////////////////
// 修改了正点原子的范例代码
//////////////////////////////////////////////////////////////////////////////////  

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE
{
	int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
	x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
    USART_SendData(USART2,(uint8_t)ch);
	return ch;
}
#endif



#if EN_USART2_RX   //如果使能了接收
//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART2_RX_BUF[30];     //接收缓冲,最大30个字节.
u8 len=0;      //根据第三位判断本次接受的数据量
//接收状态
//bit15，	接收完成标志
//bit14，	正在接收标志
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记

//初始化IO 串口1
//bound:波特率
void uart2_init(u32 bound){
    //GPIO端口设置
  	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//使能USART2，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 	USART_DeInit(USART2);  //复位串口1
	 //USART2_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

    //USART2_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
#if EN_USART2_RX		  //如果使能了接收
   //USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
#endif
    USART_Cmd(USART2, ENABLE);                    //使能串口

}

void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{

		Res = (u8)USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		if(Res==0xAA&&USART2_RX_STA==0)  //判断一次传输的开始
		{
			USART2_RX_STA|=0x4000;      //接收状态
			USART2_RX_BUF[USART2_RX_STA&0X00FF]=Res ;

		}
		if(USART2_RX_STA)
		{
			USART2_RX_BUF[USART2_RX_STA&0X00FF]=Res ;

			if((USART2_RX_STA&0x00FF)==(len+3))
				USART2_RX_STA|=0x8000;      //接收完成
		}
		if((USART2_RX_STA&0x00FF)==2)
			len=USART2_RX_BUF[2];        //获得接收长度

		USART2_RX_STA++;


	}
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();
#endif
}

void uart2_send_mydata(u8 *data,u8 len)
{
	int i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART2,data[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
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






