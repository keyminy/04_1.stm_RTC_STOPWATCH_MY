#include "led.h"
#include "button.h"

void led_all_on(void);
void led_all_off(void);
void led2_toggle(void);
void led_on_up();
void led_on_down();
void ledbar0_toggle(void);
void ledbar1_toggle(void);
void ledbar2_toggle(void);
void led_main(void);

extern char button0_count;

// one button처리 BUTTON0
// 1: led_all_on
// 2: led_all_off
// 3: led_on_up
// 4: led_on_down

// BUTTON1
// led_keepon_up  <--> led_keepon_down

// BUTTON2
// led_flower_on  <--> led_flower_off

void led_main(void)
{

	switch(button0_count)
	{
	case 0:
		led_all_on();
		break;
	case 1:
		led_all_off();
		break;
	case 2:
		led_on_up();
		break;
	case 3:
		led_on_down();
		break;
	}
}

void led_all_on(void)
{
	HAL_GPIO_WritePin(GPIOB, 0xff, 1);
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|
	//		                 GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
}

void led_all_off(void)
{
	HAL_GPIO_WritePin(GPIOB, 0xff, 0);
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|
	//		                 GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
}

void ledbar0_toggle(void)
{
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
}

void ledbar1_toggle(void)
{
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
}

void ledbar2_toggle(void)
{
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
}

void led2_toggle(void)
{
	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

// 0->1.....->7
void led_on_up()
{
	for(int i=0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x01 << i, 1);
		HAL_Delay(200);
	}
}

// 7->6->5....->0
void led_on_down()
{
	for(int i=0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, 1);
		HAL_Delay(200);
	}
}

// 0->1.....->7  기존 on된것 끄지 말고 유지
void led_keepon_up()
{
	for (int i=0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x01 << i, 1);
		HAL_Delay(200);
	}
}
// 7->6->5....->0 기존 on된것 끄지 말고 유지
void led_keepon_down()
{
	for (int i=0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, 1);
		HAL_Delay(200);
	}
}

void flower_on()
{
	HAL_GPIO_WritePin(GPIOB, 0xff, GPIO_PIN_RESET);

	for(int i=0; i < 4 ; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x10 << i | 0x08 >> i, GPIO_PIN_SET);
		HAL_Delay(300);
	}
}
void flower_off()
{
	HAL_GPIO_WritePin(GPIOB, 0xff, GPIO_PIN_SET);


	for(int i=0; i < 4 ; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i | 0x01 << i, GPIO_PIN_RESET);
		HAL_Delay(300);
	}
}

