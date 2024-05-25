/*
 * button.c
 *
 * Created: 2024-04-25 오후 1:30:31
 *  Author: kccistc
 */ 
#include "button.h" 

extern Stopwatch* main_stopwatch_ptr;
extern Min2Sec_Clock* main_min2secClock_ptr;

unsigned char button_status[BUTTON_NUMBER] =
	{BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE};
	

char button0_count=0;

void button0_check(void)
{
	// 버튼을 한번 울렀다 떼었는가 ?
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0)== BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 4;
	}
}

void stop_watch_state_chk(Stopwatch* pStopwatch, Min2Sec_Clock* pMin2sec_clock)
{
	if(
		pMin2sec_clock->state == CLOCK_IDLE
	){
		switch(pStopwatch->state){
		case STOPWATCH_IDLE:
			pStopwatch->ms_count = 0;
			pStopwatch->sec_count = 0;
			if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin,BUTTON1) == BUTTON_PRESS) {
				// go to running state
				pStopwatch->ms_count = 0;
				pStopwatch->sec_count = 0;
				pStopwatch->state = STOPWATCH_RUNNING;
			}
			break;
		case STOPWATCH_RUNNING:
			// display stopwatch
			// Switch to Min2Sec_Clock
			if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS) {
				pStopwatch->state = STOPWATCH_IDLE;
				pMin2sec_clock->state = CLOCK_RUNNING;
			}
			else if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin,BUTTON1) == BUTTON_PRESS)
				pStopwatch->state = STOPWATCH_PAUSED;
			break;
		case STOPWATCH_PAUSED:
			// Switch to Min2Sec_Clock
			if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS) {
				pStopwatch->state = STOPWATCH_IDLE;
				pMin2sec_clock->state = CLOCK_RUNNING;
			}
			else if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin,BUTTON1) == BUTTON_PRESS)
				pStopwatch->state = STOPWATCH_RUNNING;
			else if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin,BUTTON2) == BUTTON_PRESS) {
				pStopwatch->state = STOPWATCH_IDLE;
				pMin2sec_clock->state = CLOCK_RUNNING;
			};
			break;
		}
	}
	return;
}

void min2sec_clock_state_chk(Stopwatch* pStopwatch, Min2Sec_Clock* pMin2sec_clock)
{
	if(pMin2sec_clock->state != CLOCK_IDLE && pStopwatch->state == STOPWATCH_IDLE){
	switch (pMin2sec_clock->state){
		case CLOCK_RUNNING:
			if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS) {
				pStopwatch->state = STOPWATCH_RUNNING; // go to stopwatch mode
				pMin2sec_clock->state = CLOCK_IDLE;
			}
			// change second
			else if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin,BUTTON1) == BUTTON_PRESS) {
				pMin2sec_clock->state = CHANGE_SEC;
			}
			break;
		case CHANGE_SEC:
			if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS) {
				// Back to CLOCK_RUNNING
				pMin2sec_clock->state = CLOCK_RUNNING;
			}else if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin,BUTTON1) == BUTTON_PRESS){
				pMin2sec_clock->state = CHANGE_MIN;
			}
			break;
		case CHANGE_MIN:
			if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin,BUTTON1) == BUTTON_PRESS){
				pMin2sec_clock->state = CHANGE_SEC;
			}
			break;
		}
	}
	return;
}


// 버튼이 온전하게 눌렸다 떼어진 상태 이면 1을 리턴 한다. 
int get_button(GPIO_TypeDef  *GPIO, int button_pin, int button_num)  // 예) GPIOC BUTTON0 4
{         
	int current_state; 

	current_state = HAL_GPIO_ReadPin(GPIO, button_pin);  //  active low press:0 , Release:1 버튼값을 읽는다.
	if (current_state == BUTTON_PRESS && button_status[button_num]==BUTTON_RELEASE)  // 버튼이 처음 눌려진상태
	{
		HAL_Delay(60);   // 노이즈가 지나가기를 기다린다
		button_status[button_num]=BUTTON_PRESS;  // 처음 눌려진 상태가 아니다
		return BUTTON_RELEASE;        // 아직은 완전히 눌렸다 떼어진 상태가 아니다.
	}     
	else if (button_status[button_num]==BUTTON_PRESS && current_state==BUTTON_RELEASE)
	       // 버튼이 이전에 눌러진 상태였으며 현재는 떼어진 상태
	{
		button_status[button_num]=BUTTON_RELEASE;   // 다음 버튼 상태를 체크하기위해 초기화
		HAL_Delay(60);   // 노이즈가 지나가기를 기다린다
	    return BUTTON_PRESS;    // 완전히 눌렸다 떼어진 상태임을 리턴
	}
	 
	 return BUTTON_RELEASE;   // 아직 완전히 스위치를 눌렀다 뗀 상태가 아닌경우 나
	             // 스위치가 open 된 상태                   	
}
