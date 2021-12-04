#include<LiquidCrystal_I2C.h>
#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN A3
LiquidCrystal_I2C lcd(0x27, 20, 4);
float value=0;
float rev=0;
int rpm;
int oldtime = 0;
int time;
int speed;
float wheel_size = 26;
float perimeter;
int echoPinF = 4;
int trigPinF = 3; 
long durationF;
int distanceF;
int echoPinB = 5;
int trigPinB = 6; 
int buzzer = 7;
long durationB;
int distanceB; 
int light = A0;
int white = 8;
int red = 9;
int valuel;
const int MPU=0x68; 
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int t;
const int buttonR = 11;          
const int ledR =13; 
const int buttonL = 10;          
const int ledL =12;               
int ledflagR=0; 
int ledflagL=0;                 
int potent = A1;
float sensor_volt;
float RS_gas; 
float R0;
float ratio;
float BAC;
float BAC_pr;
int R2 = 2000;
float biggest_BAC = 0.00;
int val;

void isr() {
rev++;
}
 
void setup(){
  pinMode(trigPinF, OUTPUT);
  pinMode(echoPinF, INPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(light,INPUT);
  pinMode(white,OUTPUT);
  pinMode(red,OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);
  lcd.begin(); 
  attachInterrupt(0,isr,RISING); 
  pinMode(buttonR,INPUT);       
  pinMode(ledR,OUTPUT);         
  digitalWrite(ledR,LOW);
  pinMode(buttonL,INPUT);       
  pinMode(ledL,OUTPUT);         
  digitalWrite(ledL,LOW); 
  Serial.begin (9600);

  for (int i = 0; i < 5; i++){
    int sensorValue = analogRead(A7);
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = ((5.0 * R2)/sensor_volt) - R2; 
    R0 = 16000;
    ratio = RS_gas/R0;
    double x = 0.4*ratio;   
    BAC = pow(x,-1.431);
    BAC_pr = abs((BAC*0.001)-0.1); 
    Serial.print("BAC = ");
    Serial.print(BAC_pr);
    Serial.print(" g/L\n\n");
    lcd.setCursor(0,1);
    lcd.print("Exhale in the sensor");
    if (BAC_pr > biggest_BAC){
      biggest_BAC = BAC_pr;
      } 
    delay(1000);
  }
  if (biggest_BAC > 0.4){
    while(true){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Alcohol level is: ");
      lcd.setCursor(0,1);
      lcd.print(BAC_pr);
      lcd.print(" g/L");
      lcd.setCursor(0,2);
      lcd.print("You can't ride after");
      lcd.setCursor(0,3);
      lcd.print("consuming alcohol!");
      while (true){
        digitalWrite(buzzer,HIGH);
        delay(500);
        digitalWrite(buzzer,LOW);
        delay(500);
        }
      }
    }
    else 
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Alcohol level is: ");
      lcd.setCursor(0,1);
      lcd.print(BAC_pr);
      lcd.print(" g/L");
      lcd.setCursor(0,2);
      lcd.print("Have a safe ride!");
      delay(2000);
      lcd.clear();
      }
}
 
void loop(){
   val = analogRead(potent);
  Serial.println (val);
  if (digitalRead(buttonR)==HIGH){ 
   
    if (ledflagR==0) { 
                  
      ledflagR=1;                 

      }                         
    else {                        
      ledflagR=0;                 

    }
    if (val<400){
      ledflagR = 0;
      }
      
 delay(1000);                  
  }
  if (ledflagR==1){
    do {
      digitalWrite(ledR,HIGH);
      delay(500);
      digitalWrite(ledR,LOW);
      delay(250);
      }
      while(ledflagR==0);
      }            

       if (digitalRead(buttonL)==HIGH){ 
    if (ledflagL==0) {             
      ledflagL=1;                 

      }                         
    else {                        
      ledflagL=0;                 

    }
    if (val>510){
      ledflagL = 0;
      }
      
 delay(1000);                  
  }
  if (ledflagL==1){
    do {
      digitalWrite(ledL,HIGH);
      delay(500);
      digitalWrite(ledL,LOW);
      delay(500);
      }
      while(ledflagL==0);
      }
  delay (1000);
  detachInterrupt(0); 
  time=millis()-oldtime; 
  rpm=((rev/time)*60000);
  int rpmshow = rpm/3;
  oldtime=millis(); 
  rev=0;
  perimeter=3.14*((wheel_size*2.54)/2);
  speed=((rpm*perimeter)/60)/9;

  attachInterrupt(0,isr,RISING);
  digitalWrite(trigPinF, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinF, HIGH);
  delay(250);
  digitalWrite(trigPinF, LOW);
  durationF = pulseIn(echoPinF, HIGH);
  distanceF = (durationF * 0.034) / 2; 
   digitalWrite(trigPinB, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinB, HIGH);
  delay(250);
  digitalWrite(trigPinB, LOW);
  durationB = pulseIn(echoPinB, HIGH);
  distanceB = (durationB * 0.034) / 2;
  
  Serial.print("F: ");
  Serial.print(distanceF);
  Serial.println(" cm");
  lcd.clear();
  lcd.setCursor(0,3);
   lcd.print("Road level:");
  lcd.setCursor(0,2);
  lcd.print("F:");
  lcd.print(distanceF);
  lcd.print("cm");
  Serial.print("B: ");
  Serial.print(distanceB);
  Serial.println("cm");
  lcd.setCursor(9,2);
  lcd.print("B:");
  lcd.print(distanceB);
  lcd.print("cm");
 
  lcd.setCursor(0,0);
  lcd.print( rpmshow);
  lcd.print(" RPM");
  lcd.setCursor(0,1);
  lcd.print("Speed:");
  lcd.print( speed);
  lcd.print("Km/h");
  
  if ( distanceF < 10 or distanceB < 10){
  digitalWrite (buzzer, HIGH);
  digitalWrite (red,LOW);
  delay(250);
  digitalWrite(red,HIGH);
  delay(250);
  digitalWrite (red,LOW);
  delay(250);
  digitalWrite(red,HIGH);
  delay(250);
  digitalWrite (red,LOW);
  delay(250);
  digitalWrite(red,HIGH);
  delay(250);
  digitalWrite (red,LOW);
  delay(250);
  digitalWrite(red,HIGH);
  delay(250);
    }
  else {
    digitalWrite(buzzer, LOW);
    }
    
    valuel = analogRead(light);
 Serial.print("Light:");
 Serial.println(valuel);
 delay(1000);
  
 if (valuel < 310){
  digitalWrite(white,HIGH);
  digitalWrite(red,HIGH);
  }
  
  else{
    digitalWrite(white,LOW);
    digitalWrite(red,LOW);
    }
    
    Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);  
  GyX=Wire.read()<<8|Wire.read();  
  
    t = abs((GyX/32.8)+25);
  Serial.print("Gyroscope: ");
  Serial.print("X = "); 
  Serial.print(GyX);
  
  Serial.println(" ");
 
  lcd.setCursor(12,3);

  lcd.print(t);  

  lcd.setCursor(14,0);
  int chk = DHT.read11(DHT11_PIN);

  lcd.print("t= ");
  lcd.print((int)DHT.temperature);
  delay(4000);
  int temp = (DHT.temperature);
  Serial.print(temp);

    if (temp < 5){
    digitalWrite (buzzer, HIGH);
    delay(2000);
    digitalWrite (buzzer, LOW);
   
    }  
}
