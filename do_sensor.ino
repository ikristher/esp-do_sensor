#include <SoftwareSerial.h>

#include <ESPert.h>
#include "ThingSpeak.h"

#define rx 14                                          
#define tx 12       

SoftwareSerial ezo(rx, tx, false, 256);    
ESPert espert;
WiFiClient client;

const char *host = "api.thingspeak.com";

String apiKey = "RVA45Z16Q3DB8W19";

String doReading = "";
boolean readingComplete = false;

int readingCount = 0;


int sleepTime = 60; //sleep in seconds

void setup() {
  
  ezo.begin(9600);
  setupEZO();
  delay(30);


  setupWifi();
}

void setupWifi(){
  espert.init();

  espert.oled.init();
  delay(2000);

  espert.oled.clear();
  espert.oled.println(espert.info.getId());
  espert.oled.println();

  int mode = espert.wifi.init();

  if (mode == ESPERT_WIFI_MODE_CONNECT) {
    espert.println(">>> WiFi mode: connected.");
    espert.oled.println("WiFi: connected.");
    espert.oled.print("IP..: ");
    espert.oled.println(espert.wifi.getLocalIP());
  } else if (mode == ESPERT_WIFI_MODE_DISCONNECT) {
    espert.println(">>> WiFi mode: disconnected.");
    espert.oled.println("WiFi: not connected.");
  } else if (mode == ESPERT_WIFI_MODE_SMARTCONFIG) {
    espert.println(">>> WiFi mode: smart config.");
  } else if (mode == ESPERT_WIFI_MODE_SETTINGAP) {
    espert.println(">>> WiFi mode: access point.");
  }
}

void loop() {

  espert.loop();
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
    if(readingCount>=16){
      
      uploadData();
      //check if high or low
      //turn on/off relay
      sleep();
      
    }
    doReading = "";
    readingComplete = false;
    
  }
  
  
  
}

void uploadData(){


  String postStr = apiKey;
  postStr +="&field1=" + String(doReading.toFloat());

  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    return;
  }



  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

void setupEZO(){
  ezo.print("C,1\r");
  
}

void sleep(){
  ezo.print("SLEEP\r");

  delay(2000);
  Serial.println("Entering Sleep Mode");
  
  ESP.deepSleep(sleepTime * 1000000);
}


//void readSerialInput(){
//  
//  while(Serial.available()>0){
//    ezo.print(Serial.readStringUntil(13));
//    ezo.print('\r');
//  }
//}

