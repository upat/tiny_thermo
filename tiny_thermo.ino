#include <ST7032.h>
#include <DS18B20.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

DS18B20 ds(1);
ST7032 lcd;

volatile uint8_t wakeCounter = 0;

void setup() {
  /* ウォッチドッグタイマ割り込み設定 */
  noInterrupts(); /* 割り込み禁止 */
  /* WDTリセットフラグのクリア */
  /* Bit 3 – WDRF: Watchdog Reset Flag */
  MCUSR &= ~(1 << WDRF);
  /* 4クロック間レジスタ設定変更許可、ウォッチドッグタイマ有効化 */
  /* Bit 4 – WDCE: Watchdog Change Enable */
  /* Bit 3 – WDE: Watchdog Enable */
  WDTCR |= (1 << WDCE) | (1 << WDE);
  /* ウォッチドッグタイムアウト割り込みフラグクリア、ウォッチドッグタイムアウト割り込み有効化、タイムアウト期間最大b1001=8s */
  /* Bit 7 – WDIF: Watchdog Timeout Interrupt Flag */
  /* Bit 6 – WDIE: Watchdog Timeout Interrupt Enable */
  /* Bits 5, 2:0 – WDP[3:0]: Watchdog Timer Prescaler 3, 2, 1, and 0 */
  WDTCR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0) | (1 << WDIF);
  interrupts();   /* 割り込み許可 */

  /* PB2～PB5 A/D変換機能の無効化 */
  /* Bit 7 – ADEN: ADC Enable */
  ADCSRA &= ~(1 << ADEN);

  /* 未使用ピン設定 */
  pinMode(3, INPUT_PULLUP); /* デバッグ環境下ではここに接続したモジュールを外すこと */
  pinMode(4, INPUT_PULLUP);

  /* DS18B20 分解能設定(9bit ±0.5℃) */
  ds.setResolution(9);

  /* LCD設定 */
  lcd.begin(8, 2);
  lcd.setContrast(30); /* 環境によって変更(3.3V=30、5V=10) */
}

void loop() {
  char dispStr[9] = {};
  float tempVal = 0.0f;
  static uint16_t minCounter = 0;
  static uint16_t dayCounter = 0;

  /* センサー値読み出し+表示 */
  tempVal = ds.getTempC();
  if (0.0 <= tempVal && 40.0 >= tempVal) {
    lcd.setCursor(0, 0);
    /* float→char変換(スペース埋め右詰め) */
    convertFtoC(dispStr, tempVal);
    /* 温度表示 */
    lcd.print(dispStr);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("   Error");
  }
  
  /* 0埋め3桁稼働日数/0埋め4桁稼働時間(分) */
  lcd.setCursor(0, 1);
  convertUtoC(dispStr, dayCounter, minCounter);
  lcd.print(dispStr);

  if (1440 > minCounter) {
    minCounter++;   /* 実行回数カウンタ(分)加算 */
  } else {
    minCounter = 0;
    if (999 > dayCounter) {
      dayCounter++; /* 実行回数カウンタ(日)加算 */
    } else {
      dayCounter = 0;
    }
  }

  wakeCounter = 0; /* カウンタクリア */
  /* 約56s待ち */
  while (7 > wakeCounter) {
    goToSleep();
  }
}

/* ウォッチドッグ割り込み */
ISR(WDT_vect) {
  /* カウンタ加算 */
  wakeCounter++;
}

/*******************************************************************/
/* 処理内容：スリープ実行関数                                      */
/* 引数　　：なし                                                  */
/* 戻り値　：なし                                                  */
/* 備考　　：ATTiny85スリープ関数のコール                          */
/*******************************************************************/
void goToSleep(void) {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); /* スリープモード設定 */
  sleep_enable();  /* スリープ有効化 */
  interrupts();    /* 割り込み許可(割り込み禁止誤作動対策) */
  sleep_cpu();     /* スリープ実行 */
  sleep_disable(); /* スリープ無効化 */
}

/*******************************************************************/
/* 処理内容：指定した形式のfloat->char変換                         */
/* 引数　　：9Byteの配列ポインタ、0.0～40.0(室温想定)までの値      */
/* 戻り値　：なし                                                  */
/* 備考　　：ST7032 8x2行キャラクターLCD用                         */
/*******************************************************************/
void convertFtoC(char* strBuf, float tempVal) {
  uint16_t intVal = 0;

  /* 小数1桁に丸め(* 10 + 0.5(丸め)して整数化) */
  intVal = (uint16_t)(tempVal * 10.0f + 0.5f);
  /* 終端文字 */
  strBuf[8] = '\0';
  /* 温度記号 */
  strBuf[7] = 'C';
  strBuf[6] = 0xF2; /* ℃の°の部分 */
  /* 小数部から順に格納 */
  /* 小数部 */
  strBuf[5] = '0' + intVal % 10;   /* asciiの0に足してchar変換 */
  intVal /= 10;
  /* 小数点 */
  strBuf[4] = '.';
  /* 整数部(1の位) */
  strBuf[3] = '0' + intVal % 10;   /* asciiの0に足してchar変換 */
  intVal /= 10;
  /* 整数部(10の位) */
  if (1 > intVal) {
    strBuf[2] = ' ';
  } else {
    strBuf[2] = '0' + intVal % 10; /* asciiの0に足してchar変換 */
  }
  /* 空白(未使用箇所) */
  strBuf[1] = strBuf[0] = ' ';
}

/*******************************************************************/
/* 処理内容：指定した形式のuint16_t->char変換                      */
/* 引数　　：9Byteの配列ポインタ、3桁の値、4桁の値                 */
/* 戻り値　：なし                                                  */
/* 備考　　：ST7032 8x2行キャラクターLCD用                         */
/*******************************************************************/
void convertUtoC(char* strBuf, uint16_t dayVal, uint16_t minVal) {
  int i = 0;

  /* 終端文字 */
  strBuf[8] = '\0';
  /* 0埋め4桁稼働時間(分) */
  for (i = 7; i > 3; i--) {
    strBuf[i] = '0' + (minVal % 10); /* asciiの0に足してchar変換(0の場合は0が入り続ける) */
    minVal /= 10;
  }
  /* 区切り記号 */
  strBuf[3] = '/';
  /* 0埋め3桁稼働日数 */
  for (i = 2; i >= 0; i--) {
    strBuf[i] = '0' + (dayVal % 10); /* asciiの0に足してchar変換(0の場合は0が入り続ける) */
    dayVal /= 10;
  }
}