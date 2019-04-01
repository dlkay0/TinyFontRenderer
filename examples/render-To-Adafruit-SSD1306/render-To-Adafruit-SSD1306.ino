/**************************************************************************
 This is an example for rendering TinyFonts on an SSD1306 OLED display

 Requires: Adafruit_SSD1306 library, Adafruit_GFX library, and SPIFFS
 
 Install fonts to SPIFFS
 **************************************************************************/
#include "TinyFontRenderer.h"
#include "SPIFFS.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306* display;
int i = 0;
TinyFontRenderer *tinyFontRenderer;

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();

  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
  
  if(!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {// Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void loop() {
  display->clearDisplay();

  if(tinyFontRenderer) delete tinyFontRenderer;
  switch(i) {
    case 0:
      tinyFontRenderer = new TinyFontRenderer("/ComicSansAnti.tinyfnt");
      tinyFontRenderer->drawString(0, 0, "ComicSansAnti", oledCallback, false);
      break;
    
    case 1:
      tinyFontRenderer = new TinyFontRenderer("/ComicSans.tinyfnt");
      tinyFontRenderer->drawString(0, 0, "ComicSans", oledCallback, false);
      break;
    
    case 2:
      tinyFontRenderer = new TinyFontRenderer("/VideophreakAnti.tinyfnt");
      tinyFontRenderer->drawString(0, 0, "VideophreakAnti", oledCallback, false);
      break;
    
    case 3:
      tinyFontRenderer = new TinyFontRenderer("/Videophreak.tinyfnt");
      tinyFontRenderer->drawString(0, 0, "Videophreak", oledCallback, false);
      break;
    
    case 4:
      tinyFontRenderer = new TinyFontRenderer("/Arial5x16.tinyfnt");
      tinyFontRenderer->drawString(0, 0, "Arial5x16", oledCallback, false);
      break;
    
    case 5:
      tinyFontRenderer = new TinyFontRenderer("/Bahnschrift12x32.tinyfnt");
      tinyFontRenderer->drawString(0, 0, "Bahnschrift12x32", oledCallback, false);
      break;
  }
  i = (i + 1) % 6;
  display->display();
  delay(1000);

  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, "Hello World", oledCallback, false);
  display->display();

  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, "How", oledCallback, false);
  display->display(); delay(10);
  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, "fast", oledCallback, false);
  display->display(); delay(10);
  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, "can", oledCallback, false);
  display->display(); delay(10);
  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, "you", oledCallback, false);
  display->display(); delay(10);
  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, "read", oledCallback, false);
  display->display(); delay(10);
  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, "display?", oledCallback, false);
  display->display(); delay(10);
  display->clearDisplay();
  display->display();
  delay(2000);
  
  uint16_t count = tinyFontRenderer->getNumberOfDefinedCharacters();
  char* chars = tinyFontRenderer->getDefinedCharacters();

  Serial.print(count);Serial.println(" characters in font.");
  uint16_t maxCharsPerLine = SCREEN_WIDTH / tinyFontRenderer->getMaximumCharacterWidth();
  for(uint16_t i=0; i < count; i++) {
    char c = chars[i];

    if(i % maxCharsPerLine == 0) display->clearDisplay();

    Serial.print(c);
    tinyFontRenderer->drawCharacter(tinyFontRenderer->getMaximumCharacterWidth() * (i % maxCharsPerLine), 0, c, oledCallback, false);
    display->display();
    
    delay(5);
  }

  delay(2000);
  Serial.println();

  uint16_t i = 0;
  String line = "";
  do {
    String test = line + chars[i];
    if(tinyFontRenderer->getStringWidth(test) > SCREEN_WIDTH) {
      display->clearDisplay();
      tinyFontRenderer->drawString(0, 0, line, oledCallback, false);
      display->display();
      line = chars[i];
      delay(250);
    }
    else {
      line = test;
    }
  } while(++i < count);

  display->clearDisplay();
  tinyFontRenderer->drawString(0, 0, line, oledCallback, false);
  display->display();
  delay(3000);
  

  free(chars);
}

///////////////////////////////////////////////////////////
void oledCallback(const int x, const int y, const uint16_t opacity) {
  if(x > SCREEN_WIDTH - 1 || y > SCREEN_HEIGHT - 1) return;
  display->drawPixel(x, y, opacity == TinyFontRenderer::OpacityOpaque ? WHITE : BLACK);
  //Serial.print("(");Serial.print(x);Serial.print(",");Serial.print(y);Serial.print("-");Serial.println(opacity);
  //display->display();
}
