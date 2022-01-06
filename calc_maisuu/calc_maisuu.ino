// 実際使うプログラム
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

String maisuu1000(float);
String maisuu5000(float);
String maisuu10000(float);
void connectWiFi(void);
void connectServer(void);
void sendData(String, String, String);

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------//
const int LED1000 = 4; // LEDのピン設定
const int LED5000 = 2;
const int LED10000 = 27;

const int CDS1000 = 33; // CdSセル接続ピン
const int CDS10000 = 15;
const int CDS_main = 12;

// 接続先のSSIDとパスワード
const char *ssid = "xxxxxxxx";     //無線ルーターのssidを入力
const char *password = "xxxxxxxx"; //無線ルーターのパスワードを入力

WiFiClientSecure sslclient;

const char *server = "script.google.com";

//---------------------------------------------------//
// プログラムで使う変数定義
//---------------------------------------------------//
bool closed; // 蓋閉じてるかどうか判定
bool preClosed = false;
const int closedBright = 300; // TODO: 蓋閉じてる時の明るさ設定
int preMoney = 0;
int maxBright1000 = 1400;
int maxBright10000 = 800;

void setup()
{
  pinMode(CDS1000, INPUT);
  pinMode(CDS10000, INPUT);
  pinMode(CDS_main, INPUT);

  Serial.begin(9600);
  //wifiに接続
  // connectWiFi();
  delay(1000);
}

void loop()
{
  int brightSum = 0;
  for (int i = 0; i < 10; i++)
  {
    brightSum += analogRead(CDS_main);
    delay(100);
  }

  float brightAve = brightSum / 10;
  closed = (brightAve <= closedBright);
  delay(500);

  Serial.println(brightAve);
  Serial.print("closed: ");
  Serial.println(closed);
  Serial.print("preClosed: ");
  Serial.println(preClosed);

  // 蓋が閉まっている，かつ前のループでは蓋が開いていた時金額を計算する
  if (closed && !preClosed)
  {
    int brightSum1000 = 0;
    // int brightSum5000 = 0;
    int brightSum10000 = 0;
    int money;

    for (int i = 0; i < 10; i++)
    {
      brightSum1000 += analogRead(CDS1000);
      // brightSum5000 += analogRead(CDS5000);
      brightSum10000 += analogRead(CDS10000);
      delay(100);
    }

    float brightAve1000 = brightSum1000 / 10;
    // float brightAve5000 = brightSum5000 / 10;
    float brightAve5000 = 0;
    float brightAve10000 = brightSum10000 / 10;

    Serial.print("千円: ");
    Serial.print(brightAve1000);
    Serial.print(", 五千円: ");
    Serial.print(brightAve5000);
    Serial.print(", 一万円: ");
    Serial.println(brightAve10000);

    // money += maisuu1000(brightAve1000) * 1000;
    // money += maisuu5000(brightAve5000) * 5000;
    // money += maisuu10000(brightAve10000) * 10000;

    sendData(maisuu1000(brightAve1000), maisuu5000(brightAve5000), maisuu10000(brightAve10000));

    preMoney = money;
  }
  Serial.println("----------------");
  preClosed = closed;
  delay(1000);
}

// とりあえず6枚まで
String maisuu1000(float brightAve)
{
  if (brightAve < maxBright1000 * 0.01)
  { // TODO: 数値変える
    return "3";
  }
  else if (brightAve < maxBright1000 * 0.03)
  {
    return "2";
  }
  else if (brightAve < maxBright1000 * 0.5)
  {
    return "1";
  }
  else
  {
    return "0";
  }
}

String maisuu5000(float brightAve)
{
  if (brightAve < 10)
  { // TODO: 数値変える
    return "3";
  }
  else if (brightAve < 50)
  {
    return "2";
  }
  else if (brightAve < 300)
  {
    return "1";
  }
  else
  {
    return "0";
  }
}

String maisuu10000(float brightAve)
{
  if (brightAve < maxBright10000 * 0.01)
  { // TODO: 数値変える
    return "3";
  }
  else if (brightAve < maxBright10000 * 0.05)
  {
    return "2";
  }
  else if (brightAve < maxBright10000 * 0.5)
  {
    return "1";
  }
  else
  {
    return "0";
  }
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
void sendData(String balance, String income, String spending)
{
  //wifiに接続
  connectWiFi();

  String url = "https://script.google.com/macros/s/AKfycbyQIOHoRLm_kllRpEN1grPp-q-frubj0AweroqUpmMpH5yWL4YRHdkAj9EO8lDrIuMNQg/exec"; //googlescript web appのurlを入力
  //urlの末尾に測定値を加筆
  url += "?";
  url += "1_cell=";
  url += balance;
  url += "&2_cell=";
  url += "0";
  url += "&3_cell=";
  url += spending;

  connectServer();

  sslclient.println("GET " + url);
  delay(3000); //私の環境ではここに待ち時間を入れないとデータが送れないことがある
  sslclient.stop();

  Serial.println("データ送信完了");
  Serial.println(""); //改行

  WiFi.mode(WIFI_OFF);
}
