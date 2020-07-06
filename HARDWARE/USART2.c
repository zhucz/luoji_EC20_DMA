/*******************************************************************************
 * 版权说明： Copyright (c) 2020.06
 *
 * 文 件 名： USART2.c
 * 作    者： 282081226@126.com 
 * 创建日期： 2020年06月18日
 * 文件描述： USART2串口初始化
 *
 * 修改日志： 
 * 版本号        日期        作者        说明
 * v0.1          2020-06-18  tigers       创建文件
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
UART_HandleTypeDef 	        USART2_Handler; 	//UART句柄  USART2_Handler

//extern OS_EVENT             *usart2_q;
extern u8                   LTE4G_2RX_CNT;
extern TIM_HandleTypeDef    TIM2_Handler;       //定时器句柄 


/*===================================
* @brief  : USART2串口初始化
* @param  : bound 波特率
* @retval : void
===================================*/
#if 1
void USART2_Init(u32 bound)
{
	//GPIO端口设置
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

	//USART2 初始化设置
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//波特率
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()会使能USART2
		
	__HAL_UART_DISABLE_IT(&USART2_Handler,UART_IT_TC);
//    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_ERR);//开启错误无中断
	__HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);	//开启接收中断
	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //使能USART2中断
	HAL_NVIC_SetPriority(USART2_IRQn,1,3);			    //抢占优先级3，子优先级3
}
#else
void USART2_Init(u32 bound)
{
	//GPIO端口设置
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

	//USART2 初始化设置
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//波特率
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()会使能USART2
		
	__HAL_UART_DISABLE_IT(&USART2_Handler,UART_IT_TC);
//    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_ERR);//开启错误无中断
	__HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);	//开启接收中断
	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //使能USART2中断
	HAL_NVIC_SetPriority(USART2_IRQn,1,3);			    //抢占优先级3，子优先级3
}
#endif

/*===================================
* @brief  : USART2 串口中断服务
* @param  : void
* @retval : void
===================================*/

volatile uint8_t type_ec20 = 0;
//extern OS_FLAG_GRP *at_flag;


void USART2_IRQHandler(void)
{   
	uint8_t res;
//    INT8U err;

    if((__HAL_UART_GET_FLAG(&USART2_Handler,UART_FLAG_RXNE) != RESET)) {  //接收中断
        HAL_UART_Receive(&USART2_Handler, &res,1,0x1FFFF);
        
        if(type_ec20 == 1){
            //处理EC20数据的发送
            if(at_data.idx < 1000){
                at_data.buf[at_data.idx++] = res;
            }else{
                at_data.idx = 0;
            }
            
        }else{
            //开启定时器2
            //处理EC20初始化
//            TIM2_Handler.Instance->CNT = 0;       //计数器清理
//            HAL_TIM_Base_Start_IT(&TIM2_Handler); //使能定时器2和定时器2中断 
//            
//            lte4g.rx_start = 1;			          //标记开始接受
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
//DMA方式
#include "usart2.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
//#include "includes.h"
#include "sys.h"

AT_DATA at_data;
volatile int http_flag = 0;

GD_USART2			        lte4g;
UART_HandleTypeDef 	        USART2_Handler; 	//UART句柄  USART2_Handler

//extern OS_EVENT             *usart2_q;
extern u8                   LTE4G_2RX_CNT;
extern TIM_HandleTypeDef    TIM2_Handler;       //定时器句柄 



DMA_InitTypeDef DMA_InitStructure;

#define RXBUF_SIZE  200               //接收缓存大小
//uint8_t dma_rx_buf[RXBUF_SIZE];

DMA_HandleTypeDef  USART2TxDMA_Handler;      //DMA句柄
DMA_HandleTypeDef  USART2RxDMA_Handler;      //DMA句柄


#if 1
void USART2_Init(u32 bound)
{
	//GPIO端口设置
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

	//USART2 初始化设置
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//波特率
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()会使能USART2
		
//	__HAL_UART_DISABLE_IT(&USART2_Handler,UART_IT_TC);
////    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_ERR);//开启错误无中断
//	__HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);	//开启接收中断
//	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //使能USART2中断
//	HAL_NVIC_SetPriority(USART2_IRQn,1,3);			    //抢占优先级3，子优先级3
}
#else
void USART2_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStruct;
	
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    __HAL_RCC_DMA1_IS_CLK_ENABLED();//DMA1时钟使能  DMA1_Stream5 : RX ,  DMA1_Stream6 : TX
    
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

	//USART2 初始化设置
	USART2_Handler.Instance=USART2;			        	//USART2
	USART2_Handler.Init.BaudRate=bound;		        	//波特率
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&USART2_Handler);			        	//HAL_UART_Init()会使能USART2

}
#endif


//开启一次DMA传输
//huart:串口句柄
//pData：传输的数据指针
//Size:传输的数据量
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
//    HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
//    HAL_DMA_Start   (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
    
//    HAL_DMA_Start(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size);//开启DMA传输
    
    HAL_DMA_Start_IT(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size);//开启DMA传输
    huart->Instance->CR3 |= USART_CR3_DMAT;//使能串口DMA发送
}	  





