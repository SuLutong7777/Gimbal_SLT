#include "util_pwm.h"


/*******************PWM��ʼ��*********************/
void PWM_InitPWM(PWM_PWMHandleTypeDef* pwm, TIM_HandleTypeDef* htim, uint32_t ch) {
    pwm->htim   = htim;
    pwm->ch     = ch;
    pwm->state  = PWM_OFF;
    pwm->duty   = 0.0;
    pwm->clk    = 72000000;                          // APB1����Ƶ��
    pwm->period = 19999;                             // ʱ����
    
    pwm->conf.OCMode        = TIM_OCMODE_PWM1;
    pwm->conf.OCPolarity    = TIM_OCPOLARITY_HIGH;   // ��ʼΪ�ߵ�ƽ
    pwm->conf.OCFastMode    = TIM_OCFAST_DISABLE;
    
    PWM_SetPWMFreq(pwm, 50);
}

/*******************PWM����*********************/
void PWM_StartPWM(PWM_PWMHandleTypeDef* pwm) {
    if (pwm->state == PWM_OFF) {
        HAL_TIM_PWM_Start(pwm->htim, pwm->ch); 
        pwm->state = PWM_ON;
    }
}

/*******************PWM�ر�*********************/
void PWM_StopPWM(PWM_PWMHandleTypeDef* pwm) {
    if (pwm->state == PWM_ON) {
        HAL_TIM_PWM_Stop(pwm->htim, pwm->ch);
        pwm->state = PWM_OFF;
    }
}

/*******************PWM����ռ�ձ�*********************/
void PWM_SetPWMDuty(PWM_PWMHandleTypeDef* pwm, float duty) {
    if (duty == pwm->duty) return;
    PWM_PWMStateEnum last_state = pwm->state;
    PWM_StopPWM(pwm);
    pwm->duty = duty;
    
    pwm->conf.Pulse = pwm->duty * (pwm->htim->Init.Period + 1);   // ռ�ձȵ�ʱ����
    HAL_TIM_PWM_ConfigChannel(pwm->htim, &(pwm->conf), pwm->ch);
    
    if (last_state == PWM_ON) 
        PWM_StartPWM(pwm);

}

/*******************PWM����Ƶ��*********************/
void PWM_SetPWMFreq(PWM_PWMHandleTypeDef* pwm, uint32_t freq) {
    if (freq == pwm->freq) return;
    PWM_PWMStateEnum last_state = pwm->state;
    if (freq == 0) {
        PWM_SetPWMDuty(pwm, 0);
        return;
    }

    PWM_StopPWM(pwm);
    pwm->freq = freq;

    pwm->htim->Init.Period = pwm->period;
    pwm->htim->Init.Prescaler = pwm->clk / (pwm->htim->Init.Period + 1) / freq - 1; // ��Ƶ��
    HAL_TIM_PWM_Init(pwm->htim);
    
    PWM_SetPWMDuty(pwm, pwm->duty);
    
    if (last_state == PWM_ON) 
        PWM_StartPWM(pwm);

}
