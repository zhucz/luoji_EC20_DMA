#include "ltdc.h"
#include "lcd.h"



LTDC_HandleTypeDef  LTDC_Handler;	    //LTDC句柄
DMA2D_HandleTypeDef DMA2D_Handler; 	    //DMA2D句柄

//根据不同的颜色格式,定义帧缓存数组
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	u32 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
#else
	u16 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
#endif

u32 *ltdc_framebuf[2];					//LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域
_ltdc_dev lcdltdc;						//管理LCD LTDC的重要参数

//打开LCD开关
//lcd_switch:1 打开,0，关闭
void LTDC_Switch(u8 sw)
{
	if(sw==1) __HAL_LTDC_ENABLE(&LTDC_Handler);
	else if(sw==0)__HAL_LTDC_DISABLE(&LTDC_Handler);
}

//开关指定层
//layerx:层号,0,第一层; 1,第二层
//sw:1 打开;0关闭
void LTDC_Layer_Switch(u8 layerx,u8 sw)
{
	if(sw==1) __HAL_LTDC_LAYER_ENABLE(&LTDC_Handler,layerx);
	else if(sw==0) __HAL_LTDC_LAYER_DISABLE(&LTDC_Handler,layerx);
	__HAL_LTDC_RELOAD_CONFIG(&LTDC_Handler);
}

//选择层
//layerx:层号;0,第一层;1,第二层;
void LTDC_Select_Layer(u8 layerx)
{
	lcdltdc.activelayer=layerx;
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LTDC_Display_Dir(u8 dir)
{
    lcdltdc.dir=dir; 	//显示方向
	if(dir==0)			//竖屏
	{
		lcdltdc.width=lcdltdc.pheight;
		lcdltdc.height=lcdltdc.pwidth;	
	}else if(dir==1)	//横屏
	{
		lcdltdc.width=lcdltdc.pwidth;
		lcdltdc.height=lcdltdc.pheight;
	}
}

//画点函数
//x,y:写入坐标
//color:颜色值
void LTDC_Draw_Point(u16 x,u16 y,u32 color)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	if(lcdltdc.dir)	//横屏
	{
        *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x))=color;
	}else 			//竖屏
	{
        *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x)+y))=color; 
	}
#else
	if(lcdltdc.dir)	//横屏
	{
        *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x))=color;
	}else 			//竖屏
	{
        *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y))=color; 
	}
#endif
}

//读点函数
//x,y:读取点的坐标
//返回值:颜色值
u32 LTDC_Read_Point(u16 x,u16 y)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	if(lcdltdc.dir)	//横屏
	{
		return *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x));
	}else 			//竖屏
	{
		return *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x)+y)); 
	}
#else
	if(lcdltdc.dir)	//横屏
	{
		return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x));
	}else 			//竖屏
	{
		return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y)); 
	}
#endif 
}

