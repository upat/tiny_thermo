# tiny_thermo
概要
---
ATtiny85(Digispark)を使用し、乾電池駆動を目指した温度計

機能
---
- 約1分毎に温度を計測し、LCDに表示
- リセット確認用に駆動カウンタを表示(日/分)

開発・動作環境
---
- 使用ボード
    - Digispark
- 画面表示モジュール
    - 秋月電子通商 I2C接続小型LCDモジュール(8×2行) ([https://akizukidenshi.com/catalog/g/g109109/](https://akizukidenshi.com/catalog/g/g109109/))
- 温度センサー
    - DS18B20 ([https://electronicwork.shop/items/690b665668911951d994bac1](https://electronicwork.shop/items/690b665668911951d994bac1))
- Arduino IDE 2.3.6(使用ライブラリ)
    - ATTinyCore 1.5.2 ([https://github.com/SpenceKonde/ATTinyCore](https://github.com/SpenceKonde/ATTinyCore))
    - DS18B20 1.0.0 ([https://github.com/matmunk/DS18B20](https://github.com/matmunk/DS18B20))
    - arduino_ST7032 ([https://github.com/tomozh/arduino_ST7032/](https://github.com/tomozh/arduino_ST7032/))
- 電源
    - 単3電池ボックス(カバー付き/スイッチ付き) (1本) ([https://electronicwork.shop/items/65a4a29156efca00313c2849](https://electronicwork.shop/items/65a4a29156efca00313c2849))

使い方
---
- 配線例

<img src="https://github.com/upat/tiny_thermo/blob/master/images/pic1.png" width="60%" height="60%">

- 改造など
    - 乾電池運用のため電源LED、PB1 LED、5V降圧レギュレーターを除去しています。
    - 上記改造を行った後は乾電池電源を接続したままUSB接続するとおそらく壊れます。どちらか片方からの電源供給を推奨。
    - PB1 LEDを除去せず使用する場合はPB1でDS18B20の通信が行えないため、PB3またはPB4にDS18B20を接続してください。
    - 上記によりPB3またはPB4を使用する場合は、USB経由で書き込み時にDS18B20を外さないとスケッチが書き込めなくなります。

ファイル構成
---
- LICENCE
    - ライセンスファイル
- README.md
    - このファイル
- tiny_thermo.ino
    - メイン処理

参考文献
---
- ATtiny85データシート ([https://ww1.microchip.com/downloads/en/devicedoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf](https://ww1.microchip.com/downloads/en/devicedoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf))
- DS18B20データシート ([https://www.analog.com/media/en/technical-documentation/data-sheets/DS18B20.pdf](https://www.analog.com/media/en/technical-documentation/data-sheets/DS18B20.pdf))
- AQM0802データシート(主にp.62) ([https://akizukidenshi.com/goodsaffix/st7032.pdf](https://akizukidenshi.com/goodsaffix/st7032.pdf))

ライセンス
---
MIT Licence
