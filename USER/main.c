
#include "UpgradeInit.h"
#include "usart2.h"
#include "ec20.h"
#include "string.h"
// at_data.buf 的拷贝
typedef struct {
    char buf[512];
    int w_index;
}RECV_BUF;
static RECV_BUF recv_buf;


extern AT_DATA at_data;

#if MYDMA 
 extern volatile uint8_t type_ec20;
#else
volatile uint8_t type_ec20;
#endif


const uint8_t send_0x1a[1]= {0x1A};              //EC20 Ctrl+Z 后才能传输数据 特殊字符
static char at_buf_copy[512];                   // 处理后的服务器响应



static int find_pattern(const char *msg, const char *pattern)
{
	int l = 0;
	while(*(msg+l)!='\0')
	{
		if(strncmp(msg+l, pattern, strlen(pattern))==0)
			return l;
		l++;
	}
	return -1;
}

/*
AT模块接收的响应解析。
    还有一个场景没测试, 可能会有问题???
SEND OK

+QIURC: "recv",0,17
HTTP/1.1 200 OK

+QIURC: "recv",0,226          
Content-Type: application/json
Content-Length: 171

{
  "code": 0, 
  "data": {
    "datetime": "20200521111111", 
    "description": "\u95f2\u4e8b\u5fc3\u8df3", 
    "task": 0
  }, 
  "msg": "\u6210\u529f", 
  "ok": true
}

+QIURC: "closed",0

返回值:     0： 解析成功
            -1: 解析失败
*/
static int at_parse(void)
{
    int ret_code = -1;
    char buf[32];          
    int start=0, end, len, i, pos=0;
    memset(recv_buf.buf, 0x0, sizeof(RECV_BUF));
    
    while(1)
    {
        start = find_pattern(at_buf_copy+pos, "+QIURC: \"recv\"");
        if(start == -1){
           if(strlen(recv_buf.buf))
               ret_code = 0;
           else
               ret_code = -1;
           break;
        }
           
        end = find_pattern(at_buf_copy + start + pos, "\r\n");
        if(end == -1)
           break;
        
        memset(buf, 0x0, sizeof(char)*32);
        memcpy(buf, at_buf_copy+start+pos+17, end-17);
        len = atoi(buf);
        
        if(strlen(at_buf_copy) - end < len)
            break;
        else{
            for(i=0; i<len; i++)
                recv_buf.buf[recv_buf.w_index++] = *(at_buf_copy+start+pos+end+i+2);
        }
        
        pos = start+end+len+2+pos;
    }
    return ret_code;
}



int at_send_func(const char* send_buf)
{
    int count = 0;


    memset(at_data.buf, 0x0, USART2_RXMAX_LEN);
    type_ec20 = 1;
    // 建立连接
    at_data.idx = 0;
    EC20_Send_ATCMD((uint8_t *)TCPCONN_PUBLIC_DIR);  //TCPCONN_ZXAQ   TCPCONN_PUBLIC_DIR
    count = 0;
    while(1){
        count++;
        delay_ms(100);
        if(count >= 10)
            goto err_tcp;
        else{
            if(strstr((char*)at_data.buf, "+QIOPEN: 0,0")){
                printf("ec usart recv: [%s]\n", at_data.buf);
                break;
             }else{
                 printf("conn try: %d,[%s] \n", count, at_data.buf);
             }     
        }  
    }
    
    // 设立连接模式
    memset(at_data.buf, 0x0, USART2_RXMAX_LEN);
    at_data.idx = 0;
    EC20_Send_ATCMD((uint8_t *)DIR_OUTPU_1);
    
    count = 0;
    while(1){ 

        if(++count >= 50)
            goto err_tcp;
        else{
            if(strstr((char*)at_data.buf, "\r\n> ")){
                printf("ec recv mode: [%s]\n", at_data.buf);

                break;
             }else{
                 delay_ms(100);
                 printf("conn set mode: %d,[%s] \n", count, at_data.buf);
             }     
        }  
    }
        
     memset(at_data.buf, 0x0, USART2_RXMAX_LEN);
    at_data.idx = 0;
    // 发送数据
    EC20_Send_ATCMD((uint8_t *)send_buf);
    EC20_Send_ATCMD((uint8_t *)send_0x1a);   
    
    count = 0;
    while(1)
    {   
        delay_ms(3000);
        if(++count >= 30)
            break;
        memset(at_buf_copy, 0x0, 1024);
        strcpy(at_buf_copy, (char*)at_data.buf);
        
        printf("recv: \n");
        printf("%d==>%s\n", count, at_buf_copy);
        
        if(at_parse() == 0){
            goto succ_tcp;
        }
    }
    
    // 重试失败
    
err_tcp:
    // 数据接收失败, 关闭连接
    printf("before close: [%s]\n", at_data.buf);
    memset(at_data.buf, 0x0, USART2_RXMAX_LEN);
    at_data.idx = 0;
    EC20_Send_ATCMD((uint8_t *)QICLOSE);
    delay_ms(100);
    return -1;
    
succ_tcp:
    // 数据接收成功, 关闭连接
    memset(at_data.buf, 0x0, USART2_RXMAX_LEN);
    at_data.idx = 0;
    EC20_Send_ATCMD((uint8_t *)QICLOSE);
    delay_ms(100);
    return 0;
     
}