//LTDC填充矩形,DMA2D填充
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
//有时候需要频繁的调用填充函数，所以为了速度，填充函数采用寄存器版本，
//不过下面有对应的库函数版本的代码。
void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{ 
	u32 psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	//坐标系转换
	if(lcdltdc.dir)	//横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//竖屏
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	}
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	__HAL_RCC_DMA2D_CLK_ENABLE();	//使能DM2D时钟
	DMA2D->CR&=~(DMA2D_CR_START);	//先停止DMA2D
	DMA2D->CR=DMA2D_R2M;			//寄存器到存储器模式
	DMA2D->OPFCCR=LCD_PIXFORMAT;	//设置颜色格式
	DMA2D->OOR=offline;				//设置行偏移 

	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器
	DMA2D->OCOLR=color;						//设定输出颜色寄存器 
	DMA2D->CR|=DMA2D_CR_START;				//启动DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)	//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//超时退出
	} 
	DMA2D->IFCR|=DMA2D_FLAG_TC;		//清除传输完成标志 		
	

	

}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
//void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
//{
//	u32 psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
//	u32 timeout=0; 
//	u16 offline;
//	u32 addr;  
//    if(ex>=lcdltdc.width)ex=lcdltdc.width-1;
//	if(ey>=lcdltdc.height)ey=lcdltdc.height-1;
//	//坐标系转换
//	if(lcdltdc.dir)	//横屏
//	{
//		psx=sx;psy=sy;
//		pex=ex;pey=ey;
//	}else			//竖屏
//	{
//		psx=sy;psy=lcdltdc.pheight-ex-1;
//		pex=ey;pey=lcdltdc.pheight-sx-1;
//	}
//	offline=lcdltdc.pwidth-(pex-psx+1);
//	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
//    if(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB565)  //如果是RGB565格式的话需要进行颜色转换，将16bit转换为32bit的
//    {
//        color=((color&0X0000F800)<<8)|((color&0X000007E0)<<5)|((color&0X0000001F)<<3);
//    }
//	//配置DMA2D的模式
//	DMA2D_Handler.Instance=DMA2D;
//	DMA2D_Handler.Init.Mode=DMA2D_R2M;			//内存到存储器模式
//	DMA2D_Handler.Init.ColorMode=LCD_PIXFORMAT;	//设置颜色格式
//	DMA2D_Handler.Init.OutputOffset=offline;		//输出偏移 
//	HAL_DMA2D_Init(&DMA2D_Handler);              //初始化DMA2D
//    HAL_DMA2D_ConfigLayer(&DMA2D_Handler,lcdltdc.activelayer); //层配置
//    HAL_DMA2D_Start(&DMA2D_Handler,color,(u32)addr,pex-psx+1,pey-psy+1);//开启传输
//    HAL_DMA2D_PollForTransfer(&DMA2D_Handler,1000);//传输数据
//    while((__HAL_DMA2D_GET_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC)==0)&&(timeout<0X5000))//等待DMA2D完成
//    {
//        timeout++;
//    }
//    __HAL_DMA2D_CLEAR_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC);       //清除传输完成标志
//}

//在指定区域内填充指定颜色块,DMA2D填充	
//此函数仅支持u16,RGB565格式的颜色数组填充.
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)  
//注意:sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
//color:要填充的颜色数组首地址
void LTDC_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u32 psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	//坐标系转换
	if(lcdltdc.dir)	//横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//竖屏
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	}
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	__HAL_RCC_DMA2D_CLK_ENABLE();	//使能DM2D时钟
	DMA2D->CR&=~(DMA2D_CR_START);	//先停止DMA2D
	DMA2D->CR=DMA2D_M2M;			//存储器到存储器模式
	DMA2D->FGPFCCR=LCD_PIXFORMAT;	//设置颜色格式
	DMA2D->FGOR=0;					//前景层行偏移为0
	DMA2D->OOR=offline;				//设置行偏移 

	DMA2D->FGMAR=(u32)color;		//源地址
	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器 
	DMA2D->CR|=DMA2D_CR_START;					//启动DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)		//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//超时退出
	} 
	DMA2D->IFCR|=DMA2D_FLAG_TC;				//清除传输完成标志  	
}  

//LCD清屏
//color:颜色值
void LTDC_Clear(u32 color)
{
	LTDC_Fill(0,0,lcdltdc.width-1,lcdltdc.height-1,color);
	
}

//LTDC时钟(Fdclk)设置函数
//Fvco=Fin*pllsain; 
//Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;

//Fvco:VCO频率
//Fin:输入时钟频率一般为1Mhz(来自系统时钟PLLM分频后的时钟,见时钟树图)
//pllsain:SAI时钟倍频系数N,取值范围:50~432.  
//pllsair:SAI时钟的分频系数R,取值范围:2~7
//pllsaidivr:LCD时钟分频系数,取值范围:RCC_PLLSAIDIVR_2/4/8/16,对应分频2~16 
//假设:外部晶振为25M,pllm=25的时候,Fin=1Mhz.
//例如:要得到20M的LTDC时钟,则可以设置:pllsain=400,pllsair=5,pllsaidivr=RCC_PLLSAIDIVR_4
//Fdclk=1*400/5/4=400/20=20Mhz
//返回值:0,成功;1,失败。


u8 LTDC_Clk_Set(u32 pllsain,u32 pllsair,u32 pllsaidivr)
{
	RCC_PeriphCLKInitTypeDef PeriphClkIniture;
	
	//LTDC输出像素时钟，需要根据自己所使用的LCD数据手册来配置！
    PeriphClkIniture.PeriphClockSelection=RCC_PERIPHCLK_LTDC;	//LTDC时钟 	
	PeriphClkIniture.PLLSAI.PLLSAIN=pllsain;    
	PeriphClkIniture.PLLSAI.PLLSAIR=pllsair;  
	PeriphClkIniture.PLLSAIDivR=pllsaidivr;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkIniture)==HAL_OK)    //配置像素时钟
    {
        return 0;   //成功
    }
    else return 1;  //失败    
}

