#include <WiFi.h>
#include <WiFiClientSecure.h>

// 接続先のSSIDとパスワード
const char *ssid = "xxxxxxxx";     //無線ルーターのssidを入力
const char *password = "xxxxxxxx"; //無線ルーターのパスワードを入力

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  sendData();
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

//測定データの送信
void sendData()
{

  WiFiClientSecure sslclient;

  const char *server = "script.google.com";
  String url = "https://script.google.com/macros/s/AKfycbyQIOHoRLm_kllRpEN1grPp-q-frubj0AweroqUpmMpH5yWL4YRHdkAj9EO8lDrIuMNQg/exec"; //googlescript web appのurlを入力

  //測定値を準備
  int val0, val1, val2, val3, val4, val5;
  val0 = analogRead(4); //A10 IO4 ADC2_CH0
  val1 = analogRead(0); //A11 IO0 ADC2_CH1
  val2 = analogRead(2);
  val3 = analogRead(15);
  val4 = analogRead(27);
  val5 = analogRead(12);
  Serial.print(val1);
  Serial.print(", ");
  Serial.print(val3);
  Serial.println(" ");

  //wifiに接続
  connectWiFi();

  //測定値の表示
  //Serial.println(sensor_data1);
  //Serial.println(sensor_data2);
  //Serial.println(sensor_data3);

  //urlの末尾に測定値を加筆
  url += "?";
  url += "&1_cell=";
  url += val1;
  url += "&2_cell=";
  url += val3;
  url += "&3_cell=";
  url += val5;

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

  sslclient.println("GET " + url);
  delay(1000); //私の環境ではここに待ち時間を入れないとデータが送れないことがある
  sslclient.stop();

  Serial.println("データ送信完了");
  Serial.println(""); //改行

  //WiFiを切断
  WiFi.mode(WIFI_OFF);
}
