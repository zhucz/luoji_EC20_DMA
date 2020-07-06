
#include "UpgradeInit.h"

#define FLASH_SAVE_ADDR  0x08040000 	//设置FLASH 保存地址(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.
										//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.	
										
//要写入到STM32 FLASH的字符串数组									
#define READ_COUNT 512
u8   ReadAppBuffer[READ_COUNT];	//数组长度	
u32  ChangeBuffer[READ_COUNT/4];	

char *m_file = "3:fwworker.BIN"; //APP 文件名称 默认起始为 "3:"后跟文件名，文件名不能含中文
										
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

FIL    FP_Struct;		//用于打开文件	

u32 Update_Flag0[2]={0x00000000};
u32 Update_Flag1[2]={0x00000000};
u32 Update_Flag2[2]={0xffffffff};

u8 Wait_Number=0;//用于判断USB是否插入

u8 UsbConnected = 0;

u8 t;

void Record_log(const char *level,const char *event,const char *position);


//创建日志文件
void Creat_journal_Txt(void)
{
	FIL log_file;
	f_open(&log_file, "1:/work.log", FA_OPEN_ALWAYS  | FA_WRITE);	
	f_close (&log_file);
}





void Record_log(const char *level,const char *event,const char *position)
{

	FIL log_file;
	FRESULT res;
	
	res=f_open(&log_file, "1:/work.log",FA_WRITE);//
	
	//if(res==FR_OK)
	{


		f_lseek (&log_file,f_size (&log_file));
		
		f_printf(&log_file, "%s",level);
		
		f_printf(&log_file, "  20%02d-%02d-%02d %02d:%02d:%02d  ",RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date, RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
		
		f_printf(&log_file, "%s",event);
		
		f_printf(&log_file, "  %s\r\n",position);
	}
	f_close (&log_file);
}













//用户测试主程序
//返回值:0,正常
// 1,有问题
u8 USH_User_App(void)
{ 
	u32  APP_Sector = 0;
	u16  APP_Byte = 0;
	u16  ReadNum;
	u32  i = 0;
	u32  aa = 0;
	u32 total,free;
	u8 gCheckFat=0;
	
	f_mount(fs[3],"3:",1); 	//重新挂载U盘
	
	Wait_Number++;
	
	gCheckFat=exf_getfree("3:",&total,&free);
	
	if(gCheckFat == 0 && UsbConnected == 0)
	{			
			UsbConnected = 1;
			gCheckFat = f_open(&FP_Struct,m_file,FA_READ);
			if(gCheckFat == 0)
			{
				aa=f_size(&FP_Struct);
				
				printf("%d\r\n",aa);
				
				APP_Sector = aa / READ_COUNT;
				APP_Byte = aa % READ_COUNT;
				for(i = 0;i < APP_Sector;i++)
				{
					f_read (&FP_Struct,ReadAppBuffer,READ_COUNT,(UINT *)&ReadNum);
					STMFLASH_Write(FLASH_SAVE_ADDR + i * READ_COUNT,(u32*)ReadAppBuffer,READ_COUNT/4);	
				}
				if(APP_Byte != 0)
				{
					f_read (&FP_Struct,ReadAppBuffer,APP_Byte,(UINT *)&ReadNum);
					
					STMFLASH_Write(FLASH_SAVE_ADDR + APP_Sector*READ_COUNT,(u32*)ReadAppBuffer,APP_Byte/4);
				
				}
				f_close (&FP_Struct);
				POINT_COLOR=RED;   	
				
				Record_log("警戒","升级程序升级成功","Creat_journal_Txt");
				
				Show_Str(160,50,200,24,"ϵͳ�����ɹ���",24,0);
				
				delay_ms(2000);
				
				HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
	
				__HAL_RCC_TIM4_CLK_DISABLE();
				
				if(((*(vu32*)(FLASH_SAVE_ADDR+4))&0xFF000000) == 0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	
					STMFLASH_Write(0x081FF000,Update_Flag2,2);
					delay_ms(500);
					
					Record_log("警戒","升级程序系统重启","USH_User_App");
					HAL_NVIC_SystemReset();	
				}
				else 
				{
					printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
					
					Record_log("警戒","升级程序地址非法","USH_User_App");
					
					LCD_ShowString(30,210,200,16,16,"Illegal FLASH APP!");	   
				}														
				
			}			
	} 
	
	
	f_mount(0,"3:",1); 	//卸载U盘
			 
	return gCheckFat;
} 


void system_init(void)
{
  Stm32_Clock_Init(360,25,2,8); 
  delay_init(192);
  uart_init(115200);

  LED_Init();			
  SDRAM_Init();			
  LCD_Init();				
  RTC_Init();       
  RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); 

  my_mem_init(SRAMIN);		
  my_mem_init(SRAMEX);		
  my_mem_init(SRAMCCM);		
  
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
  
}