//LTDC,层颜窗口设置,窗口以LCD面板坐标系为基准
//注意:此函数必须在LTDC_Layer_Parameter_Config之后再设置.
//layerx:层值,0/1.
//sx,sy:起始坐标
//width,height:宽度和高度
void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height)
{
    HAL_LTDC_SetWindowPosition(&LTDC_Handler,sx,sy,layerx);  //设置窗口的位置
    HAL_LTDC_SetWindowSize(&LTDC_Handler,width,height,layerx);//设置窗口大小    
}

//LTDC,基本参数设置.
//注意:此函数,必须在LTDC_Layer_Window_Config之前设置.
//layerx:层值,0/1.
//bufaddr:层颜色帧缓存起始地址
//pixformat:颜色格式.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
//alpha:层颜色Alpha值,0,全透明;255,不透明
//alpha0:默认颜色Alpha值,0,全透明;255,不透明
//bfac1:混合系数1,4(100),恒定的Alpha;6(101),像素Alpha*恒定Alpha
//bfac2:混合系数2,5(101),恒定的Alpha;7(111),像素Alpha*恒定Alpha
//bkcolor:层默认颜色,32位,低24位有效,RGB888格式
//返回值:无
void LTDC_Layer_Parameter_Config(u8 layerx,u32 bufaddr,u8 pixformat,u8 alpha,u8 alpha0,u8 bfac1,u8 bfac2,u32 bkcolor)
{
	LTDC_LayerCfgTypeDef pLayerCfg;
	
	pLayerCfg.WindowX0=0;                       //窗口起始X坐标
	pLayerCfg.WindowY0=0;                       //窗口起始Y坐标
	pLayerCfg.WindowX1=lcdltdc.pwidth;          //窗口终止X坐标
	pLayerCfg.WindowY1=lcdltdc.pheight;         //窗口终止Y坐标
	pLayerCfg.PixelFormat=pixformat;		    //像素格式
	pLayerCfg.Alpha=alpha;				        //Alpha值设置，0~255,255为完全不透明
	pLayerCfg.Alpha0=alpha0;			        //默认Alpha值
	pLayerCfg.BlendingFactor1=(u32)bfac1<<8;    //设置层混合系数
	pLayerCfg.BlendingFactor2=(u32)bfac2<<8;	//设置层混合系数
	pLayerCfg.FBStartAdress=bufaddr;	        //设置层颜色帧缓存起始地址
	pLayerCfg.ImageWidth=lcdltdc.pwidth;        //设置颜色帧缓冲区的宽度    
	pLayerCfg.ImageHeight=lcdltdc.pheight;      //设置颜色帧缓冲区的高度
	pLayerCfg.Backcolor.Red=(u8)(bkcolor&0X00FF0000)>>16;   //背景颜色红色部分
	pLayerCfg.Backcolor.Green=(u8)(bkcolor&0X0000FF00)>>8;  //背景颜色绿色部分
	pLayerCfg.Backcolor.Blue=(u8)bkcolor&0X000000FF;        //背景颜色蓝色部分
	HAL_LTDC_ConfigLayer(&LTDC_Handler,&pLayerCfg,layerx);   //设置所选中的层
}  



