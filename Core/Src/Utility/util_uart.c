
#include "util_uart.h"
#include "periph_pc.h"
#include "usart.h"

/****************Uart初始化DMA*******************/
void Uart_InitUartDMA(UART_HandleTypeDef* huart) {
	__HAL_UART_CLEAR_IDLEFLAG(huart);                    // 清除空闲中断标志位
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);         // 开启非阻塞空闲中断
}

/****************UartDMA发送*******************/
void Uart_SendMessageDMA(UART_HandleTypeDef* huart, uint8_t txdata[], uint16_t size) {
	uint32_t ret = HAL_UART_Transmit_DMA(huart, txdata, size);                      // DMA发送函数
	if(ret != HAL_OK) {
		Uart_ErrorHandler(ret);
	}
}

/****************UartDMA接收*******************/         // 返回值为状态值 // rxdata为接收缓冲区， size为接收大小
HAL_StatusTypeDef Uart_ReceiveDMA(UART_HandleTypeDef* huart, uint8_t rxdata[], uint32_t size) {
	if(huart->RxState == HAL_UART_STATE_READY) {         /// 判断uart是否接收状态
		if((rxdata == NULL) || (size == 0U)) {           // 如果接收区为空
			return HAL_ERROR;
		}
		huart->pRxBuffPtr = rxdata;
		huart->RxXferSize = size;
		huart->ErrorCode = HAL_UART_ERROR_NONE;
		HAL_StatusTypeDef ret =  HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)rxdata, size);
		SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
		return ret;
	} else {
		return HAL_BUSY;
	}
}

/****************返回剩余未使用的接收位数*******************/
uint16_t Uart_DMACurrentDataCounter(DMA_HandleTypeDef* dma_handle) {
    /* Return the number of remaining data units for DMAy Streamx */
    return __HAL_DMA_GET_COUNTER(dma_handle);
}
    
/****************Uart空闲中断处理*******************/
void Uart_RxIdleCallback(UART_HandleTypeDef* huart) {
	if(huart == &huart1) {                               
		Gimbal_RxCallback(huart);                              // 进入Pc接收处理函数
	}
}

/****************Uart错误中断处理*******************/
void Uart_ErrorHandler(uint32_t ret) {
	while (1)
	{
		return;
	}
	
}

/****************Uart接收句柄*******************/
void Uart_ReceiveHandler(UART_HandleTypeDef* huart) {
	
	// 判断是否指定空闲中断位 以及 是否开启空闲中断源
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE)) {
		__HAL_UART_CLEAR_IDLEFLAG(huart);                  // 清除空闲中断位
		Uart_RxIdleCallback(huart);                        // 进入uart空闲中断处理函数
	}
}

