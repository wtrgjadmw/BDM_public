#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Arduino.h>

void AE_HX711_Init(void);
void AE_HX711_Reset(void);
long AE_HX711_Read(void);
long AE_HX711_Averaging(long adc, char num);
float AE_HX711_getGram(char num);

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------//
#define pin_dout  8
#define pin_slk   9

//---------------------------------------------------//
// ロードセル　Ｓ字型　ＳＣ３０１Ａ　１００ｋＧ [P-12036]
//---------------------------------------------------//
//#define OUT_VOL   0.002f      //定格出力 [V]
//#define LOAD      100000.0f   //定格容量 [g]

//---------------------------------------------------//
// ロードセル　シングルポイント（ ビーム型）　ＳＣ６０１　１２０ｋＧ [P-12035]
//---------------------------------------------------//
//#define OUT_VOL   0.001f      //定格出力 [V]
//#define LOAD      120000.0f   //定格容量 [g]

//---------------------------------------------------//
// ロードセル　シングルポイント（ ビーム型）　ＳＣ１３３　２０ｋＧ [P-12034]
//---------------------------------------------------//
//#define OUT_VOL   0.002f      //定格出力 [V]
//#define LOAD      20000.0f    //定格容量 [g]

//---------------------------------------------------//
// ロードセル　シングルポイント（ビーム型）　ＳＣ６１６Ｃ　５００ｇ[P-12532]
//---------------------------------------------------//
//#define OUT_VOL   0.0007f   //定格出力 [V]
//#define LOAD      500.0f    //定格容量 [g]

#define OUT_VOL   0.0008f   //定格出力 [V]
#define LOAD      2000.0f    //定格容量 [g]

float offset;

int count = 0;

double gram[5]; //gram記憶用

int total;
int coin;
int rep  = 3;

int display_count = 0;

double sabun[2];

double gram_ave[2];//重量が落ち着ているときの値を記憶しておく

double coin_sabun;

double gosa = 0.3;

int total_amount = 0;

boolean situation = true;//tureが定常、falseが非定常

boolean coin_in = false;//tureがコインを入れられたと判別、falseはコイン入れられていない

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);
  Serial.println("AE_HX711 test");
  AE_HX711_Init();
  AE_HX711_Reset();
  offset = AE_HX711_getGram(30);

  //LCD用のsetup
  lcd.init();                      // initialize the lcd

  // Print a message to the LCD.
  lcd.backlight();
  //lcd.setCursor(0, 0);
  //lcd.print("Savings amount");
  //lcd.setCursor(0, 1);
  //lcd.print("Arduino!");
}

void loop()
{
  float data;
  char S1[20];
  char s[20];

  double gram0;

  data = AE_HX711_getGram(5);

  for (int i = 0; i  < rep - 1; i++) {
    gram[rep - i - 1] = gram[rep - i - 2];
  }
  sabun[1] = sabun[0];

  gram0 = (data - offset) * 4.5 /4.75;
  gram[0] = gram0;
  //sprintf(S1, "%s [g] (0x%4x)", dtostrf((data - offset), 5, 3, s), AE_HX711_Read());
  Serial.println(gram0);
  sabun[0] = gram[0] - gram[1];

  if (sabun[1] < 0.7 && sabun[0] < 0.7) {
    if(situation == false){
      gram_ave[1] = gram_ave[0];
      coin_in = true;
    }
    situation = true;
    gram_ave[0] = (gram[0] + gram[1] + gram[2]) / 3.0;
  } else {
    situation = false;
    //Serial.println("test");
  }
  if (coin_in) {
    coin_sabun = gram_ave[0] - gram_ave[1];
    coin = coin_hantei(coin_sabun);
    coin_in = false;
    //Serial.println("coin in");
    //Serial.println(coin);
    total_amount = total_amount + coin;
    lcd.setCursor(0, 1);
    lcd.print("insert coin");
    lcd.setCursor(12, 1);
    lcd.print(coin);

    display_count = 0;
  }
  
  lcd.setCursor(0, 0);
  lcd.print("Total");
  lcd.setCursor(6, 0);
  lcd.print(total_amount);
  display_count++;
  if(display_count == 6){
    lcd.setCursor(0, 1);
    lcd.print("                      ");
  }
  //delay(500);
  count++;
}

int coin_hantei(double g){
  if(coin_hantei2(1.0,g) == true){
    return  1;
  }
  if(coin_hantei2(3.75,g) == true){
    return  5;
  }
  if(coin_hantei2(4.5,g) == true){
    return  10;
  }
    if(coin_hantei2(4.0,g) == true){
    return  50;
  }
  if(coin_hantei2(4.8,g) == true){
    return  100;
  }
  if(coin_hantei2(7.0,g) == true){
    return  500;
  }
  return 0;
  
}

boolean coin_hantei2(double g , double h){
  if(g - gosa < h && g + gosa > h){
    return true;
  }
  return false;
}


void AE_HX711_Init(void)
{
  pinMode(pin_slk, OUTPUT);
  pinMode(pin_dout, INPUT);
}

void AE_HX711_Reset(void)
{
  digitalWrite(pin_slk, 1);
  delayMicroseconds(100);
  digitalWrite(pin_slk, 0);
  delayMicroseconds(100);
}

long AE_HX711_Read(void)
{
  long data = 0;
  while (digitalRead(pin_dout) != 0);
  delayMicroseconds(10);
  for (int i = 0; i < 24; i++)
  {
    digitalWrite(pin_slk, 1);
    delayMicroseconds(5);
    digitalWrite(pin_slk, 0);
    delayMicroseconds(5);
    data = (data << 1) | (digitalRead(pin_dout));
  }
  //Serial.println(data,HEX);
  digitalWrite(pin_slk, 1);
  delayMicroseconds(10);
  digitalWrite(pin_slk, 0);
  delayMicroseconds(10);
  return data ^ 0x800000;
}


long AE_HX711_Averaging(long adc, char num)
{
  long sum = 0;
  for (int i = 0; i < num; i++) sum += AE_HX711_Read();
  return sum / num;
}

float AE_HX711_getGram(char num)
{
#define HX711_R1  20000.0f
#define HX711_R2  8200.0f
#define HX711_VBG 1.25f
#define HX711_AVDD      4.2987f//(HX711_VBG*((HX711_R1+HX711_R2)/HX711_R2))
#define HX711_ADC1bit   HX711_AVDD/16777216 //16777216=(2^24)
#define HX711_PGA 128
#define HX711_SCALE     (OUT_VOL * HX711_AVDD / LOAD *HX711_PGA)

  float data;

  data = AE_HX711_Averaging(AE_HX711_Read(), num) * HX711_ADC1bit;
  data =  data / HX711_SCALE;


  return data;
