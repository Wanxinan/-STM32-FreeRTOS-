#include "Flash.h"



void flashClean(void){
		FLASH_Unlock();//解锁
		FLASH_ErasePage(FLASH_ADDRESS);//页擦除
		FLASH_Lock();//加锁
}
void changeFlashFor32(uint32_t addr, uint32_t data)
{
		FLASH_Unlock();							// 解锁
		FLASH_ProgramWord(addr, data);			// 32bits
		FLASH_Lock();							// 加锁
}
// 获取状态
int getFlashStatus(void){
	int status = *((__IO uint32_t *)(FLASH_ADDRESS));
	return status;
}

void readFlash(char *buffer) {
	
	int buf_base_addr = FLASH_ADDRESS+4;
	for (int i = 0; i < FLASH_STRING_BUF_LEN; i++) {
		 buffer[i] = *(__IO uint8_t *)(buf_base_addr + i);
	}
}

void setFlash(char *buffer){
	
	 flashClean();
	
   changeFlashFor32(FLASH_ADDRESS,0);
   for (int i = 0; i < FLASH_STRING_BUF_LEN; i += 4) {
       changeFlashFor32(FLASH_ADDRESS+4+i, *(int *)(buffer+i));
   }
	 
}


