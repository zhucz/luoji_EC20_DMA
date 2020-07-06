/*******************************************************************************
 * ��Ȩ˵���� Copyright (c) 2020.06
 *
 * �� �� ���� USART2.c
 * ��    �ߣ� 282081226@126.com 
 * �������ڣ� 2020��06��18��
 * �ļ������� USART2���ڳ�ʼ��
 *
 * �޸���־�� 
 * �汾��        ����        ����        ˵��
 * v0.1          2020-06-18  tigers       �����ļ�
*******************************************************************************/
#include "usart2.h"
#if MYDMA
//{
#include "usart2.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
//#include "includes.h"
#include "sys.h"

AT_DATA at_data;
volatile int http_flag = 0;

GD_USART2			        lte4g;
UART_HandleTypeDef 	        USART2_Handler; 	//UART���  USART2_Handler

//extern OS_EVENT             *usart2_q;
extern u8                   LTE4G_2RX_CNT;
extern TIM_HandleTypeDef    TIM2_Handler;       //��ʱ����� 


/*===================================
* @brief  : USART2���ڳ�ʼ��
* @param  : bound ������
* @retval : void
===================================*/
#if 1
void USART2_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStruct;
	
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//USART2 ��ʼ������
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//������
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//����żУ��λ
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()��ʹ��USART2
		
	__HAL_UART_DISABLE_IT(&USART2_Handler,UART_IT_TC);
//    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_ERR);//�����������ж�
	__HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);	//���������ж�
	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //ʹ��USART2�ж�
	HAL_NVIC_SetPriority(USART2_IRQn,1,3);			    //��ռ���ȼ�3�������ȼ�3
}
#else
void USART2_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStruct;
	
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	//USART2 ��ʼ������
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//������
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//����żУ��λ
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()��ʹ��USART2
		
	__HAL_UART_DISABLE_IT(&USART2_Handler,UART_IT_TC);
//    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_ERR);//�����������ж�
	__HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);	//���������ж�
	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //ʹ��USART2�ж�
	HAL_NVIC_SetPriority(USART2_IRQn,1,3);			    //��ռ���ȼ�3�������ȼ�3
}
#endif

/*===================================
* @brief  : USART2 �����жϷ���
* @param  : void
* @retval : void
===================================*/

volatile uint8_t type_ec20 = 0;
//extern OS_FLAG_GRP *at_flag;


void USART2_IRQHandler(void)
{   
	uint8_t res;
//    INT8U err;

    if((__HAL_UART_GET_FLAG(&USART2_Handler,UART_FLAG_RXNE) != RESET)) {  //�����ж�
        HAL_UART_Receive(&USART2_Handler, &res,1,0x1FFFF);
        
        if(type_ec20 == 1){
            //����EC20���ݵķ���
            if(at_data.idx < 1000){
                at_data.buf[at_data.idx++] = res;
            }else{
                at_data.idx = 0;
            }
            
        }else{
            //������ʱ��2
            //����EC20��ʼ��
//            TIM2_Handler.Instance->CNT = 0;       //����������
//            HAL_TIM_Base_Start_IT(&TIM2_Handler); //ʹ�ܶ�ʱ��2�Ͷ�ʱ��2�ж� 
//            
//            lte4g.rx_start = 1;			          //��ǿ�ʼ����
            if(lte4g.rxindex < 190) {	
                lte4g.rxbuf[lte4g.rxindex] = res;
                lte4g.rxindex++;						
            }else{
                lte4g.rxindex = 0;
            }
        }  
    }
} 
//}
#else
//DMA��ʽ
#include "usart2.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
//#include "includes.h"
#include "sys.h"

AT_DATA at_data;
volatile int http_flag = 0;

GD_USART2			        lte4g;
UART_HandleTypeDef 	        USART2_Handler; 	//UART���  USART2_Handler

//extern OS_EVENT             *usart2_q;
extern u8                   LTE4G_2RX_CNT;
extern TIM_HandleTypeDef    TIM2_Handler;       //��ʱ����� 



DMA_InitTypeDef DMA_InitStructure;

#define RXBUF_SIZE  200               //���ջ����С
//uint8_t dma_rx_buf[RXBUF_SIZE];

DMA_HandleTypeDef  USART2TxDMA_Handler;      //DMA���
DMA_HandleTypeDef  USART2RxDMA_Handler;      //DMA���


#if 1
void USART2_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStruct;
	
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//USART2 ��ʼ������
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//������
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//����żУ��λ
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()��ʹ��USART2
		
