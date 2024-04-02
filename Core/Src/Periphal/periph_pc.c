#include "periph_pc.h"
#include "periph_servo.h"
#include "util_uart.h"
#include "usart.h"
#include "lib_buff.h"
#include "sys_const.h"
#include "util_flash.h"

//Pc_PcDataTypeDef Pc_PcData;
Gimbal_GimbalTypeDef Gimbal;

/**************Gmibal��ʼ��**************/
void Gimbal_InitGimbal() {
    //Pc_PcDataTypeDef *pc = &Pc_PcData;
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
	
		/*** ��ȡflash����̨id ***/
		uint32_t ID[1];
    Flash_ReadFlash(1, ID, FALSH_ID_ADDRESS);
    gimbal->id = (int)ID[0]; 
	
		/*** ��ʼ����̨id ***/
    //gimbal->id = 1;
                                                                
    gimbal->angle_offset_pitch = Gimbal_Angle_Offset_Pitch[gimbal->id - 1];     // ����pitch��ƫ���
    gimbal->angle_offset_yaw = Gimbal_Angle_Offset_Yaw[gimbal->id - 1];         // ����yaw��ƫ���
    gimbal->state = GIMBAL_OFF;                                                 // ������̨״̬Ϊ�ر�״̬
	
	Gimbal_InitTxData();														// ��ʼ�����ͻ�����
    Servo_InitServo(&(gimbal->servo_pitch), &htim2, TIM_CHANNEL_3);             // ��ʼ���϶��
    Servo_InitServo(&(gimbal->servo_yaw), &htim2, TIM_CHANNEL_4);               // ��ʼ���϶��

	Uart_InitUartDMA(&huart1);                                                  // ��ʼ��DMA,���������ж�Դ
    Uart_ReceiveDMA(&huart1, gimbal->Gimbal_RxData, Const_Gimbal_RX_BUFF_LEN);  // ����DMA����ģʽ�����ڴ��Ͽ���һ���ڴ�����������
}

/**************Gmibal��ʼ�����ͻ�����**************/
void Gimbal_InitTxData() {
	Gimbal_GimbalTypeDef *gimbal = &Gimbal;
	gimbal->Gimbal_TxData[0] = 0x55;
	gimbal->Gimbal_TxData[1] = 0xAA;
	gimbal->Gimbal_TxData[2] = 0x51;
	gimbal->Gimbal_TxData[3] = 0x05;
	gimbal->Gimbal_TxData[4] = 0x00;
	gimbal->Gimbal_TxData[5] = (uint8_t)(gimbal->id);
	gimbal->Gimbal_TxData[6] = 0x00;
	gimbal->Gimbal_TxData[7] = 0x00;
	gimbal->Gimbal_TxData[8] = 0x00;
	gimbal->Gimbal_TxData[9] = 0x00;
	gimbal->Gimbal_TxData[10] = 0x00;
	gimbal->Gimbal_TxData[11] = 0x00;
	gimbal->Gimbal_TxData[12] = 0x00;
	gimbal->Gimbal_TxData[13] = 0x00;
	gimbal->Gimbal_TxData[14] = Gimbal_Sum(gimbal->Gimbal_TxData, 15);
}

/**************Gmibal������**************/
void Gimbal_Heartbeat() {
	Gimbal_GimbalTypeDef *gimbal = &Gimbal;
	if(gimbal->state == GIMBAL_ON) {
			HAL_UART_Transmit_IT(&huart1, gimbal->Gimbal_TxData, 15);
	}
}

/**************Gmibal����֪ͨ**************/
void Gimbal_OnlineNotice() {
	Gimbal_GimbalTypeDef *gimbal = &Gimbal;
	for(int i = 0; i < 10; i++) {
		if(gimbal->state == GIMBAL_ON) {
			HAL_UART_Transmit_IT(&huart1, gimbal->Gimbal_TxData, 15);
		}
		HAL_Delay(1000);
	}
}