const char *test_ec20 = "POST /te-1/v1/alarmEvents HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\r\nContent-Type: application/json\r\nHost: 192.168.3.135\r\nContent-Length: 213\r\n\r\n{\"code\":\"12345678\",\"link\":1,\"data\":[{\"controller_type_code\":14,\"controller_num\":1,\"equipment_num\":4,\"module_num\":0,\"loop_num\":1,\"addr_num\":4,\"chan_num\":0,\"dev_code\":18,\"datetime\":\"20200624184145\",\"evt_code\":296}]}";

int err_send;


void LED_Init1(void)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_GPIOB_CLK_ENABLE();               			//开启GPIOB时钟
  __HAL_RCC_GPIOI_CLK_ENABLE();               			//开启GPIOI时钟
  __HAL_RCC_GPIOD_CLK_ENABLE();               			//开启GPIOD时钟

  GPIO_Initure.Pin   = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5; 	//PB1,0,5
  GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  				//推挽输出
  GPIO_Initure.Pull  = GPIO_PULLUP;          				//上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;      				//高速
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);


  GPIO_Initure.Pin   = GPIO_PIN_7; 						//PI7  RUN_LED
  GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  				//推挽输出
  GPIO_Initure.Pull  = GPIO_PULLUP;          				//上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;      				//高速
  HAL_GPIO_Init(GPIOI,&GPIO_Initure);


  GPIO_Initure.Pin   = GPIO_PIN_7; 						//PD7  4G RESET
  GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  				//推挽输出
  GPIO_Initure.Pull  = GPIO_PULLUP;          				//上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;      				//高速
  HAL_GPIO_Init(GPIOD,&GPIO_Initure);

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);		//PB1置1 
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);		//PB0置1  
}


void LED_Flash1(void)
{
	static uint8_t i = 0;
	if(i == 0){
	   i = 1;
	   HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_SET);
	}else{
	   i = 0;
	   HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_RESET);
	}
}



char *data_1 = "AT1111\r\n";
uint8_t dat_buf[10] = {0x7E,0x00,0x01,0x20,0x5a};

float pro=0;  
#define SEND_BUF_SIZE 100	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.
extern UART_HandleTypeDef 	        USART2_Handler; 	//UART句柄  USART2_Handler

uint16_t len = 0;

extern volatile uint8_t rx_ok;
extern volatile uint16_t rx_cnt;


int main(void)
{
  system_init();
  USART2_Init(115200);        //add 2020-06-18
  MYDMA_Init_TEST();
  LED_Init1();
  
  while(1)
	{
//        err_send = -1;
//        if((err_send = at_send_func(test_ec20)) != 0){//第一次发送
//            printf("err_no: %d.......[send error 1]\r\n", err_send); 
//            delay_ms(600);
//            
//            if((err_send = at_send_func(test_ec20)) != 0){
//                printf("err_no: %d.......[send error 2]\r\n", err_send); 
//                delay_ms(1000);
//                
//                if((err_send = at_send_func(test_ec20)) != 0){
//                    printf("err_no: %d.......[send error 3]\r\n", err_send); 
//                }    
//            }   
//        }
        
        printf("%s", recv_buf.buf);
        printf("do something... \n");
        
        

        //_BSP_UARTxTransmit((uint8_t *)data_1, 12);
          //EC20_Send_ATCMD((uint8_t *)data_1);
          //EC20_Send_ATCMD(dat_buf);
          
//          MYDMA_USART_Transmit(&USART2_Handler,dat_buf,5);
//          MYDMA_USART_Transmit(&USART2_Handler,at_data.buf,at_data.idx);
//          at_data.idx = 0;
          
          if(rx_ok == 1){
            rx_ok = 0;
            HAL_UART_DMAStop(&USART2_Handler);
            MYDMA_USART_Transmit(&USART2_Handler,at_data.buf,at_data.idx);
           
          }
          
        
//         len = strlen((char*)AT);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)AT,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)ATE0);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)ATE0,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)CSQ);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CSQ,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)CPIN);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CPIN,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)ATI);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)ATI,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)CREG);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CREG,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)CGREG);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CGREG,len); //启动传输   
//         delay_ms(1000);
//         

//         len = strlen((char*)TCPCONN_PUBLIC_DIR);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)TCPCONN_PUBLIC_DIR,len); //启动传输   
//         delay_ms(1000);
//         
//         
//         len = strlen((char*)DIR_OUTPU_1);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)DIR_OUTPU_1,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)TCPCONN_PUBLIC_DIR);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)TCPCONN_PUBLIC_DIR,len); //启动传输   
//         delay_ms(1000);
//         
//         len = strlen((char*)test_ec20);
//         MYDMA_USART_Transmit(&USART2_Handler,(u8*)test_ec20,len); //启动传输   
//         delay_ms(1000);

//        len = strlen((char*)send_0x1a);
//        MYDMA_USART_Transmit(&USART2_Handler,(u8*)send_0x1a,len); //启动传输   
//        delay_ms(1000);

        
        LED_Flash1();
//        delay_ms(1000);
        
	}	
}












