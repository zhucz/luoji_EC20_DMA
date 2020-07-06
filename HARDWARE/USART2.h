#ifndef __USART2_H__
#define __USART2_H__

#include "sys.h"

#define USART2_RXMAX_LEN  1034

#define   MYDMA    0
/*
0x1: ���ӽ������ȴ���Ӧ
0x2: ���Ͳ�����ģʽ���ȴ���Ӧ
0x4: ����������ȴ���Ӧ
0x8��socket �Ͽ��ȴ���Ӧ
*/
typedef struct {
    uint8_t buf[USART2_RXMAX_LEN];  // ��������
    volatile char state;            // ��ǰ״̬ 0x1 , 0x2, 0x4, 0x8
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


//��ʼ��
//void _BSP_USARTDMA_Configuration(uint32_t bRate);
//ʹ��DMA��������
void _BSP_UARTxTransmit(uint8_t *pData, uint16_t Size);


extern DMA_HandleTypeDef  USART2TxDMA_Handler;      //DMA���

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void MYDMA_Init(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
void MYDMA_Init_TEST(void);
#endif

