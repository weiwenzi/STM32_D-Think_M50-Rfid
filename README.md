STM32_D-Think_M50-Rfid
====================
Use STM32 to control D-Think_M50 model to read rfid cards

使用STM32的usart串口控制RFID模块（D-Think_M50）进行读写

使用时请将SYSTEM/USART, SYSTEM/USART2复制到工程SYSTEM文件夹中（用于控制串口USART和USART2）
HARDWARE/RFID 复制到工程HARDWARE文件夹中

# HARDWARE/RFID 程序清单
## 接入USART1的RFID芯片
### u8 RFID_Init1(void);
初始化读卡器并使能天线，使用前请务必确认USART端口已经被初始化(至少调用过一次SYSTEM/USART/usart.h::uart_init(u32 bound))

返回值：0为正常初始化，1为出错

### u8 GET_UID1(u8 *uid);
获取读卡器UID并记录于uid中

返回值：0为正常，1为出错

### u8 RFID_READ1(u8 *uid, u8 *block);
读取rfid第一块数据，读取结果保存在block中

返回值：0为正常，1为出错

### u8 RFID_DisableAntenna1(void);
关闭天线，减少发热

返回值：0为正常，1为出错

### 接入USART2的RFID芯片
### u8 RFID_Init2(void);
初始化读卡器并使能天线，使用前请务必确认USART2端口已经被初始化(至少调用过一次SYSTEM/USART2/usart2.h::uart2_init(u32 bound))

返回值：0为正常初始化，1为出错

### u8 GET_UID2(u8 *uid);
获取读卡器UID并记录于uid中

返回值：0为正常，1为出错

### u8 RFID_READ2(u8 *uid, u8 *block);
读取rfid第一块数据，读取结果保存在block中

返回值：0为正常，1为出错

### u8 RFID_DisableAntenna2(void);
关闭天线，减少发热

返回值：0为正常，1为出错

