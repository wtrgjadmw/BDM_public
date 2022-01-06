// 蓋を開け閉めした時の照度の変化を確認するプログラム

#include <Arduino.h>

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------//
int LED = 27;  // LEDのピン設定

const int CDS5000 = 12;     // CdSセル接続ピン

void setup() {
  pinMode(CDS5000, INPUT);
  
  Serial.begin(9600);
  delay(1000);
}

void loop()
{
  for (int j = 0; j < 10; j++) {
    int brightSum = 0;
    for (int i = 0; i < 10; i++) {
      brightSum += analogRead(CDS5000);
      delay(100);
    }
    float brightAve = brightSum / 10;
    Serial.println(brightAve);
    delay(2000);
  }
  Serial.println("----------------");
  delay(5000);
}
