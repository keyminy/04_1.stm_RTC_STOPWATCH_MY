#ifndef __DEF_H
#define __DEF_H

typedef enum {
    STOPWATCH_IDLE,
    STOPWATCH_RUNNING,
    STOPWATCH_PAUSED
} StopwatchState;

typedef enum {
    CLOCK_IDLE,
    CLOCK_RUNNING,
	/*Note : Stops counting time while you're changing the seconds and minutes */
	CHANGE_SEC,
	CHANGE_MIN
} Min2Sec_ClockState;

typedef struct{
	uint16_t ms_count;
	uint16_t sec_count;
	StopwatchState state;
} Stopwatch;

typedef struct{
	uint16_t ms_count;
	uint16_t sec_count;
	Min2Sec_ClockState state;
} Min2Sec_Clock;

#endif
