#include <SPI.h>

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
int state = 0;
#define trigPin A0      //Trig 
#define echoPin A1     //Echo 
const float SOUND_SPEED = 340.0 / 1000;
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s
RF24 myRadio (7, 6);
struct package
{
  int X = 512;
  int Y = 512;
  int Z = 0;
};

byte addresses[][6] = {"0"};
int OUT1 = 3;
int OUT2 = 5;
int OUT3 = 9;
int OUT4 = 10;


typedef struct package Package;
Package data;

void setup()
{
  pinMode(4, OUTPUT);
  tone(4, 500); 
  delay(200);       
    tone(4, 600); 
  delay(200);       
    tone(4, 700);
  delay(200);     
  noTone(4); 
  delay(300); 

    
  tone(4, 800); 
  delay(200);
  noTone(4); 
  delay(70);     
   tone(4, 800); 
  delay(200); 
  noTone(4);  
  
  
  
  state = 0;
  Serial.begin(115200);
  delay(1000);
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);
  myRadio.begin();
  myRadio.setChannel(1);
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);

  analogWrite(OUT1, 0);
  analogWrite(OUT2, 0);
  analogWrite(OUT3, 0);
  analogWrite(OUT4, 0);

}

void autoDrive() {
  analogWrite(OUT3, 0);
  analogWrite(OUT4, 0);
  analogWrite(OUT1, 0);
  analogWrite(OUT2, 0);

  delay (500);

  analogWrite(OUT3, 255);
  analogWrite(OUT4, 0);
  analogWrite(OUT1, 0);
  analogWrite(OUT2, 255);

  delay(1000);

  analogWrite(OUT3, 0);
  analogWrite(OUT4, 255);
  analogWrite(OUT1, 0);
  analogWrite(OUT2, 255);

  delay (200);

  analogWrite(OUT3, 0);
  analogWrite(OUT4, 0);
  analogWrite(OUT1, 0);
  analogWrite(OUT2, 0);

  long duree, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); //Trig envois pendant 10ms
  digitalWrite(trigPin, LOW);

  // On calcul le temps pour l'aller retour du signal
  duree = pulseIn(echoPin, HIGH);
  distance = duree * 340 / (2 * 10000);

  analogWrite(OUT3, 255);
  analogWrite(OUT4, 0);
  analogWrite(OUT1, 255);
  analogWrite(OUT2, 0);

  delay (400);

  analogWrite(OUT3, 0);
  analogWrite(OUT4, 0);
  analogWrite(OUT1, 0);
  analogWrite(OUT2, 0);

  long dureebis, distancebis;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); //Trig envois pendant 10ms
  digitalWrite(trigPin, LOW);

  // On calcul le temps pour l'aller retour du signal
  dureebis = pulseIn(echoPin, HIGH);
  distancebis = dureebis * 340 / (2 * 10000);


  if ( distance <= distancebis) {
   state = 0;
   return;
  } else {
    analogWrite(OUT3, 0);
    analogWrite(OUT4, 255);
    analogWrite(OUT1, 0);
    analogWrite(OUT2, 255);

    delay (400);
    state = 0;
    return;
  }


}

void loop()
{


  if (myRadio.available())
  {
    while (myRadio.available())
    {
      myRadio.read( &data, sizeof(data) );
    }


    if (data.Z == 1) {

      if (state == 0) {
        analogWrite(OUT3, 0);
        analogWrite(OUT4, 255);
        analogWrite(OUT1, 255);
        analogWrite(OUT2, 0);
      } else {
        autoDrive();

      }
      long duree, distance;
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10); //Trig envois pendant 10ms
      digitalWrite(trigPin, LOW);

      // On calcul le temps pour l'aller retour du signal
      duree = pulseIn(echoPin, HIGH);
      distance = duree * 340 / (2 * 10000);

      if (distance <= 20) {
        state = 1;
      } else {
        state = 0;
      }

      Serial.println(distance);
    } else {


      int X = data.X;
      int Y = data.Y;
      int foward = map(X, 524, 1024, 0, 255);
      int backward = map(X, 500, 0, 0, 255);
      int right = map(Y, 524, 1024, 0, 255);
      int left = map(Y, 500, 0, 0, 255);
      if (X > 524 && Y < 524 && Y > 500) {
        analogWrite(OUT3, foward);
        analogWrite(OUT4, 0);
        analogWrite(OUT1, foward);
        analogWrite(OUT2, 0);
      } else if (X < 500 && Y < 524 && Y > 500) {
        analogWrite(OUT4, backward);
        analogWrite(OUT3, 0);
        analogWrite(OUT2, backward);
        analogWrite(OUT1, 0);
      } else if (X < 524 && X > 500 && Y < 524 && Y > 500) {
        analogWrite(OUT4, 0);
        analogWrite(OUT3, 0);
        analogWrite(OUT2, 0);
        analogWrite(OUT1, 0);
      } else if (X < 524 && X > 500 && Y > 524) {
        analogWrite(OUT4, 0);
        analogWrite(OUT3, left);
        analogWrite(OUT2, left);
        analogWrite(OUT1, 0);
      } else if (X < 524 && X > 500 && Y < 500) {
        analogWrite(OUT4, right);
        analogWrite(OUT3, 0);
        analogWrite(OUT2, 0);
        analogWrite(OUT1, right);
      } else if (X > 524 && Y > 524) {
        analogWrite(OUT3, foward);
        analogWrite(OUT4, 0);
        analogWrite(OUT1, foward - right);
        analogWrite(OUT2, 0);
      } else if (X > 524 && Y < 500) {
        analogWrite(OUT3, foward - left);
        analogWrite(OUT4, 0);
        analogWrite(OUT1, foward);
        analogWrite(OUT2, 0);
      } else if (X < 500 && Y > 524) {
        analogWrite(OUT4, backward);
        analogWrite(OUT3, 0);
        analogWrite(OUT2, backward - right);
        analogWrite(OUT1, 0);
      } else if (X < 500 && Y < 500) {
        analogWrite(OUT4, backward - left);
        analogWrite(OUT3, 0);
        analogWrite(OUT2, backward);
        analogWrite(OUT1, 0);
      }

    }

  }



}
