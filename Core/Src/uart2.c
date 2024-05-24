#include "uart2.h"
#include "led.h"

extern UART_HandleTypeDef huart2;
extern uint8_t rx_data;
int front = 0;
int rear = 0;
uint8_t rx_buff[COMMAND_NUMBER][COMMAND_LENGTH];

// move from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2){
		//static변수는 함수가 다음에 실행되어도,그 값을 기억하기위해 사용한다.
		volatile static int i;
		volatile unsigned char data;
		data = rx_data;

		//printf("%c",data);
		if(data == '\r' || data == '\n'){
			rx_buff[rear][i] = '\0'; //문장의 끝을 알리는 null sign을 insert함.
			i = 0; // i를 초기화 : 다음 문장을 입력받을 준비
			rear++;
			rear %= COMMAND_NUMBER;
		}else{
			rx_buff[rear][i++] = data;
			if(i >= COMMAND_NUMBER) i = 0; // prevent buffer overflow
		}

		//그 다음 인터럽트를 받게 해줘야한다.
		HAL_UART_Receive_IT(&huart2, &rx_data, 1); // assign RX INT
	}

}

void pc_command_processing()
{
	char* cmd;
	char isSettime[] = "settime";
	if( front != rear){
		cmd = rx_buff[front]; //&rx_buff[front][0]로 // 2차원 배열의 0번째 주소를 넘겨줌
		printf("%s\n",cmd);
		front = (front+1) % COMMAND_NUMBER;

		if(strncmp(cmd,isSettime,strlen(isSettime)) == 0){
			set_rtc(&cmd[7]);
		}
	}
}
