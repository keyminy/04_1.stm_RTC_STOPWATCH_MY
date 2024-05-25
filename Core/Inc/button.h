/*
 * button.h
 *
 * Created: 2024-04-25 오후 1:15:17
 *  Author: kccistc
 */ 
#include "main.h"   // for GPIO Handling
#include "def.h"

#define BUTTON0   0
#define BUTTON1   1
#define BUTTON2   2

#define BUTTON_PRESS 0   // 버튼을 누르면 high  active-low
#define BUTTON_RELEASE 1  // 버튼을 떼면 low
#define BUTTON_NUMBER 3   // 버튼 갯수

int get_button(GPIO_TypeDef  *GPIO, int button_pin, int button_num);
void button0_check(void);
void stop_watch_state_chk(Stopwatch* pStopwatch, Min2Sec_Clock* pMin2sec_clock);
void min2sec_clock_state_chk(Stopwatch* pStopwatch, Min2Sec_Clock* pMin2sec_clock);
