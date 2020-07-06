/*******************************************************************************
 * 版权说明： Copyright (c) 2020.06
 *
 * 文 件 名： EC20.c
 * 作    者： 282081226@126.com 
 * 创建日期： 2020年06月18日
 * 文件描述：4G模块初始化
 *
 * 修改日志： 
 * 版本号        日期        作者        说明
 * v0.1          2020-06-18  tigers       创建文件
*******************************************************************************/

#include "usart2.h"

#if MYDMA
#include "ec20.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
//#include "includes.h"
#include "usart2.h"
//#include "ec20_task.h"
#include "string.h"
    
#define  NEW_EC20_DEALY_TMIE    200

extern   GD_USART2	 			lte4g;
extern   UART_HandleTypeDef 	USART2_Handler; 	//UART句柄
//extern   OS_EVENT               *usart2_q;          //消息队列

uint8_t  mode_for_4g = 0;//1:中国移动，2联通，3电信
uint8_t  start_up_ok = 0;


extern volatile uint8_t type_ec20;

extern AT_DATA at_data;

void EC20_Rest_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOD_CLK_ENABLE();               			//开启GPIOD时钟

	GPIO_Initure.Pin   = GPIO_PIN_7; 						//PD7  4G RESET
    GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  				//推挽输出
    GPIO_Initure.Pull  = GPIO_PULLUP;          				//上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;      				//高速
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
//    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0置7
}

void EC20_RESET(void)
{
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);
    delay_ms(500);										    //延时10s
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0置7
    delay_ms(10000);//10S
}

void EC20_BOOT(void)
{
    uint8_t err = 0;
    uint8_t wait_cnt = 0;
    char *strx;
    uint8_t *msg_buf_boot;
    
    printf("EC20_BOOT Start \r\n");
    EC20_Rest_Init();

    
//  EC20_RESET();


    delay_ms(3000);
    memset(&lte4g.rxbuf[0],0,sizeof(GD_USART2));
    printf("%s \r\n", (char *)&lte4g.rxbuf[0]);

    EC20_Send_ATCMD((uint8_t *)AT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    strx = strstr((const char*)(char *)&lte4g.rxbuf[0],(const char*)"OK");//返回OK
    if(strx == NULL){
        while(wait_cnt <= 5){
            wait_cnt++;

            EC20_Send_ATCMD((uint8_t *)AT);
            delay_ms(NEW_EC20_DEALY_TMIE);
            strx = strstr((const char*)(char *)&lte4g.rxbuf[0],(const char*)"OK");//返回OK
            if(strx != NULL){
                printf("%s wait_cnt = %d \r\n", strx, wait_cnt);
                return;  
            }else{
                EC20_Send_ATCMD((uint8_t *)CFUN);
                delay_ms(3000);
                printf(" wait_cnt = %d \r\n", wait_cnt);
            }
        }
    } 
}


/*===================================
* @brief  : EC20   发送字符串
* @param  : *data  要发送的数据
* @retval : void
===================================*/


#if MYDMA
void EC20_Send_ATCMD(const uint8_t *data)
{
	uint16_t len = strlen((char*)data);
    
	memset(&lte4g.rxbuf[0],0,sizeof(GD_USART2));
    lte4g.rx_done_flag = 0;
    lte4g.rxindex = 0;
    
  len = 5;
	HAL_UART_Transmit(&USART2_Handler,(uint8_t *)data,len,0x1FFFF);  
	while(__HAL_UART_GET_FLAG(&USART2_Handler,UART_FLAG_TC) != SET){};
    //memset(at_data.buf, 0x0, USART2_RXMAX_LEN);
}

//void myec20_dma_sendbuf(const uint8_t *data , uint16_t len)
//{

//  MYDMA_USART_Transmit(&USART2_Handler,(u8*)data,len); //启动传输   
//}

#else

void EC20_Send_ATCMD(const uint8_t *data)
{
    uint16_t len = strlen((char*)data);

    memset(&at_data.buf[0], 0x0, sizeof(AT_DATA));
    len = strlen((char*)AT);
    MYDMA_USART_Transmit(&USART2_Handler,(u8*)data,len); //启动传输   
}
#endif





void EC20_Init(void)
{
	char *strx ,*extstrx;
    uint8_t try_i = 0;
    uint8_t err = 0;
    uint8_t *msg_buf_init;
    
    type_ec20 = 0;
    
//1发送AT测试指令
    printf("AT Test\r\n");
	EC20_Send_ATCMD((uint8_t *)AT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
    
//2发送 ATE0,关闭回显
	EC20_Send_ATCMD((uint8_t *)ATE0);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//检查CSQ
	EC20_Send_ATCMD((uint8_t *)CSQ);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//检查模块版本号
	EC20_Send_ATCMD((uint8_t *)ATI);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);

//检查SIM卡是否在位，卡的缺口向外放置
	EC20_Send_ATCMD((uint8_t *)CPIN);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);

   //delay_ms(3000);
    
    
//查看是否注册GSM网络
	EC20_Send_ATCMD((uint8_t *)CREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
    
//查看是否注册GPRS网络
	EC20_Send_ATCMD((uint8_t *)CGREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//查看注册到哪个运营商，支持移动 联通 电信 
	EC20_Send_ATCMD((uint8_t *)COPS);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//关闭 SOCKET 连接
	EC20_Send_ATCMD((uint8_t *)QICLOSE);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
//接入APN，无用户名和密码
	EC20_Send_ATCMD((uint8_t *)QICSGP);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK_1);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]); 
    
