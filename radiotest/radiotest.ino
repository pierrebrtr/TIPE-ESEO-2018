#include <SPI.h>
#include <RF24.h>
 int buttonState = 0;
RF24 myRadio (5, 6);
byte addresses[][6] = {"0"};
int bpOld = 1;
int stateBP = 0;
struct package
{
  int X = 1;
  int Y = 1;
  int Z = 0;
};


typedef struct package Package;
Package data;


void setup()
{
  Serial.begin(115200);
  delay(100);
  myRadio.begin();
  myRadio.setChannel(1);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openWritingPipe( addresses[0]);
  pinMode(2, INPUT_PULLUP); 
  delay(100);
  
}

void loop()
{
  buttonState = digitalRead(2);
  if (buttonState == 0 && bpOld != buttonState) {
     if (stateBP == 0) {
      stateBP = 1;
     } else {
      stateBP = 0;
     }
  }
  
  myRadio.write(&data, sizeof(data));
  data.Z = stateBP;
  data.X = analogRead(A0);
  data.Y = analogRead(A1);
  delay(100);
  bpOld = buttonState;
}