//	__HAL_UART_DISABLE_IT(&USART2_Handler,UART_IT_TC);
////    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_ERR);//�����������ж�
//	__HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);	//���������ж�
//	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //ʹ��USART2�ж�
//	HAL_NVIC_SetPriority(USART2_IRQn,1,3);			    //��ռ���ȼ�3�������ȼ�3
}
#else
void USART2_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStruct;
	
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    __HAL_RCC_DMA1_IS_CLK_ENABLED();//DMA1ʱ��ʹ��  DMA1_Stream5 : RX ,  DMA1_Stream6 : TX
    
    /**USART2 GPIO Configuration    
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	//USART2 ��ʼ������
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//������
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//����żУ��λ
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()��ʹ��USART2

}
#endif


//����һ��DMA����
//huart:���ھ��
//pData�����������ָ��
//Size:�����������
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
//    HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
//    HAL_DMA_Start   (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
    
//    HAL_DMA_Start(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size);//����DMA����
    
    HAL_DMA_Start_IT(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size);//����DMA����
    huart->Instance->CR3 |= USART_CR3_DMAT;//ʹ�ܴ���DMA����
}	  





volatile uint8_t rx_ok = 0;
volatile uint16_t rx_cnt = 0;
/*===================================
* @brief  : USART2���ڳ�ʼ��
* @param  : bound ������
* @retval : void
===================================*/


void MYDMA_Init_TEST(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
    __HAL_LINKDMA(&USART2_Handler,hdmatx,USART2TxDMA_Handler);    //��DMA��USART2��ϵ����(����DMA)
    
//Tx DMA����
    USART2TxDMA_Handler.Instance = (DMA_Stream_TypeDef *)DMA1_Stream6;
    USART2TxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
    USART2TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
    USART2TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART2TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART2TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART2TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART2TxDMA_Handler.Init.Mode = DMA_NORMAL;
    USART2TxDMA_Handler.Init.Priority = DMA_PRIORITY_MEDIUM;
    USART2TxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;              
    USART2TxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    USART2TxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
    USART2TxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;

    HAL_DMA_DeInit(&USART2TxDMA_Handler); 
    HAL_DMA_Init(&USART2TxDMA_Handler); 
    
//    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //����DMA��������ж�
    


    //Rx DMA����
    __HAL_LINKDMA(&USART2_Handler, hdmarx, USART2RxDMA_Handler);

    USART2RxDMA_Handler.Instance = (DMA_Stream_TypeDef *)DMA1_Stream5;;
    USART2RxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
    USART2RxDMA_Handler.Init.Direction = DMA_PERIPH_TO_MEMORY;
    USART2RxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART2RxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART2RxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART2RxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART2RxDMA_Handler.Init.Mode = DMA_NORMAL;
    USART2RxDMA_Handler.Init.Priority = DMA_PRIORITY_MEDIUM;
    USART2RxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    
    HAL_DMA_DeInit(&USART2RxDMA_Handler);
    HAL_DMA_Init(&USART2RxDMA_Handler);
    
//    __HAL_DMA_ENABLE_IT(&USART2RxDMA_Handler, DMA_IT_TC);     //����DMA��������ж�
    
    
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);  //��ռ���ȼ�0�������ȼ�0
    HAL_NVIC_EnableIRQ(USART2_IRQn);          //ʹ��USART1�ж�ͨ��
    
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 3, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 3, 2);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    

    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_IDLE);      //����IDLE�жϣ���֡��ʽ���ղ���������
    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //����DMA��������ж�
    __HAL_DMA_ENABLE_IT(&USART2RxDMA_Handler, DMA_IT_TC);     //����DMA��������ж�
    HAL_UART_Receive_DMA(&USART2_Handler, at_data.buf, RXBUF_SIZE); 

}

//DMA�����жϴ���   
void DMA1_Stream6_IRQHandler(void)    
{
  //DMA1_Stream6�������
  if(__HAL_DMA_GET_FLAG(&USART2TxDMA_Handler, DMA_FLAG_TCIF2_6)) 
  {
    //���DMA1_Stream6������ɱ�־
    __HAL_DMA_CLEAR_FLAG(&USART2TxDMA_Handler, DMA_FLAG_TCIF2_6);
    //��������Ժ�رմ���DMA
    HAL_UART_DMAStop(&USART2_Handler);      
  }
  //�����жϴ���������
  HAL_DMA_IRQHandler(&USART2TxDMA_Handler);
}

//DMA�����жϴ���
void DMA1_Stream5_IRQHandler(void)
{
  if(__HAL_DMA_GET_FLAG(&USART2RxDMA_Handler, DMA_FLAG_TCIF1_5)) 
  {
    __HAL_DMA_CLEAR_FLAG(&USART2RxDMA_Handler, DMA_FLAG_TCIF1_5);
    HAL_UART_DMAStop(&USART2_Handler);
  }

  HAL_UART_Receive_DMA(&USART2_Handler, at_data.buf, RXBUF_SIZE);
  HAL_DMA_IRQHandler(&USART2RxDMA_Handler);
}

//USART2�жϺ���
void USART2_IRQHandler(void)
{
  //��������һ֡���ݺ��ǽ������
  if(__HAL_UART_GET_FLAG(&USART2_Handler, UART_FLAG_IDLE) != RESET)  //�ж��Ƿ�ΪIDLE�ж�  
  {
    __HAL_UART_CLEAR_IDLEFLAG(&USART2_Handler);
    //�ܼ�����ȥDMA��δ��������ݸ���(���65535)
    at_data.idx = RXBUF_SIZE - USART2RxDMA_Handler.Instance->NDTR;
    rx_ok = 1;      //��ǽ������
  }
  
  HAL_UART_IRQHandler(&USART2_Handler);
}































