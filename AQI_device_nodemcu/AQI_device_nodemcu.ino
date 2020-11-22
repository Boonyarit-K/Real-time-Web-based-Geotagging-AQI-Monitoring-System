#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h>
#include <time.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#define ESP_AP_NAME "AQI project"
#define FIREBASE_HOST "iot-realtime-aqi.firebaseio.com"
#define FIREBASE_AUTH "tUqwEKpeDJYtODi5TcNVwcN2sF7Wef8BxhUm1pLN"

SoftwareSerial NodeSerial(D7, D6); // RX | TX set for receive data from arduino
int timezone = 7 * 3600; //set TimeZone
int dst = 0;

void setup() {
  Serial.begin(9600);
  WiFiManager wifiManager;
  wifiManager.autoConnect(ESP_AP_NAME);
  while (WiFi.status() != WL_CONNECTED)
  {
     delay(250);
     Serial.print(".");
  }
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  Serial.begin(9600);
  NodeSerial.begin(57600);
  Serial.println();
  Serial.println("NodeMCU/ESP8266 Run");
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nLoading time");
  while (!time(nullptr)) {
  Serial.print("*");
  delay(1000);
  }
}

void loop() {
  //collect data received from arduino
  if(NodeSerial.available() > 0)
  {
    String text = NodeSerial.readString();   //read data that get from arduino
    Serial.println(text);
    int firstch = count(text,"FIRST",0);
    int lastch = count(text,"LAST",0);   //count first position
    
    while(lastch<firstch){
      int lastch = count(text,"LAST",1);   //count second position
      if(lastch>firstch){
        break;
      }
    }
    
    Serial.println(firstch);
    Serial.println(lastch);
    
    String newtext = Cuttext(text,firstch+5,lastch);
    Serial.println(newtext);

    //separate data and convert data
    int countPM1 = count(newtext,":",0);
    int countPM25 = count(newtext,":",1);
    int countPM10 = count(newtext,":",2);
    int countLAT = count(newtext,":",3);
    int countLONG = count(newtext,":",4);
    int countHUMI = count(newtext,":",5);
    int countTEMP = count(newtext,":",6);
    
    float PM1 = ConStrToFlo(Cuttext(newtext,0,countPM1));
    float PM25 = ConStrToFlo(Cuttext(newtext,countPM1+1,countPM25));
    float PM10 = ConStrToFlo(Cuttext(newtext,countPM25+1,countPM10));
    float LAT = ConStrToFlo(Cuttext(newtext,countPM10+1,countLAT));
    float LONG = ConStrToFlo(Cuttext(newtext,countLAT+1,countLONG));
    float HUMI = ConStrToFlo(Cuttext(newtext,countLONG+1,countHUMI));
    float TEMP = ConStrToFlo(Cuttext(newtext,countHUMI+1,countTEMP));

    //call date and time from server
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    float TIME = ((p_tm->tm_hour)*10000)+((p_tm->tm_min)*100)+(p_tm->tm_sec);
    float DATE = ((p_tm->tm_year-100)*10000)+((p_tm->tm_mon+1)*100)+(p_tm->tm_mday);

  //push data to firebase
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["pm1"] = PM1;
  root["pm25"] = PM25;
  root["pm10"] = PM10;
  root["lat"] = LAT;
  root["long"] = LONG;
  root["humidity"] = HUMI;
  root["temperature"] = TEMP;
  root["time"] = TIME;
  root["date"] = DATE;
  String name = Firebase.push("NODE1", root);
  if (Firebase.failed()) {
      Serial.print("pushing /AQI sensor1 failed:");
      Serial.println(Firebase.error()); 
      return;
  }
  Serial.print("pushed: /AQI sensor1/");
  Serial.println(name);
  }
}

//function for convert string to float
float  ConStrToFlo(String data){
  float result = data.toFloat(); 
  return result;
}

//function for count position of data
  // cond = 0 mean count text in 1st position
  // cond = 1 mean count text in 2nd position
  // cond = n mean count text in (n+1)nd position
int count(String data,String text,int cond){
  int count = data.indexOf(text,count);
  for(int i =1 ; i<=cond ; i++){
      count = data.indexOf(text,count+i);
  }
  return count;
}

//function for cut text to each data set
String Cuttext(String data,int first,int last){
  String newtext = data.substring(first,last);
  return newtext;
}
