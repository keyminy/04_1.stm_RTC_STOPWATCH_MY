#include "main.h" // GPIO/HAL정보
#include "def.h"
#include "button.h"
#include <string.h> // for strncpy etc
#include <stdlib.h> // atoi itoa

extern RTC_HandleTypeDef hrtc;
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);

void get_rtc_date_time(Stopwatch* pStopwatch, Min2Sec_Clock* pMin2sec_clock);
unsigned char bcd2dec(unsigned char byte);
unsigned char dec2bcd(unsigned char byte);
void set_rtc(char* date_time);

RTC_TimeTypeDef sTime = {0}; // 시각 정보(=특정 시점을 의미, from~to가 아님)
RTC_DateTypeDef sDate = {0}; // 날짜 정보

// uint8_t Year;
// 예)24년의 Year에 저장된 data format
// 7654 3210
// ---- ----
// 0010 0100
//   2   4
// ===> 24
unsigned char bcd2dec(unsigned char byte){
	// 받아온 bcd2dec(sDate.Year)
	// Year정보를 Decimal로
	unsigned char high,low;
	low = byte & 0x0f; // 상위 4bit를 0으로 날림,하위 4bit만 취함
	// 상위 비트 구하기
	// 0000 0010 * 10 = 20이되겠네?
	high = (byte >> 4) * 10; // 앞에는 날라간단 말이야?라고함
	return high+low; // 더하면 24가 온전하게 들어가지 않겠어요?
}


// RTC에서 날짜와 시각정보를 읽어오는 함수
void get_rtc_date_time(Stopwatch* pStopwatch, Min2Sec_Clock* pMin2sec_clock){
	static RTC_TimeTypeDef oldTime; // 자동으로 0으로 들어감
	char lcdbuff[40];

	// =============================================
	/* clock mode */
	// =============================================
	if(pMin2sec_clock->state==CLOCK_RUNNING)
	{
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
		if(oldTime.Seconds != sTime.Seconds){
			// 현재 update된 정보를 출력한다.(1초에 1번씩 출력)
			// YYYY-MM-DD HH:mm:ss
			printf("%02d-%02d %02d:%02d:%02d\n"
					,bcd2dec(sDate.Month)
					,bcd2dec(sDate.Date)
					,bcd2dec(sTime.Hours)
					,bcd2dec(sTime.Minutes)
					,bcd2dec(sTime.Seconds)
			);
			/* 분초 시계 */
			sprintf(lcdbuff,"%02d-%02d %02d:%02d:%02d"
					,bcd2dec(sDate.Month)
					,bcd2dec(sDate.Date)
					,bcd2dec(sTime.Hours)
					,bcd2dec(sTime.Minutes)
					,bcd2dec(sTime.Seconds)
			); // LCD에서 '\n'은 못알아먹는다.
			move_cursor(0, 0); //0row,0column으로 커서 이동
			lcd_string(lcdbuff); // '\0'를 만날때까지 찍는다
		}
	}
	else if
	(
		pMin2sec_clock->state==CHANGE_SEC
		|| pMin2sec_clock->state==CHANGE_MIN
	)
	{
		// If clock is changing seconds or minutes
		switch (pMin2sec_clock->state){
			case CHANGE_SEC:
				if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin,BUTTON2) == BUTTON_PRESS){
					sTime.Seconds = dec2bcd((bcd2dec(sTime.Seconds) + 1));
				}
				break;
			case CHANGE_MIN:
				if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin,BUTTON2) == BUTTON_PRESS){
					sTime.Minutes = dec2bcd((bcd2dec(sTime.Minutes) + 1));
				}
				break;
		}
		// RTC에 적용
		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
		/* 분초 시계 */
		sprintf(lcdbuff,"%02d-%02d %02d:%02d:%02d"
				,bcd2dec(sDate.Month)
				,bcd2dec(sDate.Date)
				,bcd2dec(sTime.Hours)
				,bcd2dec(sTime.Minutes)
				,bcd2dec(sTime.Seconds)
		); // LCD에서 '\n'은 못알아먹는다.
		move_cursor(0, 0); //0row,0column으로 커서 이동
		lcd_string(lcdbuff); // '\0'를 만날때까지 찍는다
	}

	// =============================================
	/* stopwatch */
	// =============================================
	if(pStopwatch->state == STOPWATCH_RUNNING || pStopwatch->state == STOPWATCH_PAUSED){
		printf("Stop watch running, %02d-%02d %02d\n"
				,pStopwatch->sec_count/60%60
				,pStopwatch->sec_count%60
				,pStopwatch->ms_count/10%100
		);
		sprintf(lcdbuff,"%02d:%02d:%02d"
				,pStopwatch->sec_count/60%60
				,pStopwatch->sec_count%60
				,pStopwatch->ms_count/10%100
		);
		move_cursor(1, 0); //0row,0column으로 커서 이동
		lcd_string(lcdbuff); // '\0'를 만날때까지 찍는다
	}else if(pStopwatch->state == STOPWATCH_IDLE){
		sprintf(lcdbuff,"%02d:%02d:%02d"
				,0
				,0
				,0
		);
		move_cursor(1, 0); //0row,0column으로 커서 이동
		lcd_string(lcdbuff); // '\0'를 만날때까지 찍는다
	}
	oldTime.Seconds = sTime.Seconds;
}

// 10진수를 BCD Format으로 변환
// 10진수로 24 -> 0001_1000(2진수)
// bcd로 0010_0100 변환해야함
unsigned char dec2bcd(unsigned char byte){
	// 받아온 bcd2dec(sDate.Year)
	// Year정보를 Decimal로
	unsigned char high,low;
	high = (byte/10) << 4;
	low = byte % 10;
	return high+low; // 더하면 24가 온전하게 들어가지 않겠어요?
}

// 시각 보정 기능
// setrtc240524141800 (2024년 05월 24일 14시 18분 00초)
// 240524141800를 넘겨 받는다(주소로, call by reference)
void set_rtc(char* date_time){
	char yy[4],mm[4],dd[4]; // date
	char hh[4],min[4],sec[4]; // time정보

	strncpy(yy,date_time,2);
	strncpy(mm,date_time+2,2); // date_time+2 = &date_time[2]
	strncpy(dd,date_time+4,2);

	strncpy(hh,date_time+6,2);
	strncpy(min,date_time+8,2);
	strncpy(sec,date_time+10,2);

	// 1.ascii -> int변환
	sDate.Year = dec2bcd(atoi(yy));
	sDate.Month = dec2bcd(atoi(mm));
	sDate.Date = dec2bcd(atoi(dd));

	// 2.int -> bcd
	sTime.Hours = dec2bcd(atoi(hh));
	sTime.Minutes = dec2bcd(atoi(min));
	sTime.Seconds = dec2bcd(atoi(sec));
	// 3.RTC에 적용
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}


