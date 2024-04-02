/********************云台库********************/

#ifndef PC_PERIPH_H
#define PC_PERIPH_H
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#include "periph_servo.h"

#define Const_Gimbal_RX_BUFF_LEN    30

#define Command_Set_Angle       0x01
#define Command_Set_ID          0x02
#define Command_Find_ID         0x03
#define Command_Broad_ID        0x04
#define Command_Online          0x05
#define Command_Get_Angle       0x06
#define Command_Get_Pose        0x07

typedef enum {
    GIMBAL_ON = 1,
    GIMBAL_OFF = 0
}Gimbal_GimbalStateEnum;

/*****************定义Gmibal结构体******************/
typedef struct {
    int id;
    Servo_ServoTypeDef servo_pitch;
    Servo_ServoTypeDef servo_yaw;
    float angle_offset_pitch;                         // pitch轴偏置角
    float angle_offset_yaw;                           // yaw轴偏置角
    Gimbal_GimbalStateEnum state;

    uint8_t Gimbal_RxData[Const_Gimbal_RX_BUFF_LEN];  // 接收缓冲区
	uint8_t Gimbal_TxData[Const_Gimbal_RX_BUFF_LEN];  	// 发送缓冲区
}Gimbal_GimbalTypeDef;

void Gimbal_InitGimbal();															
void Gimbal_InitTxData();
void Gimbal_StartGimbal();
void Gimbal_StopGimbal();
void Gimbal_SetGimbalAngle(float angle_pitch, float angle_yaw);
void Gimbal_SetGimbalID(int id);
void Gimbal_DecodeGimbalData(uint8_t* buff, int rxdatalen);
int Gimbal_SumCheck(uint8_t* buff, int rxdatalen);
uint8_t Gimbal_Sum(uint8_t* buff, int rxdatalen);
void Gimbal_RxCallback(UART_HandleTypeDef* huart);
void Gimbal_OnlineNotice();
void Gimbal_Heartbeat();

#endif