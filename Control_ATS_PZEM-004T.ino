#include "IO_Mapping.h"
#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27, 16, 2);

//BlynkTimer timer;
char auth[] = BLYNK_AUTH_TOKEN;

bool statusListrikPadam = 0; // tidak padam = 0 || padam = 1
uint8_t state = 0;
char fasaPadam;

/* *************** Setting SSID dan Password WiFi ************ */
const char* ssid = "Infinix NOTE 12";
const char* password = "astungkara1612";
/* *************** Akhir Setting WiFi ************ */

/* *************** PZEM-004T Address *************** */
PZEM004Tv30 pzem1(&Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x11);
PZEM004Tv30 pzem2(&Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x12);
/* *************** End of PZEM-004T  *************** */

/* -------------- End of Initialitation Variable  -------------- */

void setup() {
  Serial.begin(115200);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Welcome.....");
  delay(1000);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("LCD Ready!");
  delay(1000);
  
  // Initialize the relay pins as outputs
  pinMode(RELAY_PLN, OUTPUT);
  pinMode(RELAY_GENSET, OUTPUT);
  pinMode(RELAY_AMF, OUTPUT);
  
  // Initially turn off Relay Genset and AMF
  digitalWrite(RELAY_PLN, HIGH);
  digitalWrite(RELAY_GENSET, HIGH);
  digitalWrite(RELAY_AMF, HIGH);

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Blynk.begin(auth, ssid, password);

}

void loop() {
   /*
        Alur program:
      1. baca tegangan (ketiga fasa)
      2. jika tegangan tidak memenuhi kondis (mungkin lebih dari 3 kondisi), kirim SMS dengan pesan sesuai kendala
      3. baca kondisi pintu
      4. jika terbuka kirim pesan
      5. ulang dari awal
  */

  baca_PZEM004T();
  Blynk.run();

}

void baca_PZEM004T(){
 // Reading data from PZEM-004T
  float V_PLN = pzem1.voltage();
  float I_PLN = pzem1.current();

  float V_GENSET = pzem2.voltage();
  float I_GENSET = pzem2.current();
  
  // Check if the data is valid
  if(isnan(V_PLN)){
    V_PLN = 0;
  }
  if (isnan(V_GENSET)) {
    V_GENSET = 0;
  } 

  if (isnan(I_PLN)) {
    I_PLN = 0;
  }
  if (isnan(I_GENSET)) {
    I_GENSET = 0;
  }

  //Chacking the voltage situatuion
  if (V_PLN != 0 && statusListrikPadam == 0) {
    state = 2;
  }
  else if (V_PLN == 0 && V_GENSET >= 220){
    state = 1;
  }
  else if (V_PLN == 0 && V_GENSET == 0){
    state = 5;
  }
  if (V_PLN != 0 && statusListrikPadam == 1) {
    state = 4;
  }

  switch (state)
  {
    case 1:
    {
      statusListrikPadam = 1; // tandai listrik masih padam
      digitalWrite(RELAY_PLN, HIGH);
      digitalWrite(RELAY_AMF, LOW);
      delay(3000);
      digitalWrite(RELAY_GENSET, LOW);
      //digitalWrite(RELAY_AMF, HIGH);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PLN: OFF");
      Blynk.virtualWrite(V4, "OFF");
      lcd.setCursor(0, 1);
      lcd.print("GENSET: ON");
      delay(1000);
      
      // Display LCD untuk Tegangan
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("V_PLN: ");
      lcd.print(V_PLN, 1);
      lcd.print(" V");
      lcd.setCursor(0, 1);
      lcd.print("V_GENSET: ");
      lcd.print(V_GENSET, 1);
      lcd.print(" V");
      delay(1000);
      Blynk.virtualWrite(V0, V_PLN);
      Blynk.virtualWrite(V1, V_GENSET);

      // Display LCD untuk Arus
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("I_PLN: ");
      lcd.print(I_PLN);
      lcd.print("A");
      lcd.setCursor(0, 1);
      lcd.print("I_GENSET: ");
      lcd.print(I_GENSET);
      lcd.print("A");
      delay(1000);
      Blynk.virtualWrite(V2, I_PLN);
      Blynk.virtualWrite(V3, I_GENSET);
      state = 3;
    }
    break;

    case 2:
    {
      digitalWrite(RELAY_GENSET, HIGH);
      digitalWrite(RELAY_AMF, HIGH);
      delay(1000);
      digitalWrite(RELAY_PLN, LOW);
      
          
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PLN: ON");
      Blynk.virtualWrite(V4, "ON");
      lcd.setCursor(0, 1);
      lcd.print("GENSET: OFF");
      delay(1000);
      
      // Display LCD untuk Tegangan
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("V_PLN: ");
      lcd.print(V_PLN, 1);
      lcd.print(" V");
      lcd.setCursor(0, 1);
      lcd.print("V_GENSET: ");
      lcd.print(V_GENSET, 1);
      lcd.print(" V");
      delay(1000);
      Blynk.virtualWrite(V0, V_PLN);
      Blynk.virtualWrite(V1, V_GENSET);

      // Display LCD untuk Arus
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("I_PLN: ");
      lcd.print(I_PLN);
      lcd.print("A");
      lcd.setCursor(0, 1);
      lcd.print("I_GENSET: ");
      lcd.print(I_GENSET);
      lcd.print("A");
      delay(1000);
      Blynk.virtualWrite(V2, I_PLN);
      Blynk.virtualWrite(V3, I_GENSET);
      state = 4;
    } 
    break;

    case 3:
    {
      statusListrikPadam = 1; // tandai listrik masih padam
    }
    break;

    case 4:
    {
      statusListrikPadam = 0;   // listrik menyala
    }
    break;

    case 5:
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PLN: OFF");
      Blynk.virtualWrite(V4, "OFF");
      lcd.setCursor(0, 1);
      lcd.print("GENSET: OFF");
      delay(1000);
    }
    break;
    default:
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ada error pada ");
      lcd.setCursor(0, 1);
      lcd.print("state: ");
      lcd.print(state);
    }
    break;
  }
  setData();
}

// Set data as a 0 value
void setData(){
  float V_PLN = 0;
  float V_GENSET = 0;
  float I_PLN = 0;
  float I_GENSET = 0;
}
