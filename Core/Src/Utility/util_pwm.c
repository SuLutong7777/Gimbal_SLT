#include "util_pwm.h"


/*******************PWM初始化*********************/
void PWM_InitPWM(PWM_PWMHandleTypeDef* pwm, TIM_HandleTypeDef* htim, uint32_t ch) {
    pwm->htim   = htim;
    pwm->ch     = ch;
    pwm->state  = PWM_OFF;
    pwm->duty   = 0.0;
    pwm->clk    = 72000000;                          // APB1总线频率
    pwm->period = 19999;                             // 时钟数
    
    pwm->conf.OCMode        = TIM_OCMODE_PWM1;
    pwm->conf.OCPolarity    = TIM_OCPOLARITY_HIGH;   // 开始为高电平
    pwm->conf.OCFastMode    = TIM_OCFAST_DISABLE;
    
    PWM_SetPWMFreq(pwm, 50);
}

/*******************PWM开启*********************/
void PWM_StartPWM(PWM_PWMHandleTypeDef* pwm) {
    if (pwm->state == PWM_OFF) {
        HAL_TIM_PWM_Start(pwm->htim, pwm->ch); 
        pwm->state = PWM_ON;
    }
}

/*******************PWM关闭*********************/
void PWM_StopPWM(PWM_PWMHandleTypeDef* pwm) {
    if (pwm->state == PWM_ON) {
        HAL_TIM_PWM_Stop(pwm->htim, pwm->ch);
        pwm->state = PWM_OFF;
    }
}

/*******************PWM设置占空比*********************/
void PWM_SetPWMDuty(PWM_PWMHandleTypeDef* pwm, float duty) {
    if (duty == pwm->duty) return;
    PWM_PWMStateEnum last_state = pwm->state;
    PWM_StopPWM(pwm);
    pwm->duty = duty;
    
    pwm->conf.Pulse = pwm->duty * (pwm->htim->Init.Period + 1);   // 占空比的时钟数
    HAL_TIM_PWM_ConfigChannel(pwm->htim, &(pwm->conf), pwm->ch);
    
    if (last_state == PWM_ON) 
        PWM_StartPWM(pwm);

}

/*******************PWM设置频率*********************/
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
    pwm->htim->Init.Prescaler = pwm->clk / (pwm->htim->Init.Period + 1) / freq - 1; // 分频数
    HAL_TIM_PWM_Init(pwm->htim);
    
    PWM_SetPWMDuty(pwm, pwm->duty);
    
    if (last_state == PWM_ON) 
        PWM_StartPWM(pwm);

}
