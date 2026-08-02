#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t event = 0;

int main(void)
{
	DDRA = 0xFF;  // PORTA 전체 출력 설정
	DDRD = 0x00;  // PORTD 전체 입력 설정 (PD2, PD3 스위치용)
	DDRE = 0x00;  // PORTE 전체 입력 설정 (PE4, PE5 스위치용)
	PORTA = 0xFF; // Active Low (초기 상태: LED 모두 꺼짐)
	PORTD = 0xFF; // PD2, PD3 내부 풀업 저항 활성화
	PORTE = 0xFF; // PE4, PE5 내부 풀업 저항 활성화
	
	// 실제 스위치가 연결된 핀 : PD2(INT2), PD3(INT3), PE4(INT4), PE5(INT5)
	EIMSK = (1 << INT2) | (1 << INT3) | (1 << INT4) | (1 << INT5); // INT2~INT5 허용
	
	EICRA = (1 << ISC21) | (1 << ISC31);              // INT2, INT3 : Falling Edge
	EICRB = (1 << ISC41) | (1 << ISC51);               // INT4, INT5 : Falling Edge
	
	sei(); // 전체 인터럽트 허용
	
	while (1)
	{
		// 2번: INT2(PD2) - LED 3개씩 '우측'으로 2칸씩 이동
		if (event == 1) {
			for (int repeat = 0; repeat < 2; repeat++) 
			{ // 2회 반복
				uint8_t temp = 0b11100000; // 7, 6, 5번 LED 켜짐
				for (int i = 0; i < 4; i++) 
				{
					PORTA = ~temp;
					temp = temp >> 1; // 우측으로 1칸
					_delay_ms(200);
				}
			}
			event = 0;
		}
		// 3번: INT3(PD3) - LED 3개씩 '좌측'으로 2칸씩 이동
		else if (event == 2) 
		{
			for (int repeat = 0; repeat < 2; repeat++) 
			{ // 2회 반복
				uint8_t temp = 0b00000111; // 2, 1, 0번 LED 켜짐
				for (int i = 0; i < 4; i++) 
				{
					PORTA = ~temp;
					temp = temp << 1; // 좌측으로 1칸(1비트) 이동
					_delay_ms(200);
				}
			}
			event = 0;
		}
		// 4번: INT4(PE4) - 1개 LED 좌측 이동 후 우측 이동 (1칸씩)
		else if (event == 3) 
		{
			uint8_t temp = 0b00000001;
			for (int i = 0; i < 8; i++) 
			{
				PORTA = ~temp;
				temp = temp << 1;
				_delay_ms(100);
			}
			temp = 0b10000000;
			for (int i = 0; i < 8; i++) 
			{
				PORTA = ~temp;
				temp = temp >> 1;
				_delay_ms(100);
			}
			event = 0;
		}
		// 평소: LED 2진 카운터 (0.1초 마다)
		else 
		{
			if (PORTA == 0x00) 
			{
				PORTA = 0xFF;
				_delay_ms(100);
			}
			else 
			{
				PORTA = PORTA - 1;
				_delay_ms(100);
			}
		}
	}
}
ISR(INT2_vect) { event = 1; }  // PD2 : 3개씩 우측 이동
ISR(INT3_vect) { event = 2; }  // PD3 : 3개씩 좌측 이동
ISR(INT4_vect) { event = 3; }  // PE4 : 1개 좌우 이동

ISR(INT5_vect)                 // PE5 : 2진 카운터 초기화
{
	event = 0;
	PORTA = 0xFF;
}
