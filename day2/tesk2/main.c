#ifndef F_CPU
#define F_CPU 16000000UL
#endif //반복문 끝

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define I2C_LCD_ADDR 0x27

#define LCD_RS 0x01
#define LCD_E  0x04
#define LCD_BACKLIGHT 0x08

void I2C_init(void) 
{
	TWBR = 72; // 100kHz SCL
	TWSR = 0x00;
}

void I2C_send_byte(unsigned char data) 
{
	// i2c 통신 시작
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	// 받을 대상(LCD)의 I2C 주소 전송
	TWDR = (I2C_LCD_ADDR << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	// 실제 보낼 데이터(1바이트) 전송
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	// I2C 통신 종료 알림
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_LCD_write_nibble(unsigned char nibble, unsigned char mode) 
{
  // 데이터와 제어 신호 합치기
	unsigned char data = (nibble & 0xF0) | mode | LCD_BACKLIGHT;
  //핀 HIGH 신호 주기
	I2C_send_byte(data | LCD_E);
	_delay_us(1);
  //핀 LOW 신호 주기
	I2C_send_byte(data & ~LCD_E);
	_delay_us(50);
}

void I2C_LCD_command(unsigned char cmnd) 
{
	I2C_LCD_write_nibble(cmnd & 0xF0, 0);
	I2C_LCD_write_nibble((cmnd << 4) & 0xF0, 0);
}

void I2C_LCD_data(unsigned char data) 
{
	I2C_LCD_write_nibble(data & 0xF0, LCD_RS);
	I2C_LCD_write_nibble((data << 4) & 0xF0, LCD_RS);
}

void I2C_LCD_init(void) 
{
	I2C_init();
	_delay_ms(100);
	
	// HD44780 정석 4비트 진입 초기화 시퀀스
	I2C_LCD_write_nibble(0x30, 0);
	_delay_ms(5);
	I2C_LCD_write_nibble(0x30, 0);
	_delay_us(200);
	I2C_LCD_write_nibble(0x30, 0);
	_delay_us(200);
	I2C_LCD_write_nibble(0x20, 0); // 4비트 모드 확정
	_delay_ms(5);

	I2C_LCD_command(0x28); // 2행, 5x7 폰트
	I2C_LCD_command(0x0C); // 화면 ON
	I2C_LCD_command(0x01); // Clear
	_delay_ms(5);
}

void I2C_LCD_string(unsigned char col, unsigned char row, char *str) 
{
	unsigned char addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
	I2C_LCD_command(addr);
	while (*str) I2C_LCD_data(*str++);
}

void adcInit(void) 
{
	DDRF &= ~(1 << PF0);
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

unsigned int adcRead(void) 
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)); //변환이 끝날 때까지 대기
	return ADC; //변환된 10비트 결과값(0~1023) 반환
}

int main(void) 
{
	unsigned int adc_val = 0; // ADC 읽은 값(0~1023)을 담을 변수
	float voltage = 0.0; // 계산된 전압 값(0.0V~5.0V)을 담을 변수
	char buff[17] = {0, };
	unsigned char led_pos = 0; // 불을 켤 LED 위치 번호(0~7)를 담을 변수

	DDRA = 0xFF;
	PORTA = 0xFF;

	adcInit();
	I2C_LCD_init();

	I2C_LCD_string(0, 0, "21th_KSW");

	while (1) 
	{
		// ADC 값 읽기
		adc_val = adcRead();
		// 전압 계산
		voltage = (float)adc_val * 5.0 / 1023.0;

		// 가변저항 위치에 따라 led 켜기
		led_pos = adc_val / 128;
		if (led_pos > 7) led_pos = 7;
		PORTA = ~(1 << led_pos);

		// LCD 출력을 위한 소수점 분리
		int v_int = (int)voltage;
		int v_dec = (int)((voltage - v_int) * 1000);

		// LCD 두 번째 줄에 글자 구성 및 출력
		sprintf(buff, "%4d    %d.%03dV", adc_val, v_int, v_dec);
		I2C_LCD_string(0, 1, buff);

		_delay_ms(100);
	}
	return 0;
}