//LCD初始化函数
void LTDC_Init(void)
{   


	lcdltdc.pwidth=480;			    //面板宽度,单位:像素
	lcdltdc.pheight=272;		    //面板高度,单位:像素
//	lcdltdc.hsw=1;				    //水平同步宽度
//	lcdltdc.vsw=1;				    //垂直同步宽度
//	lcdltdc.hbp=40;				    //水平后廊
//	lcdltdc.vbp=8;				    //垂直后廊
//	lcdltdc.hfp=5;				    //水平前廊
//	lcdltdc.vfp=8;				    //垂直前廊
	LTDC_Clk_Set(288,4,RCC_PLLSAIDIVR_8);   //设置像素时钟 9Mhz 

	
	lcddev.width=lcdltdc.pwidth;
	lcddev.height=lcdltdc.pheight;
    
    lcdltdc.pixsize=2;				//每个像素占2个字节
	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;

    //LTDC配置
    LTDC_Handler.Instance=LTDC;
    LTDC_Handler.Init.HSPolarity=LTDC_HSPOLARITY_AL;         //水平同步极性
    LTDC_Handler.Init.VSPolarity=LTDC_VSPOLARITY_AL;         //垂直同步极性
    LTDC_Handler.Init.DEPolarity=LTDC_DEPOLARITY_AL;         //数据使能极性
	LTDC_Handler.Init.PCPolarity=LTDC_PCPOLARITY_IPC;   	//像素时钟极性
	
    LTDC_Handler.Init.HorizontalSync=3;//lcdltdc.hsw-1;          //水平同步宽度
    LTDC_Handler.Init.VerticalSync=3;//lcdltdc.vsw-1;            //垂直同步宽度
    LTDC_Handler.Init.AccumulatedHBP=46;//lcdltdc.hsw+lcdltdc.hbp-1; //水平同步后沿宽度
    LTDC_Handler.Init.AccumulatedVBP=15;//lcdltdc.vsw+lcdltdc.vbp-1; //垂直同步后沿高度
    LTDC_Handler.Init.AccumulatedActiveW=526;//lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth-1;//有效宽度
    LTDC_Handler.Init.AccumulatedActiveH=287;//lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight-1;//有效高度
    LTDC_Handler.Init.TotalWidth=534;//lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth+lcdltdc.hfp-1;   //总宽度
    LTDC_Handler.Init.TotalHeigh=295;//lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight+lcdltdc.vfp-1;  //总高度
	
    LTDC_Handler.Init.Backcolor.Red=0;           //屏幕背景层红色部分
    LTDC_Handler.Init.Backcolor.Green=0;         //屏幕背景层绿色部分
    LTDC_Handler.Init.Backcolor.Blue=0;          //屏幕背景色蓝色部分
	
    HAL_LTDC_Init(&LTDC_Handler);

	//层配置
	LTDC_Layer_Parameter_Config(0,(u32)ltdc_framebuf[0],LCD_PIXFORMAT,255,0,6,7,0X000000);//层参数配置
	//LTDC_Layer_Parameter_Config(1,0xc0200000,LCD_PIXFORMAT,255,0,6,7,0X000000);//层参数配置 加了有问题
	
	LTDC_Layer_Window_Config(0,0,0,lcdltdc.pwidth,lcdltdc.pheight);	//层窗口配置,以LCD面板坐标系为基准,不要随便修改!
	//LTDC_Layer_Window_Config(1,0,0,lcdltdc.pwidth,lcdltdc.pheight);	//层窗口配置,以LCD面板坐标系为基准,不要随便修改!
	
 	LTDC_Display_Dir(1);			//0竖屏1横屏
	LTDC_Select_Layer(0); 			//选择第1层
	LCD_LED=1;         		        //点亮背光
    LTDC_Clear(BLACK);			//清屏
}




void HAL_LTDC_MspInit(LTDC_HandleTypeDef* ltdcHandle)
{

   GPIO_InitTypeDef GPIO_InitStruct = {0};
 // if(ltdcHandle->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */

  /* USER CODE END LTDC_MspInit 0 */
    /* LTDC clock enable */
	 
   __HAL_RCC_LTDC_CLK_ENABLE();
  
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
	  
	//鍒濆鍖朠F9锛孡CD 13.2V鑳屽厜寮曡剼
	GPIO_InitStruct.Pin=GPIO_PIN_9;                //PF9鎺ㄦ尳杈撳嚭锛屾帶鍒惰儗鍏?
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;      //鎺ㄦ尳杈撳嚭
	GPIO_InitStruct.Pull=GPIO_PULLUP;              //涓婃媺        
	GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;         //楂橀€?
	HAL_GPIO_Init(GPIOF,&GPIO_InitStruct);


	//鍒濆鍖朠C8锛岃儗鍏夊紩鑴?
	GPIO_InitStruct.Pin=GPIO_PIN_8;                //PC8鎺ㄦ尳杈撳嚭锛屾帶鍒惰儗鍏?
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;      //鎺ㄦ尳杈撳嚭
	GPIO_InitStruct.Pull=GPIO_PULLUP;              //涓婃媺        
	GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;         //楂橀€?
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
	


    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;              
    GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;      
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_10 
                          |GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5 
                          |GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN LTDC_MspInit 1 */

  /* USER CODE END LTDC_MspInit 1 */
  

  }
}



