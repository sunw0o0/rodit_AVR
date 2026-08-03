Register (Datasheet 132 ~ )

<img width="1152" height="386" alt="image" src="https://github.com/user-attachments/assets/22ac1fca-056d-4f6c-9720-bda63af7f63c" />

+ COMnA1:0, COMnB1:0, COMnC1:0 비트들은 각각 대응하는 출력 비교 핀인 OCnA, OCnB, OCnC의 동작 방식을 제어

    + COM1A1:0 -> OC1A (PB5) 핀 제어

    + COM1B1:0 -> OC1B (PB6) 핀 제어

    + COM1C1:0 -> OC1C (PB7) 핀 제어

+ COMnA1:0 비트 중 하나라도 1로 설정하면(즉, 00이 아닌 01, 10, 11 중 하나가 되면), 해당 I/O 핀의 일반 입출력 기능(GPIO) 대신 OCnA 출력 기능이 우선권을 갖고 가로채서 적용
    + COM 비트를 설정하는 순간, 해당 핀(PB5 등)은 일반 포트 핀이 아니라 타이머 전용 PWM/펄스 출력 핀으로 전환된다는 의미

 <img width="1018" height="278" alt="image" src="https://github.com/user-attachments/assets/24336f6f-9370-45f7-ad33-d0ce9aecd545" />

 + Bit 7 (ICNnC)
     + 기능: 이 비트를 1로 설정하면 노이즈 필터링 회로가 동작
     + 원리: 입력 캡처 핀(ICPn)으로 들어오는 신호를 연속으로 4번의 클록 동안 검사하여, 신호가 안정적으로 같을 때만 정상 신호로 인정
     + 특징: 필터를 거치기 때문에 캡처 동작이 시스템 클록 기준 약간 지연
  
 + Bit 6 (ICESn)
     + 0으로 설정: 하강 엣지(5V -> 0V)일 때 감지
     + 1로 설정: 상승 엣지(0V -> 5V)일 때 감지
     + 엣지가 감지되면 현재 타이머 카운터값(TCNT)이 ICRn 레지스터로 복사되고, 인터럽트 플래그(ICFn)가 켜진다. (만약 ICRn 레지스터를 Fast PWM 모드의 TOP 값(주기 설정)으로 사용할 때는, 입력 캡처 기능이 자동으로 비활성화)
  
  + Bit 4:3 (WGMn3:2)
    + 기능: TCCR1A에 있는 WGMn1:0 비트와 결합하여 총 4개의 비트(WGMn3:0)로 동작 모드를 결정
    + 역할: Normal, CTC, Fast PWM, Phase Correct PWM 모드 등을 선택하는 비트
   
  + Bit 2:0 – CSn2:0
    + 기능: 타이머/카운터가 사용할 클록 원천 및 분주비를 결정하는 3개의 비트

   <img width="1090" height="1238" alt="image" src="https://github.com/user-attachments/assets/61a8c37e-ac35-49b7-b7d6-00eb8419efae" />

  + 외부 핀을 타이머 클록으로 쓸 경우, 해당 핀을 출력(Output)으로 설정하더라도 핀 상태 변화를 감지해서 카운트할 수 있음
  + TCCR1C / TCCR3C (Bit 7, 6, 5: FOCnx)
      + 사용 조건: non-PWM 모드 (Normal 모드 또는 CTC 모드)에서만 동작하며, PWM 모드에서는 작동하지 않고 무시
      + 원리: 원래는 카운터(TCNT)가 증가하다가 OCR 설정값과 똑같아져야 핀 출력 상태가 바뀝니다. 하지만 이 비트(FOC1A 등)에 1을 써서 넣어주는 순간, TCNT 숫자와 상관없이 즉시 '비교 일치' 상황이 일어난 것으로 강제 조작합니다. 이에 따라 COMnx1:0에 설정된 대로 핀 전압(TOGGLE, HIGH, LOW)이 즉시 바뀝니다.
   
  <img width="950" height="1192" alt="image" src="https://github.com/user-attachments/assets/99ae114d-dfbe-4aa4-9950-676c8fd06743" />

  + TCNTnH / TCNTnL
      + 16비트 접근 방식: 8비트 데이터 버스를 사용하는 MCU 특성상, 16비트를 한 번에 다루기 위해 상위 8비트(H)와 하위 8비트(L)로 나누어져 있
  + OCR1AH/L, OCR1BH/L, OCR1CH/L
      + 카운터(TCNT)가 0부터 올라가다가 OCR에 설정된 값과 똑같아지는 순간 핀 전압이 LOW/HIGH로 바뀌게 된다.
