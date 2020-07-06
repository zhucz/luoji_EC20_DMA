#ifndef __USART2_H__
#define __USART2_H__

#include "sys.h"

#define USART2_RXMAX_LEN  1034

#define   MYDMA    0
/*
0x1: 连接建立，等待响应
0x2: 发送不定长模式，等待响应
0x4: 发送完请求等待响应
0x8：socket 断开等待响应
*/
typedef struct {
    uint8_t buf[USART2_RXMAX_LEN];  // 数据内容
    volatile char state;            // 当前状态 0x1 , 0x2, 0x4, 0x8
    volatile uint16_t idx;
}AT_DATA;


typedef struct gb_usart2{
	uint8_t  rxbuf[200];
	volatile uint16_t rxindex;
	volatile uint16_t txindex;
	volatile uint8_t  rx_done_flag;
	volatile uint8_t  rx_start;
    volatile uint8_t  type_t;
}GD_USART2;


void USART2_Init(u32 bound);


//初始化
//void _BSP_USARTDMA_Configuration(uint32_t bRate);
//使用DMA发送数据
void _BSP_UARTxTransmit(uint8_t *pData, uint16_t Size);


extern DMA_HandleTypeDef  USART2TxDMA_Handler;      //DMA句柄

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void MYDMA_Init(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
void MYDMA_Init_TEST(void);
#endif

