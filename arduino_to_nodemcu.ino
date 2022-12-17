//declarasi komponen
#include <dht11.h>
#include "LiquidCrystal_I2C.h"
#include "MQ135.h"
#include "MQ7.h"
#include <SoftwareSerial.h>

#define DHT11PIN 3 //pin dht11 di 2
#define RL 10 //nilai RL =10 pada sensor
#define m -0.417 //hasil perhitungan gradien
#define b 0.425 //hasil perhitungan perpotongan
#define Ro 19 //hasil pengukuran RO
#define fan 4 // pin relay to exhaust fan
#define fan 2

dht11 DHT11;
MQ7 mq7(A0, 5.0);
MQ135 mq135(A1, 5.0);
LiquidCrystal_I2C lcd(0x27, 16, 4);

const int AOUTpin = 0; //the AOUT pin of the CO2  dan co sensor goes into analog pin A0 of the arduino
const int numReadings = 4;//nilai penambilan sample pembacaan sebesar 5 kali
const int relay1 = 2; //pin2
const int relay2 = 4; //pin3


int measurePin = A2; //Connect dust sensor to Arduino A0 pin
int R0 = 176;
int R2 = 1000;
int ledPower = 5;   //Connect 3 led driver pins of dust sensor to Arduino D
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
int value7;
int value13;
int readIndex = 0;
int buzzer = 6;
int relayON = LOW; //relay nyala
int relayOFF = HIGH; //relay mati

float RS;
float PPM_acetone;
float readings[numReadings];
float total = 0;
float average = 0;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

String sdata;

SoftwareSerial espSerial(9,10);



void setup()
{
  Serial.begin(9600);
  espSerial.begin(115200);
  lcd.begin();
  pinMode(ledPower, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, relayOFF);
  digitalWrite(relay2, relayOFF);
}

void loop()
{
  int chk = DHT11.read(DHT11PIN);
  Serial.println();
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 0);
  Serial.print("Temperature (C): ");
  Serial.println((float)DHT11.temperature, 0);
  lcd.setCursor(0, 0);
  lcd.print("---Kualitas Udara---");
  lcd.setCursor(0, 1);
  lcd.print("hum:");
  lcd.print((float)DHT11.humidity, 2);
  lcd.print("%");
  lcd.setCursor(11, 1);
  lcd.print("Tmp :");
  lcd.print((float)DHT11.temperature, 0);
  lcd.print("C");
  delay(2000);

  value7 = analogRead(AOUTpin); //reads the analaog value from the CO sensor's AOUT pin
  Serial.print("CO value: ");
  Serial.println(mq7.getPPM());
  lcd.setCursor(0, 2);
  lcd.print("CO :");
  lcd.print((float)mq7.getPPM(), 2);
  delay(100);

  value13 = analogRead(AOUTpin); //reads the analaog value from the CO2 sensor's AOUT pin
  Serial.print("The amount of CO2 (in PPM): ");//Serial.println(value13);
  Serial.println(mq135.getPPM());
  lcd.setCursor(11, 2);
  lcd.print("CO2 :");
  lcd.print((float)mq135.getPPM(), 2);
  delay(2000);

  float VRL;
  float RS;
  float ratio;

  VRL = analogRead(AOUTpin) * (5 / 1023.0); //konversi analog ke tegangan
  RS = (5.0 / VRL - 1) * 10 ; //rumus untuk RS
  ratio = RS / Ro; // rumus mencari ratio
  float ppm = pow(10, ((log10(ratio) - b) / m)); //rumus mencari ppm
  total = total - readings[readIndex];
  readings[readIndex] = ppm;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  average = total / numReadings;
  Serial.println();
  Serial.print("amonia: ");
  Serial.println(average);
  //menampilkan nilai rata-rata ppm setelah 5 kali pembacaan
  lcd.setCursor(0, 3);
  lcd.print("NH3:");
  lcd.print((float)average, 2);


  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = 170 * calcVoltage - 0.0;


  Serial.println(dustDensity); // unit: ug/m3
  lcd.setCursor(11, 3);
  lcd.print("dust:");
  lcd.print(dustDensity);
  delay(1000);

  //kontrol buzzer  dan relay pada beberapa kondisi
  if ( (float)DHT11.humidity > 88) {    //jika kelembapan lebih besar dari 82.10
    digitalWrite(buzzer, HIGH);   //buzzer menyala
    delay(1000);
    digitalWrite(relay1, relayON);
    digitalWrite(relay2, relayON);
    delay(1000);
    noTone(buzzer);     // Stop sound...
    delay(1000);        // ...for 1sec
  }

  else if ( (float)DHT11.temperature > 28) {       //jika suhu lebih besar dari 32.10
    digitalWrite(buzzer, HIGH);   //buzzer menyala
    delay(1000);
    digitalWrite(relay1, relayON);
    digitalWrite(relay2, relayON);
    delay(1000);
    noTone(buzzer);     // Stop sound...
    delay(1000);        // ...for 1sec
  }

  else {                          //jika tidak
    digitalWrite(buzzer, LOW);    //buzzer mati
  }
  if ( (float)average, 2 > 50) {    //jika NH3 lebih besar dari 50
    digitalWrite(buzzer, HIGH);   //buzzer menyala
    delay(1000);
    digitalWrite(relay1, relayON);
    digitalWrite(relay2, relayON);
    delay(1000);
    noTone(buzzer);     // Stop sound...
    delay(1000);        // ...for 1sec
  }
  else {                          //jika tidak
    digitalWrite(buzzer, LOW);    //buzzer mati

  }
  if ( (float)mq7.getPPM(), 2 > 10) {    //jika co lebih dari 20
    digitalWrite(buzzer, HIGH);   //buzzer menyala
    delay(1000);
    digitalWrite(relay1, relayON);
    digitalWrite(relay2, relayON);
    delay(1000);       // ...for 1sec
  }

  else {                          //jika tidak
    digitalWrite(buzzer, LOW);    //buzzer mati
  }
  if ((float)mq135.getPPM(), 2 > 1) {    //jika co2 lebih dari 0.5
    digitalWrite(buzzer, HIGH);   //buzzer menyala
    delay(1000);
    digitalWrite(relay1, relayON);
    digitalWrite(relay2, relayON);
    delay(1000);     // ...for 1sec
    noTone(buzzer);     // Stop sound...
    delay(1000);
  }
  else {                          //jika tidak
    digitalWrite(buzzer, LOW);    //buzzer mati
  }
  if (dustDensity > 10) {     //jika kelembapan lebih besar dari 82.10
    digitalWrite(buzzer, HIGH);   //buzzer menyala
    delay(1000);
    digitalWrite(relay1, relayON);
    digitalWrite(relay2, relayON);
    delay(1000);
    noTone(buzzer);     // Stop sound...
    delay(1000);        // ...for 1sec
  }
  else {                          //jika tidak
    digitalWrite(buzzer, LOW);    //buzzer mati
    delay(1000);
  }

  int p= (float)DHT11.humidity;
  int q= (float)DHT11.temperature;
  int r= (float)mq135.getPPM();
  int s= dustDensity;
  int t= (float)mq7.getPPM();
  int u= (float)average;


  
  if (Serial.available() == 0) {
    sdata = sdata + p + "," + q + "," + r + "," + s + "," + t + "," + u ;
    Serial.print("sdata: ");
    Serial.println(sdata);
    espSerial.println(sdata);
    delay(1000);
    sdata = "";
  }
}
