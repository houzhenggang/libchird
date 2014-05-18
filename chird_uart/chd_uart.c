/*-----------------------------------------------------------------*
 * Copyright (C),		Hangzhou chird Technology Ltd
 * FileName:            chird_uart.c
 * Author:             	meifj
 * Version:             
 * Datetime:            
 * Description:         create by meifj 2014-03-23	����Э��ķ�װ
 * Version information: 
 *------------------------------------------------------------*/

#include "chd_uart.h"

//���ʽ : HEAD1 HEAD2 LENGTH TYPE PARAM CHECK LAST
//���յ�������һ���frame�д�ŵ���TYPE PARAM CHECK LAST

static unsigned char send_frame[10] = {FIRST_CHAR, SECOND_CHAR};
unsigned char framedata[10] = {0};
unsigned char frame_finish = 0;

void uart_comm_handler(int8 ch)
{
	static int8 cnt = 0;
	static int8 start = 0;
	static int8 len;

	if(frame_finish == 0 && start == 0){
		if(ch == FIRST_CHAR) cnt = 1;		//��һ���ַ�ΪFIRST_CHAR
		else if(ch == SECOND_CHAR && cnt == 1) cnt = 2;	  //�ڶ����ַ�ΪSECOND_CHAR
		else if(cnt == 2){
			start = 1;		// ���ӽ�������
			len = ch-1;		// ��ֵ���ȣ���ȥ�Լ��ĳ���
			cnt = 0;		// ������0
		}
		else cnt = 0;		// �κδ���Ľ��գ�����cnt��0
		
	}
	else if(start == 1){
		
		frame[cnt++] = ch;
		// ���յ�ָ�������ַ�
		if(cnt == len){
			if(frame[cnt - 1] == LAST_CHAR){
			//	uart_putchar(UART4, 0x88);
				frame_finish = 1; 
			}
			len = 0;
			start = 0;
			cnt = 0;
		}
	}
	else{
		start = 0;
	}
		
}

void chird_uart_send_key(unsigned char idx)
{
	send_frame[2] = CHIRD_UART_KEY_LEN;
	send_frame[3] = idx + CHIRD_UART_KEY1; 
	send_frame[4] = send_frame[2] + send_frame[3];
	send_frame[5] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_KEY_LEN + 2);	
}

void chird_uart_send_slide(unsigned char idx, unsigned char param)
{
	send_frame[2] = CHIRD_UART_SLIDE_LEN;
	send_frame[3] = idx + CHIRD_UART_SLIDE1; 
	send_frame[4] = param;
	send_frame[5] = send_frame[2] + send_frame[3] + send_frame[4];
	send_frame[6] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_SLIDE_LEN + 2);	
}

void chird_uart_send_led(unsigned char idx, unsigned char sw)
{
	send_frame[2] = CHIRD_UART_LED_LEN;
	send_frame[3] = idx + CHIRD_UART_LED1; 
	send_frame[4] = sw;
	send_frame[5] = send_frame[2] + send_frame[3] + send_frame[4];
	send_frame[6] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_LED_LEN + 2);
}

void chird_uart_send_param(unsigned char idx, unsigned char param1, unsigned char param2)
{

	send_frame[2] = CHIRD_UART_PARAM_LEN;
	send_frame[3] = idx + CHIRD_UART_PARAM1; 
	send_frame[4] = param1;
	send_frame[5] = param2;
	send_frame[6] = send_frame[2] + send_frame[3] + send_frame[4] + send_frame[5];
	send_frame[7] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_PARAM_LEN + 2);
}


int chird_uart_Process()
{
	/* ****** ��Linux����ϵͳ�ģ������ڴ˽����ź��� ****** */


	/* ****** ��Linux����ϵͳ�ģ������ڴ˽����ź��� ****** */
	
	if(frame_finish == 1){

		// �����ж� 
		switch(framedata[0]){
		//������
		case CHIRD_UART_SLIDE1:
			break;
		case CHIRD_UART_SLIDE2:
			break;
		case CHIRD_UART_SLIDE3:
			break;

		//����
		case CHIRD_UART_KEY1  :
			break;
		case CHIRD_UART_KEY2  :
			break;
		case CHIRD_UART_KEY3  :
			break;
		case CHIRD_UART_KEY4  :
			break;
		case CHIRD_UART_KEY5  :
			break;
		case CHIRD_UART_KEY6  :
			break;
		case CHIRD_UART_KEY7  :
			break;
		case CHIRD_UART_KEY8  :
			break;
		case CHIRD_UART_KEY9  :
			break;
		case CHIRD_UART_KEY10 :
			break;
		case CHIRD_UART_KEY11 :
			break;
		case CHIRD_UART_KEY12 :
			break;

		// LED�ơ�״̬
		case CHIRD_UART_LED1  : 
			break;
		case CHIRD_UART_LED2  : 
			break;
		case CHIRD_UART_LED3  : 
			break;
		case CHIRD_UART_LED4  : 
			break;
		case CHIRD_UART_LED5  : 
			break;
		case CHIRD_UART_LED6  : 
			break;
		case CHIRD_UART_LED7  : 
			break;
		case CHIRD_UART_LED8  : 
			break;

		// ����
		case CHIRD_UART_PARAM1: 
			break;
		case CHIRD_UART_PARAM2: 
			break;
		case CHIRD_UART_PARAM3: 
			break;
		case CHIRD_UART_PARAM4: 
			break;
		case CHIRD_UART_PARAM5: 
			break;
		case CHIRD_UART_PARAM6: 
			break;
		}
	}
	
	return 0;
}

void chird_uart_send(unsigned char * framedata, int len)
{
	/* ****** ��������д������ ****** */


	/* ****** ��������д������ ****** */
	
}

#if 0
int main()
{
	int i;
	unsigned char v = 0x5+CHIRD_UART_SLIDE1+255;
	unsigned char a[] = {FIRST_CHAR, SECOND_CHAR, 5, CHIRD_UART_SLIDE1, 255, v, LAST_CHAR};
	
	for(i = 0 ; i < 7 ; i++){
		chird_uart_handler(a[i]);
	}

	chird_uart_Process();

	chird_uart_send_key(1);
	
}
#endif