//去激活
	EC20_Send_ATCMD((uint8_t *)QIDEACT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);

//激活
	EC20_Send_ATCMD((uint8_t *)QIACT);
    delay_ms(3000);//delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
//获取当前卡的IP地址
	EC20_Send_ATCMD((uint8_t *)QIACT_IP);
    delay_ms(NEW_EC20_DEALY_TMIE);
	strx = strstr((const char*)(char *)&lte4g.rxbuf[0],(const char*)"OK");//返回OK
    if(strx != NULL){
        start_up_ok = 1;  //4G  启动成功
        printf("%s \r\n", (char *)&lte4g.rxbuf[0]);
        printf("4G Start Up [ OK ] ...! \r\n");
        
    }else{
        start_up_ok = 0;  //4G  启动失败
        printf("4G Start Up [Error]...! \r\n");
    }

//查询当前网络参数
    EC20_Send_ATCMD((uint8_t *)QNWINFO);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%s \r\n", (char *)&lte4g.rxbuf[0]);
    
   
}


//中国移动使用46000、46002、46007
//中国联通使用46001、46006
//中国电信使用46003、46005、46011
// \r\n+QNWINFO: "TDD LTE","46000","LTE BAND 40",38950\r\n\r\nOK\r\n
//     +QNWINFO: "TDD LTE","46000","LTE BAND 40",38950

#else
#include "ec20.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
//#include "includes.h"
#include "usart2.h"
//#include "ec20_task.h"
#include "string.h"
    
#define  NEW_EC20_DEALY_TMIE    500

extern   GD_USART2	 			lte4g;
extern   UART_HandleTypeDef 	USART2_Handler; 	//UART句柄
//extern   OS_EVENT               *usart2_q;          //消息队列

uint8_t  mode_for_4g = 0;//1:中国移动，2联通，3电信
uint8_t  start_up_ok = 0;

extern AT_DATA at_data;


extern volatile uint8_t type_ec20;

void EC20_Rest_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOD_CLK_ENABLE();               			//开启GPIOD时钟

	GPIO_Initure.Pin   = GPIO_PIN_7; 						//PD7  4G RESET
    GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  				//推挽输出
    GPIO_Initure.Pull  = GPIO_PULLUP;          				//上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;      				//高速
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
//    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0置7
}

void EC20_RESET(void)
{
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);
    delay_ms(500);										    //延时10s
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0置7
    delay_ms(10000);//10S
}

