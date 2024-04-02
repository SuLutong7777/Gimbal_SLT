#ifndef UART_UTIL_H
#define UART_UTIL_H

#include "stm32f1xx_hal.h" 
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx.h"

void Uart_ReceiveHandler(UART_HandleTypeDef* huart);
void Uart_RxIdleCallback(UART_HandleTypeDef* huart);
void Uart_InitUartDMA(UART_HandleTypeDef* huart);
HAL_StatusTypeDef Uart_ReceiveDMA(UART_HandleTypeDef* huart, uint8_t rxdata[], uint32_t size);
uint16_t Uart_DMACurrentDataCounter(DMA_HandleTypeDef* dma_handle);
void Uart_SendMessageDMA(UART_HandleTypeDef* huart, uint8_t txdata[], uint16_t size);
void Uart_ErrorHandler(uint32_t ret);

#endif