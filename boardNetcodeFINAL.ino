

int sensorPin = A0;
float sensorValue = 0;
int pumpStatus1 = 0;
int pumpStatus2 = 0;
int httpCode = 0;
const int pumpPin1 = 4;
const int pumpPin2 = 5;



String getHtmlPage(){
  
  String css = "<style>.button {background-color: #990033;border: none;border-radius: 4px;color: white;padding: 7px 15px;text-align: center;text-decoration: none;text-decoration: none;}</style>";

String strHtml = "<!DOCTYPE html><html><head>";
strHtml += css;
strHtml += "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>";
strHtml += "<meta name=\"viewport\" content=\initial-scale=1,maximum-scale=1, ";
strHtml += "minimum-scale=1, width=device-width, height=device-height\">";
strHtml += "<title>YOU ARE NOT SUPPOSE TO BE HERE</title>";
strHtml += "</head><body><h1>Controller</h1>";
strHtml += "<h3>Status : ";
strHtml += "</h3>";
strHtml += "<br>";
strHtml += "</body></html>";
return strHtml;
}

#include "ThingSpeak.h"
//#include "secrets.h"

IPAddress staticIP(192, 168, 43, 90); //ESP static ip
IPAddress gateway(192, 168, 43, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);  //DNS

const char* deviceName = "chinArduino";
unsigned long chNum = 1131173; //Thingspeak Channel
const char * wAPIkey = "HT2JH1TY990E0REC"; //writeAPIkey
const char * rAPIkey = "P9B3PD2R07GTTC1X"; //readAPIkey
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
char ssid[] = "ChinVivo";   
char pass[] = "Chinchilla";  
//char ssid[] = "home";  
//char pass[] = "ronnachai";       
int keyIndex = 0;    
WiFiClient  client;
ESP8266WebServer server(80);
void setup() {
  Serial.begin(115200);
  pinMode(pumpPin1, OUTPUT);
  pinMode(pumpPin2, OUTPUT);
  delay(100);

  WiFi.mode(WIFI_AP_STA);

  server.on("/", handleRoot);
  server.on("/L1", setOFF1);
  server.on("/H1", setON1);
  server.on("/L2", setOFF2);
  server.on("/H2", setON2);
  server.begin();
  ThingSpeak.begin(client);
}

void loop() {
  
  wifiConnect();
  Serial.println(WiFi.localIP());
  
  cloudUpdate();
  readWaterPump();
  readFertPump();

  for(int i=0;i<200;i++){
  Serial.print(".");
  server.handleClient();
  delay(100);
  }
}

//-------------------------------------------------- ต่อไวไฟ -----------------------------------------------------

void wifiConnect(){
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to IP: ");
    Serial.println(WiFi.localIP());
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}

//-------------------------------------------------- อัพเดตลง Thingspeak --------------------------------------------------
void cloudUpdate() {
  
  // Measure Signal Strength (RSSI) of Wi-Fi connection
  long rssi = WiFi.RSSI();
  sensorValue = analogRead(sensorPin);
  sensorValue = map(sensorValue, 645, 1024, 100, 0);

  Serial.print("");
  ThingSpeak.setField(1, (int)pumpStatus1);
  ThingSpeak.setField(2, (int)pumpStatus2);
  ThingSpeak.setField(3, (float)sensorValue);
  ThingSpeak.setField(4, (long)rssi);

  //เขียนยกชุด in batch
   httpCode = ThingSpeak.writeFields(chNum, wAPIkey);
  
  delay(1000);

  if (httpCode == 200) {
    Serial.println("Channel write successful.");
    Serial.print("Soil moisture: ");
    Serial.print(sensorValue);
    Serial.println(" %");
  }
  else {
    Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
  }
  // Wait 20 seconds to update the channel again

}

void readWaterPump(){
  pumpStatus1 = ThingSpeak.readIntField(chNum, 1, rAPIkey);
  httpCode = ThingSpeak.getLastReadStatus();
  if(httpCode == 200){
    Serial.println("Water Pump: " + String(pumpStatus1));
    //digitalWrite(pumpPin1, pumpStatus1);
  }
  else{
    Serial.println("Problem reading channel. HTTP error code " + String(httpCode)); 
  }
 
}

void readFertPump(){
  pumpStatus2 = ThingSpeak.readIntField(chNum, 2, rAPIkey);
  httpCode = ThingSpeak.getLastReadStatus();
  if(httpCode == 200){
    Serial.println("Fertilizer Pump: " + String(pumpStatus2));
    //digitalWrite(pumpPin2, pumpStatus2);
    
  }
  else{
    Serial.println("Problem reading channel. HTTP error code " + String(httpCode)); 
  }
}

void handleRoot() {
  server.send(200, "text/html", getHtmlPage());
}

void setON1(){
  digitalWrite(pumpPin1, 0);
  pumpStatus1 = 0;
  server.send(200, "text/html", getHtmlPage());
}

void setOFF1(){
  digitalWrite(pumpPin1, 1);
  pumpStatus1 = 1;
  server.send(200, "text/html", getHtmlPage());
}

void setON2(){
  digitalWrite(pumpPin2, 0);
  pumpStatus2 = 0;
  server.send(200, "text/html", getHtmlPage());
}

void setOFF2(){
  digitalWrite(pumpPin2, 1);
  pumpStatus2 = 1;
  server.send(200, "text/html", getHtmlPage());
}
