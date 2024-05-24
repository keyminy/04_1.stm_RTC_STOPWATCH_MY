#include "timer.h"

extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim2;

void delay_us(unsigned int us);

// count us timer plus
// 0.000001sec --> 1us : 1개의 pulse소요 시간
// 0.001sec : 1ms  <== 1000us
// 0.1sec : 100ms
//
void delay_us(unsigned int us)
{
	// TCNT=0;  AVR
	__HAL_TIM_SET_COUNTER(&htim11,0);   // timer 2번 counter를 clear한다.
	while (__HAL_TIM_GET_COUNTER(&htim11) < us)   // 지정한 pulse count를 만날떄 까지 wait
		;        // NOP
}


