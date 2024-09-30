#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define RGBLED_PIN 48 // Pin where NeoPixels are connected
#define MAX 32
Adafruit_NeoPixel strip(1, RGBLED_PIN, NEO_GRB + NEO_KHZ400);
int j;
char databuff[200];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(4096);

  Serial.println("\r\n#Starting...");
  Serial.printf("#PSRAM Free = %lu/%lu kBytes\r\n",ESP.getFreePsram()/1024,ESP.getPsramSize()/1024);

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP

  while(1) {
    strip.setPixelColor(0,strip.Color(255,255,255));
    Serial.println("test 1");
    delay(1000);
    strip.setPixelColor(0,strip.Color(255,0,0));
    Serial.println("test 2");
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  j = (j%8)==0?j+1:j;
  sprintf(databuff,"#Hello %d, Num LED=%d, color=%d (%c,%c,%c)",j,strip.numPixels(),j%8,(((j%8)&(1<<0))==0?'-':'R'), (((j%8)&(1<<1))==0?'-':'G'), (((j%8)&(1<<2))==0?'-':'B'));
  Serial.println(databuff);
  //colorWipe(strip.Color((((i%8)&(1<<0))==0?0:1)*128, (((i%8)&(1<<1))==0?0:1)*128, (((i%8)&(1<<2))==0?0:1)*128), 0);  
  for(int i = 0; i<(j==2?20:MAX); ++i) {
    strip.setPixelColor(0,strip.Color((((j%8)&(1<<0))==0?0:1)*i, (((j%8)&(1<<1))==0?0:1)*i, (((j%8)&(1<<2))==0?0:1)*i));
    strip.show();
    delay((j==2?80:40));
    if(digitalRead(0)==0) break;
  }
  for(int i = MAX-1; i>=0; --i) {
    strip.setPixelColor(0,strip.Color((((j%8)&(1<<0))==0?0:1)*i, (((j%8)&(1<<1))==0?0:1)*i, (((j%8)&(1<<2))==0?0:1)*i));
    strip.show();
    delay((j==2?80:40));
    if(digitalRead(0)==0) break;
  }
  //delay(500);
  ++j;

}