/**************Gmibal����**************/
void Gimbal_StartGimbal() {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    if(gimbal->state == GIMBAL_OFF) {
        Servo_StartServo(&(gimbal->servo_pitch));
        Servo_StartServo(&(gimbal->servo_yaw));
    }
    gimbal->state = GIMBAL_ON;
}

/**************Gmibal�ر�**************/
void Gimbal_StopGimbal() {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    if(gimbal->state == GIMBAL_ON) {
        Servo_StopServo(&(gimbal->servo_pitch));
        Servo_StopServo(&(gimbal->servo_yaw));
    }
    gimbal->state = GIMBAL_OFF;
}

/**************Gmibal�趨��̨ID**************/
void Gimbal_SetGimbalID(int id) {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    gimbal->id = id;
    uint32_t data[1] = { (uint32_t)id };
    Flash_WriteFlash(1, data, FALSH_ID_ADDRESS);                        // ����idд��FLASH

    gimbal->Gimbal_TxData[5] = (uint8_t)(gimbal->id);                   // �ı䷢�ͻ�����
    gimbal->Gimbal_TxData[14] = Gimbal_Sum(gimbal->Gimbal_TxData, 15);
}

/**************Gmibal�趨��̨����Ƕ�**************/
void Gimbal_SetGimbalAngle(float angle_pitch, float angle_yaw) {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    Servo_SetServoAngle(&(gimbal->servo_pitch), angle_pitch);
    Servo_SetServoAngle(&(gimbal->servo_yaw), angle_yaw);
}

/**************Gimbal�������ݽ���**************/  
void Gimbal_DecodeGimbalData(uint8_t* buff, int rxdatalen) {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    if(rxdatalen != 15) return;
		
    switch (buff[3]) {
        case Command_Set_Angle:                                         //// ������̨˫���ת���Ƕ�
            if(gimbal->id == (int)buff[5]) {                            // �ж���̨id�Ƿ���ȷ
                if(Gimbal_SumCheck(buff, rxdatalen) == 1) {             // �ж����У���Ƿ���ȷ
                    buff[4] = 0xC0;
                    float angle_pitch = buff2float(buff + 6);
                    float angle_yaw = buff2float(buff + 10);
                    ////ƫ�ýǺ��޷�
					if(angle_pitch > 35)                                // pitch�޷�
						angle_pitch = 35;
					else if(angle_pitch < -35)
						angle_pitch = -35;
                    angle_pitch += gimbal->angle_offset_pitch;          // pitchƫ�ý�
                    if(angle_pitch > 360)                     
                        angle_pitch -= 360;
					else if(angle_pitch < 0)
							angle_pitch += 360;
                
					angle_yaw += gimbal->angle_offset_yaw;
                    if(angle_yaw > 360)      // yawƫ�ý�
                        angle_yaw -= - 360;
					else if(angle_yaw < 0)
						angle_yaw += 360;
                    Gimbal_SetGimbalAngle(angle_pitch, angle_yaw);
                } else {
                    buff[4] = 0xE0;
                }
				buff[14] = Gimbal_Sum(buff,rxdatalen);
                //Uart_SendMessageDMA(&huart1, buff, rxdatalen);
				HAL_UART_Transmit_IT(&huart1, buff, rxdatalen);
            }
            break;
        case Command_Set_ID:                              //// ������̨id
            if(gimbal->id == (int)buff[5]) {
                if(Gimbal_SumCheck(buff, rxdatalen) == 1) {
                    buff[4] = 0xC0;
                    buff[5] = buff[13];
                    Gimbal_SetGimbalID((int)buff[13]);
                    //gimbal->id = (int)buff[13];
                } else {
                    buff[4] = 0xE0;
                }
				buff[14] = Gimbal_Sum(buff,rxdatalen);
                //Uart_SendMessageDMA(&huart1, buff, rxdatalen);
				HAL_UART_Transmit_IT(&huart1, buff, rxdatalen);
            }
            break;
        case Command_Find_ID:                             //// Ѱ���ض���̨
            if(gimbal->id == (int)buff[5]) {
                if(Gimbal_SumCheck(buff, rxdatalen) == 1) {
                    buff[4] = 0xC0;
                } else {
                    buff[4] = 0xE0;
                }
				buff[14] = Gimbal_Sum(buff,rxdatalen);
                //Uart_SendMessageDMA(&huart1, buff, rxdatalen);
				HAL_UART_Transmit_IT(&huart1, buff, rxdatalen);
            }
            break;
        case Command_Broad_ID:                            //// �㲥Ѱ��id
                if(Gimbal_SumCheck(buff, rxdatalen) == 1) {
                    buff[4] = 0xC0;
                    buff[5] = (uint8_t)(gimbal->id);
                } else {
                    buff[4] = 0xE0;
                }
				buff[14] = Gimbal_Sum(buff,rxdatalen);
                //Uart_SendMessageDMA(&huart1, buff, rxdatalen);
				HAL_UART_Transmit_IT(&huart1, buff, rxdatalen);
            break;
        case Command_Online:                             //// ����֪ͨ
            break;
        case Command_Get_Angle:                          //// ��ȡ�ض���̨��ǰ�Ƕ�
            if(gimbal->id == (int)buff[5]) {
                if(Gimbal_SumCheck(buff, rxdatalen) == 1) {
                    buff[4] = 0xC0;
                    float angle_pitch = Servo_GetServoAngle(&(gimbal->servo_pitch)) - gimbal->angle_offset_pitch;
                    if(angle_pitch < 0) angle_pitch = angle_pitch + 360;
                    float angle_yaw = Servo_GetServoAngle(&(gimbal->servo_yaw)) - gimbal->angle_offset_yaw;
                    if(angle_yaw < 0) angle_yaw = angle_yaw + 360;
                    float2buff(angle_pitch, buff + 6);
                    float2buff(angle_yaw, buff + 10);
                } else {
                    buff[4] = 0xE0;
                }
				buff[14] = Gimbal_Sum(buff,rxdatalen);
                //Uart_SendMessageDMA(&huart1, buff, rxdatalen);
				HAL_UART_Transmit_IT(&huart1, buff, rxdatalen);
            }
            break;
        case Command_Get_Pose:                           //// ��ȡ�ض���̨��ǰλ��
            break;
        default:
            break;
    };
}

