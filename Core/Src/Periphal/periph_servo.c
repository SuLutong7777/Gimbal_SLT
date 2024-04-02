#include "periph_servo.h"
#include "util_pwm.h"

/*************初始化舵机*************/
void Servo_InitServo(Servo_ServoTypeDef* servo, TIM_HandleTypeDef* htim, uint32_t ch) {
	servo->state = Servo_OFF;
	PWM_InitPWM(&(servo->pwm), htim, ch);
	PWM_SetPWMFreq(&(servo->pwm), 50);
	Servo_StartServo(servo);
	Servo_SetServoAngle(servo, 100.0f);
}

/**************开启舵机**************/
void Servo_StartServo(Servo_ServoTypeDef* servo) {
	if(servo->state == Servo_OFF)
		PWM_StartPWM(&(servo->pwm));
	
	servo->state = Servo_ON;
}

/**************关闭舵机**************/
void Servo_StopServo(Servo_ServoTypeDef* servo) {
	if(servo->state == Servo_ON)
		PWM_StopPWM(&(servo->pwm));
	
	servo->state = Servo_OFF; 
}

/************改变舵机角度************/
void Servo_SetServoAngle(Servo_ServoTypeDef* servo, float angle) {
	servo->angle = angle;
	float duty = (angle / SERVO_MAX_ANGLE * 2000 + 500) / 20000;
	PWM_SetPWMDuty(&(servo->pwm), duty);
}

/************获取舵机角度************/
float Servo_GetServoAngle(Servo_ServoTypeDef* servo) {
	return servo->angle;
}
