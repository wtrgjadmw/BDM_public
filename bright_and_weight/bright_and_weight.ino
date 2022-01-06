#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define OUT_VOL 0.001f //定格出力 [V]
#define LOAD 2000.0f   //定格容量 [g]

void AE_HX711_Init(void);
void AE_HX711_Reset(void);
long AE_HX711_Read(void);
long AE_HX711_Averaging(long adc, char num);
float AE_HX711_getGram(char num);

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------//
#define pin_dout 27
#define pin_slk 12
const int CDS = 35;   // CdSセル接続ピン
const int PWM_CH = 0; // PWMチャンネル

float offset;

// 接続先のSSIDとパスワード
const char *ssid = "xxxxxxxx";     //無線ルーターのssidを入力
const char *password = "xxxxxxxx"; //無線ルーターのパスワードを入力

WiFiClientSecure sslclient;

const char *server = "script.google.com";

void setup()
{
  pinMode(CDS, INPUT);

  Serial.begin(9600);
  Serial.println("AE_HX711 test");
  AE_HX711_Init();
  AE_HX711_Reset();
  offset = AE_HX711_getGram(30);
  //wifiに接続
  connectWiFi();
}

void loop()
{
  int bright = analogRead(CDS);

  float data;
  char S1[20];
  char s[20];

  double gram0;

  data = -AE_HX711_getGram(5);
  dtostrf((data - offset), 5, 3, s);
  Serial.println(bright);
  Serial.println(s);
  sendData("1.2", "1.4");
  delay(5000);
}

//WiFiに接続
void connectWiFi()
{

  Serial.print("ssid:");
  Serial.print(ssid);
  Serial.println(" に接続します。");

  WiFi.begin(ssid, password);

  Serial.print("WiFiに接続中");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("接続しました。");

  //IPアドレスの表示
  Serial.print("IPアドレス:");
  Serial.println(WiFi.localIP());
}

void connectServer()
{
  // サーバーにアクセス
  Serial.println("サーバーに接続中...");
  sslclient.setInsecure();
  //データの送信
  if (!sslclient.connect(server, 443))
  {
    Serial.println("接続に失敗しました");
    Serial.println(""); //改行
    return;
  }

  Serial.println("サーバーに接続しました");
}

//測定データの送信
void sendData(String bright, String weight)
{
  String url = "https://script.google.com/macros/s/AKfycbyQIOHoRLm_kllRpEN1grPp-q-frubj0AweroqUpmMpH5yWL4YRHdkAj9EO8lDrIuMNQg/exec"; //googlescript web appのurlを入力
  //urlの末尾に測定値を加筆
  url += "?";
  url += "1_cell=";
  url += bright;
  url += "&2_cell=";
  url += weight;

  connectServer();

  sslclient.println("GET " + url);
  delay(3000); //私の環境ではここに待ち時間を入れないとデータが送れないことがある
  sslclient.stop();

  Serial.println("データ送信完了");
  Serial.println(""); //改行
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
  while (digitalRead(pin_dout) != 0)
    ;
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
  for (int i = 0; i < num; i++)
    sum += AE_HX711_Read();
  return sum / num;
}

float AE_HX711_getGram(char num)
{
#define HX711_R1 20000.0f
#define HX711_R2 8200.0f
#define HX711_VBG 1.25f
#define HX711_AVDD 4.2987f                  //(HX711_VBG*((HX711_R1+HX711_R2)/HX711_R2))
#define HX711_ADC1bit HX711_AVDD / 16777216 //16777216=(2^24)
#define HX711_PGA 128
#define HX711_SCALE (OUT_VOL * HX711_AVDD / LOAD * HX711_PGA)

  float data;

  data = AE_HX711_Averaging(AE_HX711_Read(), num) * HX711_ADC1bit;
  data = data / HX711_SCALE;

  return data;
}
