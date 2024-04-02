#include "util_flash.h"

/******************** FLASH写入程序 *********************/
void Flash_WriteFlash(int page, uint32_t Data[], uint32_t addr) {
    /* 解锁FLASH */
    HAL_FLASH_Unlock();

    /* 擦除FLASH */
    FLASH_EraseInitTypeDef FlashSet;                // 初始化FLASH_EraseInitTypeDef
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;     // 擦除方式页擦除FLASH_TYPEERASE_PAGES，块擦除FLASH_TYPEERASE_MASSERASE
    FlashSet.PageAddress = addr;                    // 擦除地址
    FlashSet.NbPages = 1;                           // 擦除页数
    
    uint32_t PageError = 0;                         // 设置PageError，调用擦除函数
    HAL_FLASHEx_Erase(&FlashSet, &PageError);

    /* 对FLASH烧写 */
    for(int i = 0; i < page; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + 4 * i, Data[i]);
    }

    /* 锁住FLASH */
    HAL_FLASH_Lock();
}

/******************** FLASH读取内容 *********************/
void Flash_ReadFlash(int page, uint32_t Data[], uint32_t addr) {
    for (int i = 0; i < page; i++) {
        Data[i] = *(__IO uint32_t*)(addr + i * 4);
    }
}