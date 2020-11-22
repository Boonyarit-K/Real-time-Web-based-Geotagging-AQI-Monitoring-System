#include "PMS.h"
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <DHT.h>

SoftwareSerial UnoSerial(7, 6); // RX | TX for communicate
TinyGPSPlus gps;
SoftwareSerial ss(13,12); // RX | TX for communicate
SoftwareSerial pmsSerial1(2, 3); // RX | TX for communicate
PMS pms1(pmsSerial1);
PMS::DATA data;
#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);  
  dht.begin();
  pinMode(7, INPUT);
  pinMode(6, OUTPUT);
  UnoSerial.begin(57600);
} 
void loop(){
  long node1[7];
  GPSlocate(node1); //push data to node1[5], node1[6]
  Checkdata(6,7,node1); //print check data after pushing pushing in node1[5],node1[6]
  
  long LAT = node1[0];
  long LONG = node1[1];
  long node1[5];
  long numPM1[3],numPM25[3],numPM10[3],numHUMI[3],numTEMP[3];

  for(int i=0 ; i<3 ; i++){
    datafromsensor(node1);
    Checkdata(1,5,node1); //print check data after pushing in node1[0] to node1[4]
    numPM1[i] = node1[0];
    numPM25[i] = node1[1];
    numPM10[i] = node1[2];
    numHUMI[i] = node1[3];
    numTEMP[i] = node1[4];
    delay(1000);
  }
  
  String PM1,PM25,PM10,HUMI,TEMP;
  PM1 = ConLonToStr(clean_data(PM1[0],PM1[1],PM1[2]));
  PM25 = ConLonToStr(clean_data(PM25[0],PM25[1],PM25[2]));
  PM10 = ConLonToStr(clean_data(PM10[0],PM10[1],PM10[2]));
  HUMI = ConLonToStr(clean_data(HUMI[0],HUMI[1],HUMI[2]));
  TEMP = ConLonToStr(clean_data(TEMP[0],TEMP[1],TEMP[2]));

  String data =  "FIRST"+PM1+":"+PM25+":"+PM10+":"+LAT+":"+LONG+":"+HUMI+":"+TEMP+"LAST";   //prepare data for send to nodemcu

  Serial.println(data);  //print check data that send to nidemcu
  UnoSerial.print(data);  // send data to nodemcu pass port 7,6(RX|TX)
  UnoSerial.print("\n");

}

//function for receive data from sensors
void datafromsensor(long arr[]) // function for get PM1.0,PM2.5,PM1.0,Humi,and Temp
{
  pmsSerial1.begin(9600);
  arr[0] = 0;
  while(arr[0] < 10 || arr[0] > 600){
  if (pms1.read(data))
  {
    arr[0] = data.PM_AE_UG_1_0;   //contain PM1
    arr[1] = data.PM_AE_UG_2_5;   //contain PM2.5
    arr[2] = data.PM_AE_UG_10_0;      //contain PM10
    arr[3] = dht.readHumidity();      //contain Humidity
    arr[4] = dht.readTemperature();      //contain Temp
    while( arr[3]<=0 || arr[4]<=0){
      arr[3] = dht.readHumidity();
      arr[4] = dht.readTemperature();
      break;
    }
    break;
  }
  }
}

//function for receive gps data
void GPSlocate(long arr[]){
  ss.begin(10000);
  arr[5] = 0;
  while(arr[5] == 0){
    if(ss.available() > 0){
      gps.encode(ss.read());
      if (gps.location.isUpdated() ){
        arr[5] = gps.location.lat()*100000;   //contain lattitude
        arr[6] = gps.location.lng()*100000;   //contain longitude
        Serial.println();
      }
    }
    break;
  }
}

//function for average value of PM data
long clean_data(long para1, long para2,long para3){
  long sum = (para1+para2+para3)/3;

  //in case of wrong data
  if(para1-sum>=10){
    sum = (para2+para3)/2;
  }
  else if(para2-sum>=10){
    sum = (para1+para3)/2;
  }
  else if(para3-sum>=10){
    sum = (para1+para2)/2;
  }
  return sum;
} 

//function for convert float to string
String ConLonToStr(long data){
  String text =  String(data, DEC);
  return text;
}

//function for print data and check
int Checkdata(int first, int last,long arr[]){
  for(int i = first-1;i<last;i++){
    Serial.print(arr[i]);
    Serial.print(" : ");
  }
  Serial.println();
} 
