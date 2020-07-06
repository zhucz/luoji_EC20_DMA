#ifndef _LED_H
#define _LED_H
#include "sys.h"


#define LED0 PDout(4)   //LED0
#define LED1 PDout(5)   //LED1

void LED_Init(void);
#endif