volatile uint8_t rx_ok = 0;
volatile uint16_t rx_cnt = 0;
/*===================================
* @brief  : USART2串口初始化
* @param  : bound 波特率
* @retval : void
===================================*/


void MYDMA_Init_TEST(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
    __HAL_LINKDMA(&USART2_Handler,hdmatx,USART2TxDMA_Handler);    //将DMA与USART2联系起来(发送DMA)
    
//Tx DMA配置
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
    
//    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //开启DMA传输完成中断
    


    //Rx DMA配置
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
    
//    __HAL_DMA_ENABLE_IT(&USART2RxDMA_Handler, DMA_IT_TC);     //开启DMA传输完成中断
    
    
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);  //抢占优先级0，子优先级0
    HAL_NVIC_EnableIRQ(USART2_IRQn);          //使能USART1中断通道
    
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 3, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 3, 2);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    

    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_IDLE);      //开启IDLE中断，以帧方式接收不定长数据
    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //开启DMA传输完成中断
    __HAL_DMA_ENABLE_IT(&USART2RxDMA_Handler, DMA_IT_TC);     //开启DMA传输完成中断
    HAL_UART_Receive_DMA(&USART2_Handler, at_data.buf, RXBUF_SIZE); 

}

//DMA发送中断处理   
void DMA1_Stream6_IRQHandler(void)    
{
  //DMA1_Stream6传输完成
  if(__HAL_DMA_GET_FLAG(&USART2TxDMA_Handler, DMA_FLAG_TCIF2_6)) 
  {
    //清除DMA1_Stream6传输完成标志
    __HAL_DMA_CLEAR_FLAG(&USART2TxDMA_Handler, DMA_FLAG_TCIF2_6);
    //传输完成以后关闭串口DMA
    HAL_UART_DMAStop(&USART2_Handler);      
  }
  //调用中断处理公共函数
  HAL_DMA_IRQHandler(&USART2TxDMA_Handler);
}

//DMA接收中断处理
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

//USART2中断函数
void USART2_IRQHandler(void)
{
  //当接收完一帧数据后标记接收完成
  if(__HAL_UART_GET_FLAG(&USART2_Handler, UART_FLAG_IDLE) != RESET)  //判断是否为IDLE中断  
  {
    __HAL_UART_CLEAR_IDLEFLAG(&USART2_Handler);
    //总计数减去DMA中未传输的数据个数(最多65535)
    at_data.idx = RXBUF_SIZE - USART2RxDMA_Handler.Instance->NDTR;
    rx_ok = 1;      //标记接收完成
  }
  
  HAL_UART_IRQHandler(&USART2_Handler);
}































///*===================================
//* @brief  : USART2 串口中断服务
//* @param  : void
//* @retval : void
//===================================*/

