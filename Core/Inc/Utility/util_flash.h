#ifndef FLASH_UTIL_H
#define FLASH_UTIL_H

#include "stm32f1xx_hal.h" 
#include "stdint.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

#define FALSH_ID_ADDRESS 0x08007C00    // 存取云台ID的首地址

void Flash_WriteFlash(int page, uint32_t Data[], uint32_t addr);
void Flash_ReadFlash(int page, uint32_t Data[], uint32_t addr);

#endif
