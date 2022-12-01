#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <ESP8266WiFiMulti.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h> 
#include <Arduino_JSON.h>
#include <Arduino.h>
#include "GetScheduleJson.h"
#include "GetStatusJson.h"

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
WiFiClient client;
//WiFiClientSecure client;

#define SS_PIN        D8     
#define RST_PIN       D4 

byte uid[4];
String cardID;
const char* tagcard;

MFRC522 rfid(SS_PIN, RST_PIN); 

//ESP8266WiFiMulti WiFiMulti;

//konfigurasi WiFi
const char *ssid = "MSI 3906";
const char *pass = "12345678";
//Your Domain name with URL path or IP address with path
const char* serverName = "card.conect.id"; ///api/user?id_card=11273926&apikey=8105e86f-b71e-4a45-9708-5f1295b62263";

//Your Domain name with URL path or IP address with path
String namaServer = "https://card.conect.id/api/user?id_card=";
String namaServer1 = "&apikey=8105e86f-b71e-4a45-9708-5f1295b6226";


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

String sensorReadings;
float sensorReadingsArr[3];



void connectWifi(){
//  Serial.begin(115200); //Default Baudrate
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println("");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  
    //jika wifi tersambung
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}
void setup() {
  // put your setup code here, to run once:
//  Serial.begin(9600);
  Serial.begin(115200);
  connectWifi();
  lcd.init();
  lcd.backlight();
  SPI.begin();

  // Initial MFRC RFID Reader Module 
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
//  lcd.clear();
}

