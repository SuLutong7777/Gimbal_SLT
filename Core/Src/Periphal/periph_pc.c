#include "periph_pc.h"
#include "periph_servo.h"
#include "util_uart.h"
#include "usart.h"
#include "lib_buff.h"
#include "sys_const.h"
#include "util_flash.h"

//Pc_PcDataTypeDef Pc_PcData;
Gimbal_GimbalTypeDef Gimbal;

/**************Gmibal初始化**************/
void Gimbal_InitGimbal() {
    //Pc_PcDataTypeDef *pc = &Pc_PcData;
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
	
		/*** 读取flash中云台id ***/
		uint32_t ID[1];
    Flash_ReadFlash(1, ID, FALSH_ID_ADDRESS);
    gimbal->id = (int)ID[0]; 
	
		/*** 初始化云台id ***/
    //gimbal->id = 1;
                                                                
    gimbal->angle_offset_pitch = Gimbal_Angle_Offset_Pitch[gimbal->id - 1];     // 设置pitch轴偏向角
    gimbal->angle_offset_yaw = Gimbal_Angle_Offset_Yaw[gimbal->id - 1];         // 设置yaw轴偏向角
    gimbal->state = GIMBAL_OFF;                                                 // 设置云台状态为关闭状态
	
	Gimbal_InitTxData();														// 初始化发送缓冲区
    Servo_InitServo(&(gimbal->servo_pitch), &htim2, TIM_CHANNEL_3);             // 初始化上舵机
    Servo_InitServo(&(gimbal->servo_yaw), &htim2, TIM_CHANNEL_4);               // 初始化上舵机

	Uart_InitUartDMA(&huart1);                                                  // 初始化DMA,开启空闲中断源
    Uart_ReceiveDMA(&huart1, gimbal->Gimbal_RxData, Const_Gimbal_RX_BUFF_LEN);  // 开启DMA接收模式，在内存上开了一块内存来接收数据
}

/**************Gmibal初始化发送缓冲区**************/
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

/**************Gmibal心跳包**************/
void Gimbal_Heartbeat() {
	Gimbal_GimbalTypeDef *gimbal = &Gimbal;
	if(gimbal->state == GIMBAL_ON) {
			HAL_UART_Transmit_IT(&huart1, gimbal->Gimbal_TxData, 15);
	}
}

/**************Gmibal上线通知**************/
void Gimbal_OnlineNotice() {
	Gimbal_GimbalTypeDef *gimbal = &Gimbal;
	for(int i = 0; i < 10; i++) {
		if(gimbal->state == GIMBAL_ON) {
			HAL_UART_Transmit_IT(&huart1, gimbal->Gimbal_TxData, 15);
		}
		HAL_Delay(1000);
	}
}

/**************Gmibal开启**************/
void Gimbal_StartGimbal() {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    if(gimbal->state == GIMBAL_OFF) {
        Servo_StartServo(&(gimbal->servo_pitch));
        Servo_StartServo(&(gimbal->servo_yaw));
    }
    gimbal->state = GIMBAL_ON;
}

/**************Gmibal关闭**************/
void Gimbal_StopGimbal() {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    if(gimbal->state == GIMBAL_ON) {
        Servo_StopServo(&(gimbal->servo_pitch));
        Servo_StopServo(&(gimbal->servo_yaw));
    }
    gimbal->state = GIMBAL_OFF;
}

/**************Gmibal设定云台ID**************/
void Gimbal_SetGimbalID(int id) {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    gimbal->id = id;
    uint32_t data[1] = { (uint32_t)id };
    Flash_WriteFlash(1, data, FALSH_ID_ADDRESS);                        // 将新id写入FLASH

    gimbal->Gimbal_TxData[5] = (uint8_t)(gimbal->id);                   // 改变发送缓冲区
    gimbal->Gimbal_TxData[14] = Gimbal_Sum(gimbal->Gimbal_TxData, 15);
}

/**************Gmibal设定云台舵机角度**************/
void Gimbal_SetGimbalAngle(float angle_pitch, float angle_yaw) {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    Servo_SetServoAngle(&(gimbal->servo_pitch), angle_pitch);
    Servo_SetServoAngle(&(gimbal->servo_yaw), angle_yaw);
}

/**************Gimbal接收数据解码**************/  
void Gimbal_DecodeGimbalData(uint8_t* buff, int rxdatalen) {
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    if(rxdatalen != 15) return;
		
    switch (buff[3]) {
        case Command_Set_Angle:                                         //// 设置云台双舵机转动角度
            if(gimbal->id == (int)buff[5]) {                            // 判断云台id是否正确
                if(Gimbal_SumCheck(buff, rxdatalen) == 1) {             // 判断求和校验是否正确
                    buff[4] = 0xC0;
                    float angle_pitch = buff2float(buff + 6);
                    float angle_yaw = buff2float(buff + 10);
                    ////偏置角和限幅
					if(angle_pitch > 35)                                // pitch限幅
						angle_pitch = 35;
					else if(angle_pitch < -35)
						angle_pitch = -35;
                    angle_pitch += gimbal->angle_offset_pitch;          // pitch偏置角
                    if(angle_pitch > 360)                     
                        angle_pitch -= 360;
					else if(angle_pitch < 0)
							angle_pitch += 360;
                
					angle_yaw += gimbal->angle_offset_yaw;
                    if(angle_yaw > 360)      // yaw偏置角
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
        case Command_Set_ID:                              //// 设置云台id
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
        case Command_Find_ID:                             //// 寻找特定云台
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
        case Command_Broad_ID:                            //// 广播寻找id
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
        case Command_Online:                             //// 上线通知
            break;
        case Command_Get_Angle:                          //// 获取特定云台当前角度
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
        case Command_Get_Pose:                           //// 获取特定云台当前位姿
            break;
        default:
            break;
    };
}

/**************Gimbal验证求和校验位**************/ 
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

/**************Gimbal求和校验**************/ 
uint8_t Gimbal_Sum(uint8_t* buff, int rxdatalen) {
	uint8_t sum = 0x00;
    for(int i = 0; i < rxdatalen - 1; i++){
        sum += buff[i];
    }
		return sum;
}

//**************Gmibal接收数据处理**************//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       **************/
void Gimbal_RxCallback(UART_HandleTypeDef* huart) {
	//Pc_PcDataTypeDef *pc = &Pc_PcData;
    Gimbal_GimbalTypeDef *gimbal = &Gimbal;
    __HAL_DMA_DISABLE(huart->hdmarx);                               // 关闭DMA，防止接收数据被覆盖
		int length = Uart_DMACurrentDataCounter(huart->hdmarx);
    //int rxdatalen = Const_Gimbal_RX_BUFF_LEN - Uart_DMACurrentDataCounter(huart->hdmarx); //接收数据位数
		int rxdatalen = Const_Gimbal_RX_BUFF_LEN - length;
    Gimbal_DecodeGimbalData(gimbal->Gimbal_RxData, rxdatalen);

    //__HAL_DMA_SET_COUNTER(huart->hdmarx, Const_Pc_RX_BUFF_LEN);
    huart->hdmarx->Instance->CNDTR = (uint16_t)Const_Gimbal_RX_BUFF_LEN;
    __HAL_DMA_ENABLE(huart->hdmarx);
}