/**************Gimbal��֤���У��λ**************/ 
int Gimbal_SumCheck(uint8_t* buff, int rxdatalen) {
    uint8_t sum = 0x00;
    for(int i = 0; i < rxdatalen - 1; i++){
        sum += buff[i];
    }
    if(sum == buff[rxdatalen - 1]) {
			return 1;
		} else {
			return 0;
		}			   
}

/**************Gimbal���У��**************/ 
uint8_t Gimbal_Sum(uint8_t* buff, int rxdatalen) {
	uint8_t sum = 0x00;
    for(int i = 0; i < rxdatalen - 1; i++){
        sum += buff[i];
    }
		return sum;
}

//**************Gmibal�������ݴ���**************//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       **************/
void Gimbal_RxCallback(UART_HandleTypeDef* huart) {
	//Pc_PcDataTypeDef *pc = &Pc_PcData;
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    __HAL_DMA_DISABLE(huart->hdmarx);                               // �ر�DMA����ֹ�������ݱ�����
		int length = Uart_DMACurrentDataCounter(huart->hdmarx);
    //int rxdatalen = Const_Gimbal_RX_BUFF_LEN - Uart_DMACurrentDataCounter(huart->hdmarx); //��������λ��
		int rxdatalen = Const_Gimbal_RX_BUFF_LEN - length;
    Gimbal_DecodeGimbalData(gimbal->Gimbal_RxData, rxdatalen);

    //__HAL_DMA_SET_COUNTER(huart->hdmarx, Const_Pc_RX_BUFF_LEN);
    huart->hdmarx->Instance->CNDTR = (uint16_t)Const_Gimbal_RX_BUFF_LEN;
    __HAL_DMA_ENABLE(huart->hdmarx);
}