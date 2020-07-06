#ifndef _UPGRADEINIT_H
#define _UPGRADEINIT_H

#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "lcd.h" 
#include "ltdc.h"   
#include "sdram.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ffconf.h" 
#include "ff.h"  
#include "stmflash.h"
#include "exfuns.h"  
#include "text.h"
#include "usbh_usr.h" 
#include "iap.h"
#include "rtc.h"


void  UpgradeInit(void);
void  Upgradeapp(void);
void  system_init(void);


#endif