///*===================================
//* @brief  : USART2 �����жϷ���
//* @param  : void
//* @retval : void
//===================================*/

//volatile uint8_t type_ec20 = 0;
////extern OS_FLAG_GRP *at_flag;


////void USART2_IRQHandler(void)
////{   
////	uint8_t res;
//////    INT8U err;

////    if((__HAL_UART_GET_FLAG(&USART2_Handler,UART_FLAG_RXNE) != RESET)) {  //�����ж�
////        HAL_UART_Receive(&USART2_Handler, &res,1,0x1FFFF);
////        
////        if(type_ec20 == 1){
////            //����EC20���ݵķ���
////            if(at_data.idx < 1000){
////                at_data.buf[at_data.idx++] = res;
////            }else{
////                at_data.idx = 0;
////            }
////            
////        }else{
////            //������ʱ��2
////            //����EC20��ʼ��
//////            TIM2_Handler.Instance->CNT = 0;       //����������
//////            HAL_TIM_Base_Start_IT(&TIM2_Handler); //ʹ�ܶ�ʱ��2�Ͷ�ʱ��2�ж� 
//////            
//////            lte4g.rx_start = 1;			          //��ǿ�ʼ����
////            if(lte4g.rxindex < 190) {	
////                lte4g.rxbuf[lte4g.rxindex] = res;
////                lte4g.rxindex++;						
////            }else{
////                lte4g.rxindex = 0;
////            }
////        }  
////    }
////} 

#endif






////DMAx�ĸ�ͨ������
////����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
////�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
////DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
////chx:DMAͨ��ѡ��,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
//void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
//{ 
//    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
//    __HAL_LINKDMA(&USART2_Handler,hdmatx,USART2TxDMA_Handler);    //��DMA��USART2��ϵ����(����DMA)
//    
//    //Tx DMA����
//    USART2TxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
//    USART2TxDMA_Handler.Init.Channel=chx;                                //ͨ��ѡ��
//    USART2TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������
//    USART2TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
//    USART2TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
//    USART2TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
//    USART2TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
//    USART2TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //������ͨģʽ
//    USART2TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
//    USART2TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
//    USART2TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
//    USART2TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
//    USART2TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
//    
//    
//    HAL_DMA_DeInit(&USART2TxDMA_Handler);   
//    HAL_DMA_Init(&USART2TxDMA_Handler);
//    
//    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //����DMA��������ж�
//} 


//void MYDMA_Init(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
//{
//    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
//    __HAL_LINKDMA(&USART2_Handler,hdmatx,USART2TxDMA_Handler);    //��DMA��USART2��ϵ����(����DMA)
//    
//    //Tx DMA����
//    USART2TxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
//    USART2TxDMA_Handler.Init.Channel=chx;                                //ͨ��ѡ��
//    USART2TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������
//    USART2TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
//    USART2TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
//    USART2TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
//    USART2TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
//    USART2TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //������ͨģʽ
//    USART2TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
//    USART2TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
//    USART2TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
//    USART2TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
//    USART2TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
//    
//    
//    HAL_DMA_DeInit(&USART2TxDMA_Handler);   
//    HAL_DMA_Init(&USART2TxDMA_Handler);
//    
//    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //����DMA��������ж�
//    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 3, 2);
//    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);





//////Rx DMA����
////    USART2_DMA_RX.Instance = DMA1_Stream5;
////    USART2_DMA_RX.Init.Channel = DMA_CHANNEL_4;
////    USART2_DMA_RX.Init.Direction = DMA_PERIPH_TO_MEMORY;
////    USART2_DMA_RX.Init.PeriphInc = DMA_PINC_DISABLE;
////    USART2_DMA_RX.Init.MemInc = DMA_MINC_ENABLE;
////    USART2_DMA_RX.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
////    USART2_DMA_RX.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
////    USART2_DMA_RX.Init.Mode = DMA_NORMAL;
////    USART2_DMA_RX.Init.Priority = DMA_PRIORITY_LOW;
////    USART2_DMA_RX.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
////    HAL_DMA_DeInit(&USART2_DMA_RX);
////    HAL_DMA_Init(&USART2_DMA_RX);
////    __HAL_LINKDMA(&USART2_Handler, hdmarx, USART2_DMA_RX);
////    

////  __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_IDLE);   //����IDLE�жϣ���֡��ʽ���ղ���������
////  __HAL_DMA_ENABLE_IT(&USART2_DMA_TX, DMA_IT_TC);     //����DMA��������ж�
////  __HAL_DMA_ENABLE_IT(&USART2_DMA_RX, DMA_IT_TC);
////  HAL_UART_Receive_DMA(&USART2_Handler, dma_rx_buf, RXBUF_SIZE);
////  
////  
////    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);  //��ռ���ȼ�0�������ȼ�0
////    HAL_NVIC_EnableIRQ(USART2_IRQn);          //ʹ��USART1�ж�ͨ��
////    
////    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 3, 2);
////    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
////    
////    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 3, 1);
////    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
//    

// 
//}