//////int main(void)
//////{
//////	//系统初始化
//////	UpgradeInit();
//////    USART2_Init(115200);        //add 2020-06-18

//////    //MYDMA_Init_TEST();
//////    
//////    
//////    
////////	EC20_BOOT();					//EC20 复位需要10s钟
////////    EC20_Init();
//////    printf("AT1111\r\n");
//////    LED_Init1();
//////	while(1)
//////	{
////////        err_send = -1;
////////        if((err_send = at_send_func(test_ec20)) != 0){//第一次发送
////////            printf("err_no: %d.......[send error 1]\r\n", err_send); 
////////            delay_ms(600);
////////            
////////            if((err_send = at_send_func(test_ec20)) != 0){
////////                printf("err_no: %d.......[send error 2]\r\n", err_send); 
////////                delay_ms(1000);
////////                
////////                if((err_send = at_send_func(test_ec20)) != 0){
////////                    printf("err_no: %d.......[send error 3]\r\n", err_send); 
////////                }    
////////            }   
////////        }
//////        
//////        printf("%s", recv_buf.buf);
//////        printf("do something... \n");
//////        
//////        

//////        //_BSP_UARTxTransmit((uint8_t *)data_1, 12);
//////        EC20_Send_ATCMD((uint8_t *)data_1);
//////        
////////         len = strlen((char*)AT);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)AT,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)ATE0);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)ATE0,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)CSQ);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CSQ,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)CPIN);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CPIN,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)ATI);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)ATI,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)CREG);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CREG,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)CGREG);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)CGREG,len); //启动传输   
////////         delay_ms(1000);
////////         

////////         len = strlen((char*)TCPCONN_PUBLIC_DIR);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)TCPCONN_PUBLIC_DIR,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         
////////         len = strlen((char*)DIR_OUTPU_1);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)DIR_OUTPU_1,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)TCPCONN_PUBLIC_DIR);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)TCPCONN_PUBLIC_DIR,len); //启动传输   
////////         delay_ms(1000);
////////         
////////         len = strlen((char*)test_ec20);
////////         MYDMA_USART_Transmit(&USART2_Handler,(u8*)test_ec20,len); //启动传输   
////////         delay_ms(1000);

////////        len = strlen((char*)send_0x1a);
////////        MYDMA_USART_Transmit(&USART2_Handler,(u8*)send_0x1a,len); //启动传输   
////////        delay_ms(1000);
//////         

//////         
//////         
//////         
//////         
////////        while(1){                                    //DMA_FLAG_TCIF2_6
////////            if(__HAL_DMA_GET_FLAG(&USART2TxDMA_Handler,DMA_FLAG_TCIF2_6))//等待  DMA1_Stream6 : TX 传输完成
////////            {
////////                __HAL_DMA_CLEAR_FLAG(&USART2TxDMA_Handler,DMA_FLAG_TCIF2_6);//清除  DMA1_Stream6 : TX  传输完成标志
////////                HAL_UART_DMAStop(&USART2_Handler);      //传输完成以后关闭串口DMA
////////                break; 
////////            }
////////            pro=__HAL_DMA_GET_COUNTER(&USART2TxDMA_Handler);//得到当前还剩余多少个数据
////////            pro=1-pro/SEND_BUF_SIZE;    //得到百分比	  
////////            pro*=100;      			    //扩大100倍
////////        }
//////        
//////        LED_Flash1();
//////        delay_ms(1000);
//////        
//////	}	
//////}
