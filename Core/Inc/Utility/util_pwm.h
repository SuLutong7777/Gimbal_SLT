
#ifndef PWM_UTIL_H
#define PWM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "tim.h"

/*****************PWM状态****************/
typedef enum {
	PWM_OFF = 0,
	PWM_ON = 1
}PWM_PWMStateEnum;

/*****************PWM结构体****************/
typedef struct {
	TIM_HandleTypeDef* htim;   //定时器句柄
	uint32_t ch;               //定时器通道
	uint32_t clk;              //定时器总线时钟频率
	uint32_t period;           //PWM波形周期
	TIM_OC_InitTypeDef conf;   //PWM输出设置
	float duty;                //波形占空比
	uint32_t freq;             //PWM波形频率
	PWM_PWMStateEnum state;    //PWM是否开启
}PWM_PWMHandleTypeDef;

void PWM_InitPWM(PWM_PWMHandleTypeDef* pwm, TIM_HandleTypeDef* htim, uint32_t ch);
void PWM_StartPWM(PWM_PWMHandleTypeDef* pwm);
void PWM_StopPWM(PWM_PWMHandleTypeDef* pwm);
void PWM_SetPWMDuty(PWM_PWMHandleTypeDef* pwm, float duty);
void PWM_SetPWMFreq(PWM_PWMHandleTypeDef* pwm, uint32_t freq);

#endif

#ifdef __cplusplus
}
#endif
