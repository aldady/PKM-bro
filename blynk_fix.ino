#define BLYNK_PRINT Serial
#include <SimpleTimer.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

#define BLYNK_TEMPLATE_ID "TMPLcC9egfkW"
#define BLYNK_DEVICE_NAME "PKM"
#define BLYNK_AUTH_TOKEN "c0Jx9MtyF69aHLO8NJuiVfTurGr3vI4O"

char auth[] = "c0Jx9MtyF69aHLO8NJuiVfTurGr3vI4O";

char ssid[] = "Alda";
char pass[] = "aldarido1";

SimpleTimer timer;

String myString;

int rNH3,rDebu,rsuhu,rCO2,rCO,rHumidity,rFan1;

void myTimerEvent(){
  Blynk.virtualWrite(V3,millis()/1000);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth,ssid,pass,"blynk.cloud",80);
  timer.setInterval(1000L,HumidityVal);
  timer.setInterval(1000L,suhuval);
  timer.setInterval(1000L,CO2val);
  timer.setInterval(1000L,Debuval);
  timer.setInterval(1000L,COval);
  timer.setInterval(1000L,NH3val);
}

void loop() {
  if(Serial.available() == 0){
    Blynk.run();
    timer.run();
  }
  if(Serial.available() > 0){
    char rdata = Serial.read();
    myString = myString + rdata;
    //Serial.print(myString);
    if(rdata == '\n'){
      String a = getValue(myString,',',0);
      String b = getValue(myString,',',1);
      String c = getValue(myString,',',2);
      String d = getValue(myString,',',3);
      String e = getValue(myString,',',4);
      String f = getValue(myString,',',5);

      rHumidity = a.toInt();
      rsuhu = b.toInt();
      rCO2 = c.toInt();
      rDebu = d.toInt();
      rCO = e.toInt();
      rNH3 = f.toInt();

      Serial.println(rHumidity);
      Serial.println(rsuhu);
      Serial.println(rCO2);
      Serial.println(rDebu);
      Serial.println(rCO);
      Serial.println(rNH3);
      
      myString = "";
    }
  }
}

void NH3val(){
  int sdata = rNH3 ;
  Blynk.virtualWrite(V16, sdata);
}

void Debuval(){
  int sdata = rDebu;
  Blynk.virtualWrite(V17, sdata);
}

void suhuval(){
  int sdata = rsuhu;
  Blynk.virtualWrite(V15, sdata);
}
void CO2val(){
  int sdata = rCO2;
  Blynk.virtualWrite(V7, sdata);
}
void HumidityVal(){
  int sdata = rHumidity ;
  Blynk.virtualWrite(V14, sdata);
}
void COval(){
  int sdata =  rCO;
  Blynk.virtualWrite(V13, sdata);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
