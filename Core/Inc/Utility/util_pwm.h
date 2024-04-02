
#ifndef PWM_UTIL_H
#define PWM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "tim.h"

/*****************PWM״̬****************/
typedef enum {
	PWM_OFF = 0,
	PWM_ON = 1
}PWM_PWMStateEnum;

/*****************PWM�ṹ��****************/
typedef struct {
	TIM_HandleTypeDef* htim;   //��ʱ�����
	uint32_t ch;               //��ʱ��ͨ��
	uint32_t clk;              //��ʱ������ʱ��Ƶ��
	uint32_t period;           //PWM��������
	TIM_OC_InitTypeDef conf;   //PWM�������
	float duty;                //����ռ�ձ�
	uint32_t freq;             //PWM����Ƶ��
	PWM_PWMStateEnum state;    //PWM�Ƿ���
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
