#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// 스위치 핀 설정 (PD2: INT2, PD3: INT3, PE4: INT4, PE5: INT5)
#define INT_SW1_PIN PD2
#define INT_SW2_PIN PD3
#define INT_SW3_PIN PE4
#define INT_SW4_PIN PE5

volatile unsigned char event = 0; // 0: 평소(2진 카운터), 1: SW1, 2: SW2, 3: SW3

void interruptInit(void)
{
	// 1. 스위치 핀 입력 설정 및 내부 풀업 저항 활성화
	DDRD &= ~((1 << INT_SW1_PIN) | (1 << INT_SW2_PIN));
	PORTD |= (1 << INT_SW1_PIN) | (1 << INT_SW2_PIN);

	DDRE &= ~((1 << INT_SW3_PIN) | (1 << INT_SW4_PIN));
	PORTE |= (1 << INT_SW3_PIN) | (1 << INT_SW4_PIN);

	// 2. 인터럽트 발생 조건 설정 (Falling Edge)
	EICRA = (1 << ISC21) | (1 << ISC31); // INT2, INT3
	EICRB = (1 << ISC41) | (1 << ISC51); // INT4, INT5

	// 3. 외부 인터럽트 허용
	EIMSK |= (1 << INT2) | (1 << INT3) | (1 << INT4) | (1 << INT5);
}

// INT2 (PD2) : 3개씩 우측 이동
ISR(INT2_vect)
{
	event = 1;
}

// INT3 (PD3) : 3개씩 좌측 이동
ISR(INT3_vect)
{
	event = 2;
}

// INT4 (PE4) : 1개 좌우 이동
ISR(INT4_vect)
{
	event = 3;
}

// INT5 (PE5) : 2진 카운터 초기화 및 꺼짐
ISR(INT5_vect)
{
	event = 0;
	PORTA = 0xFF;
}

int main(void)
{
	int i = 0, repeat = 0;
	unsigned char temp = 0;

	// 1. PORTA 전체 LED 출력 설정 (Active Low)
	DDRA = 0xFF;
	PORTA = 0xFF; // 초기 상태: 모두 꺼짐

	// 2. 인터럽트 초기화 및 전체 인터럽트 허용
	interruptInit();
	sei();

	while (1)
	{
		// 1) INT2 (PD2): LED 3개씩 '우측'으로 2칸씩 이동 (2회 반복)
		if (event == 1)
		{
			for (repeat = 0; repeat < 2; repeat++)
			{
				temp = 0b11100000; // 7, 6, 5번 LED 켜짐
				for (i = 0; i < 4; i++)
				{
					PORTA = ~temp;
					temp = temp >> 1; // 우측 Shift
					_delay_ms(200);
				}
			}
			PORTA = 0xFF;
			event = 0;
		}
		// 2) INT3 (PD3): LED 3개씩 '좌측'으로 2칸씩 이동 (2회 반복)
		else if (event == 2)
		{
			for (repeat = 0; repeat < 2; repeat++)
			{
				temp = 0b00000111; // 2, 1, 0번 LED 켜짐
				for (i = 0; i < 4; i++)
				{
					PORTA = ~temp;
					temp = temp << 1; // 좌측 Shift
					_delay_ms(200);
				}
			}
			PORTA = 0xFF;
			event = 0;
		}
		// 3) INT4 (PE4): 1개 LED 좌측 이동 후 우측 이동
		else if (event == 3)
		{
			temp = 0b00000001;
			for (i = 0; i < 8; i++)
			{
				PORTA = ~temp;
				temp = temp << 1;
				_delay_ms(100);
			}

			temp = 0b10000000;
			for (i = 0; i < 8; i++)
			{
				PORTA = ~temp;
				temp = temp >> 1;
				_delay_ms(100);
			}
			PORTA = 0xFF;
			event = 0;
		}
		// 4) 평소: 2진 카운터 (0.1초 마다)
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

	return 0;
}
