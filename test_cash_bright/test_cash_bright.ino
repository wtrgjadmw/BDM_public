// 紙幣を出し入れした時の照度の変化を確認するプログラム

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// 接続先のSSIDとパスワード
const char *ssid = "xxxxxxxx";     //無線ルーターのssidを入力
const char *password = "xxxxxxxx"; //無線ルーターのパスワードを入力

WiFiClientSecure sslclient;

const char *server = "script.google.com";

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------//
const int LED1000 = 4; // LEDのピン設定
const int LED5000 = 2;
const int LED10000 = 27;

const int CDS1000 = 33; // CdSセル接続ピン
const int CDS10000 = 15;
const int CDS_main = 12;

//---------------------------------------------------//
// プログラムで使う変数定義
//---------------------------------------------------//
bool closed; // 蓋閉じてるかどうか判定
bool preClosed = false;
const int closedBright = 300; // TODO: 蓋閉じてる時の明るさ設定
int preMoney = 0;

void setup()
{
  pinMode(CDS1000, INPUT);
  pinMode(CDS10000, INPUT);
  pinMode(CDS_main, INPUT);

  Serial.begin(9600);
  // connectWiFi();
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

    for (int j = 0; j < 10; j++)
    {
      int brightSum1000 = 0;
      // int brightSum5000 = 0;
      int brightSum10000 = 0;
      for (int i = 0; i < 10; i++)
      {
        brightSum1000 += analogRead(CDS1000);
        // brightSum5000 += analogRead(CDS5000);
        brightSum10000 += analogRead(CDS10000);
        delay(100);
      }
      float brightAve1000 = brightSum1000 / 10;
      // float brightAve5000 = brightSum5000 / 10;
      float brightAve10000 = brightSum10000 / 10;

      Serial.print("千円: ");
      Serial.print(brightAve1000);
      // Serial.print(", 五千円: ");
      // Serial.print(brightAve5000);
      Serial.print(", 一万円: ");
      Serial.println(brightAve10000);
      //      char str1000[10], str10000[10];
      //      dtostrf(brightAve1000, 8, 4, str1000);
      //      dtostrf(brightAve10000, 8, 4, str10000);
      //      sendData(str1000, str10000, "");
    }
    // digitalWrite(LED, LOW);
  }
  Serial.println("----------------");

  preClosed = closed;
  delay(1000);
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
  String url = "https://script.google.com/macros/s/AKfycbyQIOHoRLm_kllRpEN1grPp-q-frubj0AweroqUpmMpH5yWL4YRHdkAj9EO8lDrIuMNQg/exec"; //googlescript web appのurlを入力
  //urlの末尾に測定値を加筆
  url += "?";
  url += "1_cell=";
  url += balance;
  url += "&2_cell=";
  url += income;
  url += "&3_cell=";
  url += spending;

  connectServer();

  sslclient.println("GET " + url);
  delay(3000); //私の環境ではここに待ち時間を入れないとデータが送れないことがある
  sslclient.stop();

  Serial.println("データ送信完了");
  Serial.println(""); //改行
}