//系统初始化
void  UpgradeInit(void)
{
	FRESULT res;

	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz   
	
//	STMFLASH_Read(0x081FF000,Update_Flag1,2);
	
//	if(Update_Flag1[0] == 0xffffffff)
//	{
//		iap_load_app(FLASH_SAVE_ADDR);//执褠FLASH APP蘸毛
//	
//	}
	
	delay_init(192);                //初始化延时函数
    uart_init(115200);
    //uart_init(9600);              //初始化USART
    //usmart_dev.init(96);
   	LED_Init();					//初始化与LED连接的硬件接口
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD 
	RTC_Init();                 //初始化RTC 
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //配置WAKE UP中断,1秒钟中断一次  
	
 	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	

	exfuns_init();				//为fatfs相关变量申请内存  
	
	W25QXX_Init();				//缘始郫W25Q128
	
	delay_ms(1000);
 	res=f_mount(fs[1],"1:",1); 		//挂载SPI FLASH.
	
	if(res==FR_NO_FILESYSTEM)
	{
		W25QXX_Init();	
		delay_ms(1000);
		res=f_mount(fs[1],"1:",1); 		//趻諛SPI FLASH.
		if(res==FR_NO_FILESYSTEM)
		{
			W25QXX_Init();
			delay_ms(1000);	
			res=f_mount(fs[1],"1:",1); 		//趻諛SPI FLASH.
		}
	}
	delay_ms(1000);
	
	if(res == FR_NO_FILESYSTEM)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		
		
		Record_log("警戒","升级程序格式","UpgradeInit");
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:");	//设置Flash磁盘的名字为：ZXAQ
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//格式化完成
			
			Record_log("警戒","升级程序格式成功","UpgradeInit");
		}
		else 
		{
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//格式化失败
			Record_log("警戒","升级程序格式失败","UpgradeInit");
			
		}
		delay_ms(1000);
	}	
	
	POINT_COLOR=WHITE; 

	STMFLASH_Read(0x081FF000,Update_Flag1,2);
	
	delay_ms(1000);

	//初始化USB主机
	USBH_Init(&USB_OTG_Core,USB_OTG_HS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb);
   
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	
	Record_log("警戒","升级程序初始成功","UpgradeInit");
	
}


//工作程序
void  Upgradeapp(void)
{
	if(Update_Flag1[0] == 0x00000000)
	{
		Show_Str(100,50,200,24,"��������ϵͳ��",24,0);
		
		Show_Str(100,218,200,24,"��Ҫ��Ű����",24,0);
		
		Show_Str(100,386,200,24,"ʱ�䡣",24,0);
		
		Show_Str(130,50,200,24,"���������벻Ҫ",24,0);
		
		Show_Str(130,218,200,24,"�ػ���ϵ硣",24,0);

		USBH_Process(&USB_OTG_Core, &USB_Host);	
	}
	else
	{

		HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
		__HAL_RCC_TIM4_CLK_DISABLE();
		__HAL_RCC_UART7_CLK_DISABLE();
		__HAL_RCC_SPI2_CLK_DISABLE(); 
		__HAL_RCC_PWR_CLK_DISABLE();
		__HAL_RCC_LTDC_CLK_DISABLE();
		__HAL_RCC_DMA2D_CLK_DISABLE();   
		__HAL_RCC_USB_OTG_HS_CLK_DISABLE();
		__HAL_RCC_FMC_CLK_DISABLE(); 
		__disable_irq();
		
		if(((*(vu32*)(FLASH_SAVE_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{	 
			Record_log("警戒","升级程序跳转工作程序","Upgradeapp");
			
			iap_load_app(FLASH_SAVE_ADDR);//执行FLASH APP代码
		}		
	}

	t++;
	if((t%2) == 0)  //每100ms更新一次显示数据
	{
		HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	} 
	//if((t%4) == 0) LED0=!LED0;	//每200ms,翻转一次LED0 
	delay_ms(50);
	
	if(t == 250)//等待12.5秒
	{
		if(Wait_Number == 0)
		{
			POINT_COLOR=RED;  
			
			Show_Str(160,50,200,24,"ϵͳ����ʧ�ܣ�",24,0);
	
			delay_ms(2000);
			
			Record_log("致命","升级程序升级失败","Upgradeapp");

			HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
			__HAL_RCC_TIM4_CLK_DISABLE();
			__HAL_RCC_UART7_CLK_DISABLE();
			__HAL_RCC_SPI3_CLK_DISABLE(); 
			__HAL_RCC_PWR_CLK_DISABLE();
			__HAL_RCC_LTDC_CLK_DISABLE();
			__HAL_RCC_DMA2D_CLK_DISABLE();   
			__HAL_RCC_USB_OTG_HS_CLK_DISABLE();
			__HAL_RCC_FMC_CLK_DISABLE(); 
			__disable_irq();
	

			if(((*(vu32*)(FLASH_SAVE_ADDR+4))&0xFF000000) == 0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_SAVE_ADDR);//执行FLASH APP代码
			}					

		}
	}

}




