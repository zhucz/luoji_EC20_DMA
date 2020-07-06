/*******************************************************************************
 * ��Ȩ˵���� Copyright (c) 2020.06
 *
 * �� �� ���� EC20.c
 * ��    �ߣ� 282081226@126.com 
 * �������ڣ� 2020��06��18��
 * �ļ�������4Gģ���ʼ��
 *
 * �޸���־�� 
 * �汾��        ����        ����        ˵��
 * v0.1          2020-06-18  tigers       �����ļ�
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
extern   UART_HandleTypeDef 	USART2_Handler; 	//UART���
//extern   OS_EVENT               *usart2_q;          //��Ϣ����

uint8_t  mode_for_4g = 0;//1:�й��ƶ���2��ͨ��3����
uint8_t  start_up_ok = 0;


extern volatile uint8_t type_ec20;

extern AT_DATA at_data;

void EC20_Rest_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOD_CLK_ENABLE();               			//����GPIODʱ��

	GPIO_Initure.Pin   = GPIO_PIN_7; 						//PD7  4G RESET
    GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  				//�������
    GPIO_Initure.Pull  = GPIO_PULLUP;          				//����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;      				//����
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
//    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0��7
}

void EC20_RESET(void)
{
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);
    delay_ms(500);										    //��ʱ10s
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0��7
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
    strx = strstr((const char*)(char *)&lte4g.rxbuf[0],(const char*)"OK");//����OK
    if(strx == NULL){
        while(wait_cnt <= 5){
            wait_cnt++;

            EC20_Send_ATCMD((uint8_t *)AT);
            delay_ms(NEW_EC20_DEALY_TMIE);
            strx = strstr((const char*)(char *)&lte4g.rxbuf[0],(const char*)"OK");//����OK
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
* @brief  : EC20   �����ַ���
* @param  : *data  Ҫ���͵�����
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

//  MYDMA_USART_Transmit(&USART2_Handler,(u8*)data,len); //��������   
//}

#else

void EC20_Send_ATCMD(const uint8_t *data)
{
    uint16_t len = strlen((char*)data);

    memset(&at_data.buf[0], 0x0, sizeof(AT_DATA));
    len = strlen((char*)AT);
    MYDMA_USART_Transmit(&USART2_Handler,(u8*)data,len); //��������   
}
#endif





void EC20_Init(void)
{
	char *strx ,*extstrx;
    uint8_t try_i = 0;
    uint8_t err = 0;
    uint8_t *msg_buf_init;
    
    type_ec20 = 0;
    
//1����AT����ָ��
    printf("AT Test\r\n");
	EC20_Send_ATCMD((uint8_t *)AT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
    
//2���� ATE0,�رջ���
	EC20_Send_ATCMD((uint8_t *)ATE0);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//���CSQ
	EC20_Send_ATCMD((uint8_t *)CSQ);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//���ģ��汾��
	EC20_Send_ATCMD((uint8_t *)ATI);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);

//���SIM���Ƿ���λ������ȱ���������
	EC20_Send_ATCMD((uint8_t *)CPIN);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);

   //delay_ms(3000);
    
    
//�鿴�Ƿ�ע��GSM����
	EC20_Send_ATCMD((uint8_t *)CREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
    
//�鿴�Ƿ�ע��GPRS����
	EC20_Send_ATCMD((uint8_t *)CGREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//�鿴ע�ᵽ�ĸ���Ӫ�̣�֧���ƶ� ��ͨ ���� 
	EC20_Send_ATCMD((uint8_t *)COPS);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
//�ر� SOCKET ����
	EC20_Send_ATCMD((uint8_t *)QICLOSE);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
//����APN�����û���������
	EC20_Send_ATCMD((uint8_t *)QICSGP);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK_1);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]); 
    
//ȥ����
	EC20_Send_ATCMD((uint8_t *)QIDEACT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);

//����
	EC20_Send_ATCMD((uint8_t *)QIACT);
    delay_ms(3000);//delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&lte4g.rxbuf[0]);
    
    
//��ȡ��ǰ����IP��ַ
	EC20_Send_ATCMD((uint8_t *)QIACT_IP);
    delay_ms(NEW_EC20_DEALY_TMIE);
	strx = strstr((const char*)(char *)&lte4g.rxbuf[0],(const char*)"OK");//����OK
    if(strx != NULL){
        start_up_ok = 1;  //4G  �����ɹ�
        printf("%s \r\n", (char *)&lte4g.rxbuf[0]);
        printf("4G Start Up [ OK ] ...! \r\n");
        
    }else{
        start_up_ok = 0;  //4G  ����ʧ��
        printf("4G Start Up [Error]...! \r\n");
    }

//��ѯ��ǰ�������
    EC20_Send_ATCMD((uint8_t *)QNWINFO);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%s \r\n", (char *)&lte4g.rxbuf[0]);
    
   
}


//�й��ƶ�ʹ��46000��46002��46007
//�й���ͨʹ��46001��46006
//�й�����ʹ��46003��46005��46011
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
extern   UART_HandleTypeDef 	USART2_Handler; 	//UART���
//extern   OS_EVENT               *usart2_q;          //��Ϣ����

uint8_t  mode_for_4g = 0;//1:�й��ƶ���2��ͨ��3����
uint8_t  start_up_ok = 0;

extern AT_DATA at_data;


extern volatile uint8_t type_ec20;

void EC20_Rest_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOD_CLK_ENABLE();               			//����GPIODʱ��

	GPIO_Initure.Pin   = GPIO_PIN_7; 						//PD7  4G RESET
    GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  				//�������
    GPIO_Initure.Pull  = GPIO_PULLUP;          				//����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;      				//����
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
//    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0��7
}

void EC20_RESET(void)
{
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);
    delay_ms(500);										    //��ʱ10s
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD0��7
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
    strx = strstr((const char*)(char *)&at_data.buf[0],(const char*)"OK");//����OK
    if(strx == NULL){
        while(wait_cnt <= 5){
            wait_cnt++;

            EC20_Send_ATCMD((uint8_t *)AT);
            delay_ms(NEW_EC20_DEALY_TMIE);
            strx = strstr((const char*)(char *)&at_data.buf[0],(const char*)"OK");//����OK
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
* @brief  : EC20   �����ַ���
* @param  : *data  Ҫ���͵�����
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
    MYDMA_USART_Transmit(&USART2_Handler,(u8*)data,len); //��������   
}
#endif




void EC20_Init(void)
{
	char *strx ,*extstrx;
    uint8_t try_i = 0;
    uint8_t err = 0;
    uint8_t *msg_buf_init;
    
    type_ec20 = 0;
    
//1����AT����ָ��
    printf("AT Test\r\n");
	EC20_Send_ATCMD((uint8_t *)AT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
    
//2���� ATE0,�رջ���
	EC20_Send_ATCMD((uint8_t *)ATE0);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//���CSQ
	EC20_Send_ATCMD((uint8_t *)CSQ);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//���ģ��汾��
	EC20_Send_ATCMD((uint8_t *)ATI);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);

//���SIM���Ƿ���λ������ȱ���������
	EC20_Send_ATCMD((uint8_t *)CPIN);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);

   //delay_ms(3000);
    
    
//�鿴�Ƿ�ע��GSM����
	EC20_Send_ATCMD((uint8_t *)CREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
    
//�鿴�Ƿ�ע��GPRS����
	EC20_Send_ATCMD((uint8_t *)CGREG);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//�鿴ע�ᵽ�ĸ���Ӫ�̣�֧���ƶ� ��ͨ ���� 
	EC20_Send_ATCMD((uint8_t *)COPS);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
//�ر� SOCKET ����
	EC20_Send_ATCMD((uint8_t *)QICLOSE);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
//����APN�����û���������
	EC20_Send_ATCMD((uint8_t *)QICSGP);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    EC20_Send_ATCMD((uint8_t *)QICSGP_CHECK_1);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]); 
    
//ȥ����
	EC20_Send_ATCMD((uint8_t *)QIDEACT);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);

//����
	EC20_Send_ATCMD((uint8_t *)QIACT);
    delay_ms(3000);//delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%d : %s \r\n", __LINE__,(char *)&at_data.buf[0]);
    
    
//��ȡ��ǰ����IP��ַ
	EC20_Send_ATCMD((uint8_t *)QIACT_IP);
    delay_ms(NEW_EC20_DEALY_TMIE);
	strx = strstr((const char*)(char *)&at_data.buf[0],(const char*)"OK");//����OK
    if(strx != NULL){
        start_up_ok = 1;  //4G  �����ɹ�
        printf("%s \r\n", (char *)&at_data.buf[0]);
        printf("4G Start Up [ OK ] ...! \r\n");
        
    }else{
        start_up_ok = 0;  //4G  ����ʧ��
        printf("4G Start Up [Error]...! \r\n");
    }

//��ѯ��ǰ�������
    EC20_Send_ATCMD((uint8_t *)QNWINFO);
    delay_ms(NEW_EC20_DEALY_TMIE);
    printf("%s \r\n", (char *)&at_data.buf[0]);
    
   
}

#endif