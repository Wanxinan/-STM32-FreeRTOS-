#include "Dog.h"

int dogInit(void){
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  // 允许修改
		IWDG_SetPrescaler(IWDG_Prescaler_128);        // 设置预分频
		IWDG_SetReload(4095);  // 设置重装值
		IWDG_Enable();  // 启动 IWDG
	
		return 0;
}
