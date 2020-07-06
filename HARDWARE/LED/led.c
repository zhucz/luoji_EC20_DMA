#include "led.h"


//初始化PB1为输出.并使能时钟	    
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();           
	
 	  /*Configure GPIO pins : PD4 PD5 PD7 */
	GPIO_Initure.Pin = GPIO_PIN_4|GPIO_PIN_5;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);	//PB0置1，默认初始化后灯灭
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);	//PB1置1，默认初始化后灯灭
}