void EC20_BOOT(void)
{
    uint8_t err = 0;
    uint8_t wait_cnt = 0;
    char *strx;
    uint8_t *msg_buf_boot;
    
    printf("EC20_BOOT Start \r\n");
    EC20_Rest_Init();

    
  //EC20_RESET();


    delay_ms(3000);
    memset(&at_data.buf[0],0,sizeof(AT_DATA));
    printf("%s \r\n", (char *)&at_data.buf[0]);

    EC20_Send_ATCMD((uint8_t *)AT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    strx = strstr((const char*)(char *)&at_data.buf[0],(const char*)"OK");//返回OK
    if(strx == NULL){
        while(wait_cnt <= 5){
            wait_cnt++;

            EC20_Send_ATCMD((uint8_t *)AT);
            delay_ms(NEW_EC20_DEALY_TMIE);
            strx = strstr((const char*)(char *)&at_data.buf[0],(const char*)"OK");//返回OK
            if(strx != NULL){
                printf("%s wait_cnt = %d \r\n", strx, wait_cnt);
                return;  
            }else{
                EC20_Send_ATCMD((uint8_t *)CFUN);
                delay_ms(3000);
                printf(" wait_cnt = %d \r\n", wait_cnt);
            }
        }
    } 
}


/*===================================
* @brief  : EC20   发送字符串
* @param  : *data  要发送的数据
* @retval : void
===================================*/
#if MYDMA
void EC20_Send_ATCMD(const uint8_t *data)
{
	uint16_t len = strlen((char*)data);
    
	memset(&lte4g.rxbuf[0],0,sizeof(GD_USART2));
    lte4g.rx_done_flag = 0;
    lte4g.rxindex = 0;
    
	HAL_UART_Transmit(&USART2_Handler,(uint8_t *)data,len,0x1FFFF);  
	while(__HAL_UART_GET_FLAG(&USART2_Handler,UART_FLAG_TC) != SET){};
    //memset(at_data.buf, 0x0, USART2_RXMAX_LEN);
}
#else

void EC20_Send_ATCMD(const uint8_t *data)
{
    uint16_t len = strlen((char*)data);

    memset(&at_data.buf[0], 0x0, sizeof(AT_DATA));
    
    len = strlen((char*)data);
    MYDMA_USART_Transmit(&USART2_Handler,(u8*)data,len); //启动传输   
}
#endif




void EC20_Init(void)
{
	char *strx ,*extstrx;
    uint8_t try_i = 0;
    uint8_t err = 0;
    uint8_t *msg_buf_init;
    
    type_ec20 = 0;
    
//1发送AT测试指令
    printf("AT Test\r\n");
	EC20_Send_ATCMD((uint8_t *)AT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
    
//2发送 ATE0,关闭回显
	EC20_Send_ATCMD((uint8_t *)ATE0);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//检查CSQ
	EC20_Send_ATCMD((uint8_t *)CSQ);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//检查模块版本号
	EC20_Send_ATCMD((uint8_t *)ATI);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);

//检查SIM卡是否在位，卡的缺口向外放置
	EC20_Send_ATCMD((uint8_t *)CPIN);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);

   //delay_ms(3000);
    
    
//查看是否注册GSM网络
	EC20_Send_ATCMD((uint8_t *)CREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
    
//查看是否注册GPRS网络
	EC20_Send_ATCMD((uint8_t *)CGREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//查看注册到哪个运营商，支持移动 联通 电信 
	EC20_Send_ATCMD((uint8_t *)COPS);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//关闭 SOCKET 连接
	EC20_Send_ATCMD((uint8_t *)QICLOSE);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
//接入APN，无用户名和密码
	EC20_Send_ATCMD((uint8_t *)QICSGP);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK_1);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]); 
    
//去激活
	EC20_Send_ATCMD((uint8_t *)QIDEACT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);

//激活
	EC20_Send_ATCMD((uint8_t *)QIACT);
    delay_ms(3000);//delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
//获取当前卡的IP地址
	EC20_Send_ATCMD((uint8_t *)QIACT_IP);
    delay_ms(NEW_EC20_DEALY_TMIE);
	strx = strstr((const char*)(char *)&at_data.buf[0],(const char*)"OK");//返回OK
    if(strx != NULL){
        start_up_ok = 1;  //4G  启动成功
        printf("%s \r\n", (char *)&at_data.buf[0]);
        printf("4G Start Up [ OK ] ...! \r\n");
        
    }else{
        start_up_ok = 0;  //4G  启动失败
        printf("4G Start Up [Error]...! \r\n");
    }

//查询当前网络参数
    EC20_Send_ATCMD((uint8_t *)QNWINFO);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%s \r\n", (char *)&at_data.buf[0]);
    
   
}

#endif