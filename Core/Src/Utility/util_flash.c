#include "util_flash.h"

/******************** FLASHд����� *********************/
void Flash_WriteFlash(int page, uint32_t Data[], uint32_t addr) {
    /* ����FLASH */
    HAL_FLASH_Unlock();

    /* ����FLASH */
    FLASH_EraseInitTypeDef FlashSet;                // ��ʼ��FLASH_EraseInitTypeDef
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;     // ������ʽҳ����FLASH_TYPEERASE_PAGES�������FLASH_TYPEERASE_MASSERASE
    FlashSet.PageAddress = addr;                    // ������ַ
    FlashSet.NbPages = 1;                           // ����ҳ��
    
    uint32_t PageError = 0;                         // ����PageError�����ò�������
    HAL_FLASHEx_Erase(&FlashSet, &PageError);

    /* ��FLASH��д */
    for(int i = 0; i < page; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + 4 * i, Data[i]);
    }

    /* ��סFLASH */
    HAL_FLASH_Lock();
}

/******************** FLASH��ȡ���� *********************/
void Flash_ReadFlash(int page, uint32_t Data[], uint32_t addr) {
    for (int i = 0; i < page; i++) {
        Data[i] = *(__IO uint32_t*)(addr + i * 4);
    }
}