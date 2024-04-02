#ifndef SERVO_PERIPH_H
#define SERVO_PERIPH_H

#include "util_pwm.h"

#define SERVO_MAX_ANGLE 350

/************舵机状态枚举************/
typedef enum {
	Servo_ON = 1,
	Servo_OFF = 0
}Servo_ServoStateEnum; 

/************舵机结构体************/
typedef struct {
	PWM_PWMHandleTypeDef pwm;
	float angle;
	Servo_ServoStateEnum state;
}Servo_ServoTypeDef;


void Servo_InitServo(Servo_ServoTypeDef* servo, TIM_HandleTypeDef* htim, uint32_t ch);
void Servo_StartServo(Servo_ServoTypeDef* servo);
void Servo_StopServo(Servo_ServoTypeDef* servo);
void Servo_SetServoAngle(Servo_ServoTypeDef* servo, float angle);
float Servo_GetServoAngle(Servo_ServoTypeDef* servo);

#endif