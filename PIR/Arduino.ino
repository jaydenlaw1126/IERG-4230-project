/*
  IERG4230 IoT Testing Program
  Passive Infra-Red (PIR) motion detector with Buzzer
  Matter Needing attention:
  On Boot/Reset/wakeup,
  GPIO15(D8) keep LOW and GPIO2(D4)keep HIGH
  GPIO0(D3) HIGH = Run mode / LOW = Flash mode
  On Board LEDs (active LOW):
    GPIO16(D0)
    GPIO2(D4)
  Connections:
  PIR          ESP8266-12E
  ---------    ---------
  VCC          VIN
  OUT          GPIO-12 (D6)
  GND          GND

  Buzzer       ESP8266-12E
  ---------    ---------
  VCC          3.3V
  I/O          GPIO-14 (D5)
  GND          GND
*/
#include "Buzzer.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h> //WI-Fi
#include "ThingSpeak.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// PIR motion detector
int  inputPin  = 12;  // PIR OUT connected to GPIO 12(D6)
int  led       = 16;  // On Board LED GPIO 16(D0)
bool detect    = false;
Buzzer buzzer(14); // (Buzzer pin,LED pin)

//Wifi definetion
const char* ssid     = "Law-home";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "iamlaw666";     // The password of the Wi-Fi network

//Thingspeak
char* writeAPIkey = "ZPPMZXPNCCA9OJP9";
char* readAPIkey = "N0GIKMYCYPK5FPUQ";
const long myID = 1264557;
int WriteField = 1;
WiFiClient client;

float sendData(long Channel, int Field, float data, char* WriteAPIkey){
  float success = ThingSpeak.writeField(myID, Field, data, writeAPIkey);
  return success;
  }

void setup()
{
  Serial.begin(115200);

  //Wifi part
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  //OLED and PIR
  Serial.println("ESP8266-12E/F OLED auto sensor light program");
  Serial.println("Build-in LED at GPIO-16(D0)");
  Serial.println("Buzzer pin at GPIO-14(D5)");
  Serial.println("PIR Detector Pin at GPIO-12(D6)");
  pinMode (inputPin, INPUT);
  pinMode (led, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
//  Serial.println("");
  delay(2000);
  
  //Initialize ThingSpeak
  ThingSpeak.begin(client);
  display.clearDisplay();
}

void loop()
{
  detect = digitalRead(inputPin); // read PIR OUT pin (Movement detected = HIGH)
  display.clearDisplay();

  if (detect == true) {
    digitalWrite(led, LOW);
    Serial.println("Movement detected");
    Serial.println("Buzzer play");
    buzzer.begin(0);
    /* Method - It creates a "normal distortion" effect on the Buzzer */
    //    buzzer.distortion(NOTE_C3, NOTE_C5);
    //    buzzer.distortion(NOTE_C5, NOTE_C3);

    /* Method - It makes the Buzzer sound */
    //OLED printing
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.print("Movement detected");
    //OLED printing as light off
    display.setCursor(0, 20);
    display.print("Light on");
    display.display();

    sendData(myID, WriteField, 1, writeAPIkey);
    //buzzer.sound(NOTE_A3, 100); // (Note, duration)
    buzzer.end(100);
    delay(5000);
  }
  else {
    digitalWrite(led, HIGH);
    Serial.println("No movement detected");

    //OLED printing
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.print("NO movement detected");
    //OLED printing as light off
    display.setCursor(0, 20);
    display.print("Light off");
    
    display.display();

    sendData(myID, WriteField, 0, writeAPIkey);
  }
  
  delay(100);
}
