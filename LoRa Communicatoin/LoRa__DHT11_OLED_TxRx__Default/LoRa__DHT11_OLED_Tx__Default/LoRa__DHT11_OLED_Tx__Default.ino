
#include <LoRa.h>
#include "LoRaBoards.h"
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN 13
#define DHTTYPE DHT11

#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64  
#define OLED_RESET -1  
#define SCREEN_ADDRESS 0x3C

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int counter = 0;

void setup() {
    Serial.begin(115200);
    dht.begin();
  
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {  
        Serial.println(F("SSD1306 allocation failed"));
        while(1);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println(F("LoRa DHT11 Tx"));
    display.display();
    delay(1500);

    setupBoards();
    delay(1500);

#ifdef RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    Serial.println("LoRa Sender with DHT11");
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(LORA_FREQ_CONFIG)) {
        Serial.println("Starting LoRa failed!");
        display.clearDisplay();
        display.setCursor(0, 10);
        display.println(F("LoRa Failed!"));
        display.display();
        while (1);
    }
}

void loop() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        display.clearDisplay();
        display.setCursor(0, 10);
        display.println(F("DHT Fail"));
        display.display();
        return;
    }

    Serial.print("Sending packet: ");
    Serial.print(counter);
    Serial.print(" | Temp: ");
    Serial.print(t);
    Serial.print(" °C, Humidity: ");
    Serial.println(h);

    /* DEFAULT SEND PACKET */

      /*

        LoRa.beginPacket();
        LoRa.print("Counter: ");
        LoRa.print(counter);
        LoRa.print(" | Temp: ");
        LoRa.print(t);
        LoRa.print(" °C, Humidity: ");
        LoRa.print(h);
        LoRa.println(" %");
        LoRa.endPacket();

      */

  /* LORA SERIAL PRINT ON OLED RX  */

    LoRa.beginPacket();
    display.setCursor(0, 20);
    LoRa.print("Counter: ");
    LoRa.println(counter);
    display.setCursor(0, 30);
    LoRa.print("Temp: ");
    LoRa.print(t);
    LoRa.println("°C");
    display.setCursor(0, 40);
    LoRa.print("Humidity: ");
    LoRa.print(h);
    LoRa.print(" %");
    LoRa.endPacket();

   /* DISPLAY ON OLED TX */

    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("Sending: ");
    display.print(counter);
    display.setCursor(0, 20);
    display.print("Temp: ");
    display.print(t);
    display.print(" C");
    display.setCursor(0, 30);
    display.print("Humidity: ");
    display.print(h);
    display.println(" %");
    display.display();

    counter++;
    delay(2000);
}

