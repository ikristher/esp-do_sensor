#include <SoftwareSerial.h>

#include <ESPert.h>
#define rx 14                                          
#define tx 12       

SoftwareSerial ezo(rx, tx, false, 256);    
ESPert espert;

String doReading = "";
boolean readingComplete = false;


String inputString = "";
boolean inputComplete = false;

float DO;
int readingCount = 0;

void setup() {
  Serial.begin(115200);
  ezo.begin(9600);
  delay(30);

  setupEZO();

 
  
}

void loop() {

  if(ezo.available()>0){
    char inChar = (char) ezo.read();
    doReading += inChar;

    if (inChar == '\r') {                             
      readingComplete = true;
    }
  }


  if(readingComplete == true){
    if(isdigit(doReading[0])){
      Serial.println(doReading);
      delay(1000);
      readingCount++;
      
    }
    doReading = "";
    readingComplete = false;
    
  }
  
  if(readingCount>=16){
    //upload data
    //check if high or low
    //turn on/off relay
    sleep();
    
  }
  
}

void setupEZO(){
  ezo.print("C,1");
  ezo.print("\r");
}

void sleep(){
  ezo.print("SLEEP\r");

 
  delay(2000);
  Serial.println("Enter Sleep Mode");
  ESP.deepSleep(10 * 1000000);
}


//void readSerialInput(){
//  
//  while(Serial.available()>0){
//    ezo.print(Serial.readStringUntil(13));
//    ezo.print('\r');
//  }
//}

