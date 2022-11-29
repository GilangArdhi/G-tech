

/* *********************************************************************************** *
 * Author : Elektro Project
 * Date   : 16 Mei 2022
 * Desc.  : using RFID Card Reader to scan UID Card 
 *          
 *          Microcontroller Board Setting Tools
 *          - Board       --> NodeMCU 1.0 (ESP-12E Module)
 *          - Upload Speed--> 921600
 *          - Port        --> COM__ ??? (listen list on your device)
 *          
 *          Pin configuration RFID-MFRC522
 *          - D0 (GPIO16) --> RST
 *          - D8 (GPIO15) --> SDA/SS
 *          - D5 (GPIO12) --> SCK
 *          - D6 (GPIO14) --> MISO
 *          - D7 (GPIO13) --> MOSI
 *          
 *    Support we on YouTube channel: https://www.youtube.com/c/ElektroProject
 * 
 * *********************************************************************************** */
#include <SPI.h>
#include <MFRC522.h> 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define SS_PIN        D8     
#define RST_PIN       D4      

byte uid[4];
String cardID;

MFRC522 rfid(SS_PIN, RST_PIN); 

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  SPI.begin();

  // Initial MFRC RFID Reader Module 
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
  lcd.clear();

}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Nama Gym");
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    for (byte i=0; i<4; i++) {
      uid[i] = rfid.uid.uidByte[i];
    }
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
     
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void printHex(byte *buffer, byte bufferSize) {
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
    lcd.setCursor(0, 0);
    lcd.print("Selamat Datang");
    lcd.setCursor(0, 1);
    lcd.print(cardID);
    delay(3000);
    lcd.clear();    
  }
}
