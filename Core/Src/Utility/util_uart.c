
#include "util_uart.h"
#include "periph_pc.h"
#include "usart.h"

/****************Uart��ʼ��DMA*******************/
void Uart_InitUartDMA(UART_HandleTypeDef* huart) {
	__HAL_UART_CLEAR_IDLEFLAG(huart);                    // ��������жϱ�־λ
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);         // ���������������ж�
}

/****************UartDMA����*******************/
void Uart_SendMessageDMA(UART_HandleTypeDef* huart, uint8_t txdata[], uint16_t size) {
	uint32_t ret = HAL_UART_Transmit_DMA(huart, txdata, size);                      // DMA���ͺ���
	if(ret != HAL_OK) {
		Uart_ErrorHandler(ret);
	}
}

/****************UartDMA����*******************/         // ����ֵΪ״ֵ̬ // rxdataΪ���ջ������� sizeΪ���մ�С
HAL_StatusTypeDef Uart_ReceiveDMA(UART_HandleTypeDef* huart, uint8_t rxdata[], uint32_t size) {
	if(huart->RxState == HAL_UART_STATE_READY) {         /// �ж�uart�Ƿ����״̬
		if((rxdata == NULL) || (size == 0U)) {           // ���������Ϊ��
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

/****************����ʣ��δʹ�õĽ���λ��*******************/
uint16_t Uart_DMACurrentDataCounter(DMA_HandleTypeDef* dma_handle) {
    /* Return the number of remaining data units for DMAy Streamx */
    return __HAL_DMA_GET_COUNTER(dma_handle);
}
    
/****************Uart�����жϴ���*******************/
void Uart_RxIdleCallback(UART_HandleTypeDef* huart) {
	if(huart == &huart1) {                               
		Gimbal_RxCallback(huart);                              // ����Pc���մ�����
	}
}

/****************Uart�����жϴ���*******************/
void Uart_ErrorHandler(uint32_t ret) {
	while (1)
	{
		return;
	}
	
}

/****************Uart���վ��*******************/
void Uart_ReceiveHandler(UART_HandleTypeDef* huart) {
	
	// �ж��Ƿ�ָ�������ж�λ �Լ� �Ƿ��������ж�Դ
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE)) {
		__HAL_UART_CLEAR_IDLEFLAG(huart);                  // ��������ж�λ
		Uart_RxIdleCallback(huart);                        // ����uart�����жϴ�����
	}
}