//volatile uint8_t type_ec20 = 0;
////extern OS_FLAG_GRP *at_flag;


////void USART2_IRQHandler(void)
////{   
////	uint8_t res;
//////    INT8U err;

////    if((__HAL_UART_GET_FLAG(&USART2_Handler,UART_FLAG_RXNE) != RESET)) {  //接收中断
////        HAL_UART_Receive(&USART2_Handler, &res,1,0x1FFFF);
////        
////        if(type_ec20 == 1){
////            //处理EC20数据的发送
////            if(at_data.idx < 1000){
////                at_data.buf[at_data.idx++] = res;
////            }else{
////                at_data.idx = 0;
////            }
////            
////        }else{
////            //开启定时器2
////            //处理EC20初始化
//////            TIM2_Handler.Instance->CNT = 0;       //计数器清理
//////            HAL_TIM_Base_Start_IT(&TIM2_Handler); //使能定时器2和定时器2中断 
//////            
//////            lte4g.rx_start = 1;			          //标记开始接受
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






////DMAx的各通道配置
////这里的传输形式是固定的,这点要根据不同的情况来修改
////从存储器->外设模式/8位数据宽度/存储器增量模式
////DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
////chx:DMA通道选择,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
//void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
//{ 
//    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
//    __HAL_LINKDMA(&USART2_Handler,hdmatx,USART2TxDMA_Handler);    //将DMA与USART2联系起来(发送DMA)
//    
//    //Tx DMA配置
//    USART2TxDMA_Handler.Instance=DMA_Streamx;                            //数据流选择
//    USART2TxDMA_Handler.Init.Channel=chx;                                //通道选择
//    USART2TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //存储器到外设
//    USART2TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
//    USART2TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
//    USART2TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
//    USART2TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
//    USART2TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //外设普通模式
//    USART2TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
//    USART2TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
//    USART2TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
//    USART2TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
//    USART2TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
//    
//    
//    HAL_DMA_DeInit(&USART2TxDMA_Handler);   
//    HAL_DMA_Init(&USART2TxDMA_Handler);
//    
//    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //开启DMA传输完成中断
//} 


//void MYDMA_Init(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
//{
//    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
//    __HAL_LINKDMA(&USART2_Handler,hdmatx,USART2TxDMA_Handler);    //将DMA与USART2联系起来(发送DMA)
//    
//    //Tx DMA配置
//    USART2TxDMA_Handler.Instance=DMA_Streamx;                            //数据流选择
//    USART2TxDMA_Handler.Init.Channel=chx;                                //通道选择
//    USART2TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //存储器到外设
//    USART2TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
//    USART2TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
//    USART2TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
//    USART2TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
//    USART2TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //外设普通模式
//    USART2TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
//    USART2TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
//    USART2TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
//    USART2TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
//    USART2TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
//    
//    
//    HAL_DMA_DeInit(&USART2TxDMA_Handler);   
//    HAL_DMA_Init(&USART2TxDMA_Handler);
//    
//    __HAL_DMA_ENABLE_IT(&USART2TxDMA_Handler, DMA_IT_TC);     //开启DMA传输完成中断
//    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 3, 2);
//    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);





//////Rx DMA配置
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

////  __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_IDLE);   //开启IDLE中断，以帧方式接收不定长数据
////  __HAL_DMA_ENABLE_IT(&USART2_DMA_TX, DMA_IT_TC);     //开启DMA传输完成中断
////  __HAL_DMA_ENABLE_IT(&USART2_DMA_RX, DMA_IT_TC);
////  HAL_UART_Receive_DMA(&USART2_Handler, dma_rx_buf, RXBUF_SIZE);
////  
////  
////    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);  //抢占优先级0，子优先级0
////    HAL_NVIC_EnableIRQ(USART2_IRQn);          //使能USART1中断通道
////    
////    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 3, 2);
////    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
////    
////    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 3, 1);
////    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
//    

// 
//}

