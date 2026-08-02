#ifndef F_CPU
#define F_CPU 16000000UL
#endif //반복문을 끝내는 코드

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SW1_PIN     PD2  // SW1 (4~7 LED)
#define SW2_PIN     PD3  // SW2 (0~3 LED)

#define INT_SW3_PIN PE4  // INT3 -> INT4
#define INT_SW4_PIN PE5  // INT4 -> INT5

volatile unsigned char move_flag = 0; // 0: 없음, 1: 좌측이동, 2: 우측이동

void interruptInit(void)
{
	//스위치 핀 입력 설정 및 내부 풀업 저항 켜기
	DDRE &= ~((1 << INT_SW3_PIN) | (1 << INT_SW4_PIN));
	PORTE |= (1 << INT_SW3_PIN) | (1 << INT_SW4_PIN);
	//인터럽트 발생 조건 설정
	EICRB &= ~((1 << ISC40) | (1 << ISC50));
	EICRB |= (1 << ISC41) | (1 << ISC51);
	//INT4, INT5 외부 인터럽트
	EIMSK |= (1 << INT4) | (1 << INT5);
}

ISR(INT4_vect)
{
	move_flag = 1; // INT3 발생 시 좌측 이동
}

ISR(INT5_vect)
{
	move_flag = 2; // INT4 발생 시 우측 이동
}

int main(void)
{
	int i = 0;

	// 1. PORTA 전체 LED 출력 설정
	DDRA = 0xFF;
	PORTA = 0xFF; // Active Low (모두 꺼짐)

	// 2. PORTD 스위치 핀 설정
	DDRD &= ~((1 << SW1_PIN) | (1 << SW2_PIN));
	PORTD |= (1 << SW1_PIN) | (1 << SW2_PIN);

	// 3. 인터럽트 초기화
	interruptInit();
	sei();

	while (1)
	{
		// 1) 인터럽트 발생 시 LED 이동 (인터럽트 1)
		if (move_flag == 1) // INT3 (좌측 이동: 0 -> 7)
		{
			for (i = 0; i < 8; i++)
			{
				PORTA = ~(1 << i);
				_delay_ms(100);
			}
			PORTA = 0xFF;
			move_flag = 0;
		}
		else if (move_flag == 2) // INT4 (우측 이동: 7 -> 0)
		{
			for (i = 7; i >= 0; i--)
			{
				PORTA = ~(1 << i);
				_delay_ms(100);
			}
			PORTA = 0xFF;
			move_flag = 0;
		}
		// 2) 일반 스위치(SW1, SW2) 입력 처리 (인터럽트 2)
		else if (!(PIND & (1 << SW1_PIN)) && !(PIND & (1 << SW2_PIN)))
		{
			// SW1, SW2 둘 다 눌리면 모두 켜기
			PORTA = 0x00;
		}
		else if (!(PIND & (1 << SW1_PIN)))
		{
			// SW1 눌리면 4~7번 LED 켜기
			PORTA = 0x0F;
		}
		else if (!(PIND & (1 << SW2_PIN)))
		{
			// SW2 눌리면 0~3번 LED 켜기
			PORTA = 0xF0;
		}
		// 3) 평소: 0.5s 마다 모든 LED 깜빡이기
		else
		{
			PORTA = 0x00; // All ON
			_delay_ms(500);

			PORTA = 0xFF; // All OFF
			_delay_ms(500);
		}
	}

	return 0;
}
