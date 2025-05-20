#ifndef __FLASH_H__
#define __FLASH_H__
#include "Config.h"  

void flashClean(void);
int getFlashStatus(void);
void readFlash(char *buffer);
void setFlash(char *buffer);

#endif

