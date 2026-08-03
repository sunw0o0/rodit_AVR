#include "LCD_Text.h"
// 스위치 4개의 현재 눌림 상태를 4bit 값으로 리턴
// bit0=SW1(PE4), bit1=SW2(PE5), bit2=SW3(PD2), bit3=SW4(PD3)
// 눌리지 않음(풀업) = 1, 눌림 = 0
U8 readButtons(void)
{
	U8 state = 0;

	if (PINE & (1 << 4)) state |= 0x01;   // SW1 : PE4
	if (PINE & (1 << 5)) state |= 0x02;   // SW2 : PE5
	if (PIND & (1 << 2)) state |= 0x04;   // SW3 : PD2
	if (PIND & (1 << 3)) state |= 0x08;   // SW4 : PD3

	return state;
}

int main(void)
{
	int A = 1, B = 1;
	char ops[4] = { '+', '-', '*', '/' };
	U8   opIndex = 0;

	U8   prevBtn = 0x0F;   // 풀업 상태: 1 = 안눌림
	char line1[17];
	char line2[17];

	// 스위치 입력 설정 + 내부 풀업 사용
	DDRE  &= ~((1 << 4) | (1 << 5));   // PE4, PE5 입력
	PORTE |=  (1 << 4) | (1 << 5);     // 내부 풀업

	DDRD  &= ~((1 << 2) | (1 << 3));   // PD2, PD3 입력
	PORTD |=  (1 << 2) | (1 << 3);     // 내부 풀업

	lcdInit();

	// 초기 상태 디버그 표시 (2번째 줄)
	sprintf(line2, "A%d %c B%d", A, ops[opIndex], B);
	lcdString(1, 0, line2);

	while (1)
	{
		U8 cur = readButtons() & 0x0F;
		U8 pressedEdge = (~cur) & prevBtn & 0x0F;   // 새로 눌린 비트 검출

		if (pressedEdge)
		{
			_delay_ms(20);                    // 디바운스
			cur = readButtons() & 0x0F;        // 디바운스 후 재확인

			if (pressedEdge & 0x01)             // SW1 : A 증가
			{
				A++;
			}
			else if (pressedEdge & 0x02)        // SW2 : 연산자 변경
			{
				opIndex = (opIndex + 1) % 4;
			}
			else if (pressedEdge & 0x04)        // SW3 : B 증가
			{
				B++;
			}
			else if (pressedEdge & 0x08)        // SW4 : 연산 실행 및 출력
			{
				char op = ops[opIndex];
				lcdClear();

				if (op == '/')
				{
					if (B != 0)
					{
						float result = (float)A / (float)B;
						sprintf(line1, "%d %c %d = %.2f", A, op, B, result);
					}
					else
					{
						sprintf(line1, "%d %c %d = ERR", A, op, B);
					}
				}
				else
				{
					int result = 0;

					if (op == '+')      result = A + B;
					else if (op == '-') result = A - B;
					else if (op == '*') result = A * B;

					sprintf(line1, "%d %c %d = %d", A, op, B, result);
				}

				lcdString(0, 0, line1);   // 1번째 줄에 전체 식 출력
			}

			// SW1~3 눌렀을 때도 2번째 줄에 현재 상태를 계속 보여줌 (디버그/확인용)
			sprintf(line2, "A%d %c B%d   ", A, ops[opIndex], B);
			lcdString(1, 0, line2);
		}

		prevBtn = cur;
		_delay_ms(10);
	}
}