void loop() {
  
  lcd.setCursor(0, 0);
  lcd.print("Nama Gym");
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    for (byte i=0; i<4; i++) {
      uid[i] = rfid.uid.uidByte[i];
    }
    tulisHexa(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
//    conbackWeb();
     
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void tulisHexa (byte *buffer, byte bufferSize){
  cardID = "";
  Serial.print("Card found.. \n");
  
  for (byte i = 0; i < bufferSize; i++) 
  {    
    cardID += String(buffer[i] < 0x10 ? "0" : "");
    cardID += String(buffer[i], HEX);
    cardID.toUpperCase();
  } 
  Serial.print(cardID);
  if ( cardID != ""){
//    writeData(cardID, namaServer, namaServer1);
    connWeb(cardID, serverName);
  }
}

void writeData(String cardID, String namaServer, String namaServer1) {
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = namaServer + cardID + namaServer1;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
  
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
        
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}


void connWeb(String cardID, const char* serverName) {
  delay(5000);

  Serial.println();
  Serial.print("connecting to ");
  Serial.println(serverName);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpsPort = 443; // 80 is for HTTP / 443 is for HTTPS!
  
  client.setInsecure(); // this is the magical line that makes everything work
  
  if (!client.connect(serverName, httpsPort)) { //works!
    Serial.println("connection failed");
    return;
  } else {
    Serial.println("connection success");
  }

  // We now create a URI for the request
  String url = "https://card.conect.id/api/user?id_card=";
  url += cardID;
  url += "&apikey=8105e86f-b71e-4a45-9708-5f1295b6226";


  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + serverName + "\r\n" + 
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  //ngga kebaca karena client.available() == 0
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
    /*processScheduleData(line);
    Serial.print("Nama : "); Serial.println(namaOnSchedule); 
    lcd.setCursor(0,0);lcd.print(namaOnSchedule); //Ex: 01:00:00,20:00:00 
    Serial.print("Expired Data : "); Serial.println(expiredOnSchedule);
    lcd.setCursor(0,1);lcd.print(expiredOnSchedule);
    delay(3000);
    lcd.clear();//Ex: 01:30:00,20:30:00*/
 }
  Serial.println();
  Serial.println("closing connection");
}

/*
void conbackWeb() {
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
              
      sensorReadings = httpGETRequest(serverName);
      Serial.println(sensorReadings);
      
//      //Contoh untuk http://digitaloxyfarming.com/api/output?api_key=HPHU5LAs7i4QIX1q
//          processStatusData(payload);
//          Serial.println(statusMessage);
//          Serial.println(statusCode);
//          Serial.println(statusNama);
//          Serial.println(statusExpired); 
//
//          //Contoh untuk http://digitaloxyfarming.com/api/schedule/today?api_key=HPHU5LAs7i4QIX1q
//          processScheduleData(payload);
//          Serial.print("Nama : "); Serial.println(namaSchedule); //Ex: 01:00:00,20:00:00 
//          Serial.print("Expired Data : "); Serial.println(expiredSchedule); //Ex: 01:30:00,20:30:00
////          Serial.print("F On: "); Serial.println(fertilizerOnSchedule); //Ex: 01:00:00,01:01:00,01:02:00
////          Serial.print("F Off: "); Serial.println(fertilizerOffSchedule); //Ex: 01:01:00,01:02:00,01:03:00
      /*JSONVar myObject = JSON.parse(sensorReadings);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
    
      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();
    
      for (int i = 0; i < keys.length(); i++) {
        JSONVar value = myObject[keys[i]];
        Serial.print(keys[i]);
        Serial.print(" = ");
        Serial.println(value);
        sensorReadingsArr[i] = double(value);
      }
      Serial.print("1 = ");
      Serial.println(sensorReadingsArr[0]);
      Serial.print("2 = ");
      Serial.println(sensorReadingsArr[1]);
      Serial.print("3 = ");
      Serial.println(sensorReadingsArr[2]);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
//  WiFiClientSecure client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();

    //Contoh untuk http://digitaloxyfarming.com/api/output?api_key=HPHU5LAs7i4QIX1q
//          processStatusData(payload);
//          Serial.println(statusMessage);
//          Serial.println(statusCode);
//          Serial.println(statusNama);
//          Serial.println(statusExpired); 

          //Contoh untuk http://digitaloxyfarming.com/api/schedule/today?api_key=HPHU5LAs7i4QIX1q
          processScheduleData(payload);
          Serial.print("Nama : "); Serial.println(namaOnSchedule); 
          lcd.setCursor(0,0);lcd.print(namaOnSchedule); //Ex: 01:00:00,20:00:00 
          Serial.print("Expired Data : "); Serial.println(expiredOnSchedule);
          lcd.setCursor(0,1);lcd.print(expiredOnSchedule);
          delay(3000);
          lcd.clear();//Ex: 01:30:00,20:30:00
//          Serial.print("F On: "); Serial.println(fertilizerOnSchedule); //Ex: 01:00:00,01:01:00,01:02:00
//          Serial.print("F Off: "); Serial.println(fertilizerOffSchedule); //Ex: 01:01:00,01:02:00,01:03:00
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
*/

//void connectWeb() {
//  // wait for WiFi connection
//  if ((WiFiMulti.run() == WL_CONNECTED)) {
//
//    WiFiClient client;
//
//    HTTPClient http;
//
//    Serial.print("[HTTP] begin...\n");
//    if (http.begin(client, "https://digitaloxyfarming.com/api/sensor/post?api_key=HPHU5LAs7i4QIX1q&sensor1=String(cardid)")) {  // HTTP
//
//
//      Serial.print("[HTTP] GET...\n");
//      // start connection and send HTTP header
//      int httpCode = http.GET();
//
//      // httpCode will be negative on error
//      if (httpCode > 0) {
//        // HTTP header has been send and Server response header has been handled
//        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
//
//        // file found at server
//        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//          String payload = http.getString();
//
//          Serial.println(payload);
//
//          //Contoh untuk http://digitaloxyfarming.com/api/output?api_key=HPHU5LAs7i4QIX1q
//          processStatusData(payload);
//          Serial.println(statusMessage);
//          Serial.println(statusPump); //Ex: 0 or 1
//          Serial.println(statusFertilizer); //Ex 0 or 1
//
//          //Contoh untuk http://digitaloxyfarming.com/api/schedule/today?api_key=HPHU5LAs7i4QIX1q
//          processScheduleData(payload);
//          Serial.print("P On: "); Serial.println(pumpOnSchedule); //Ex: 01:00:00,20:00:00 
//          Serial.print("P Off: "); Serial.println(pumpOffSchedule); //Ex: 01:30:00,20:30:00
//          Serial.print("F On: "); Serial.println(fertilizerOnSchedule); //Ex: 01:00:00,01:01:00,01:02:00
//          Serial.print("F Off: "); Serial.println(fertilizerOffSchedule); //Ex: 01:01:00,01:02:00,01:03:00
//        }
//      } else {
//        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//      }
//
//      http.end();
//    } else {
//      Serial.printf("[HTTP} Unable to connect\n");
//    }
//  }
//
//  delay(10000);
//}
