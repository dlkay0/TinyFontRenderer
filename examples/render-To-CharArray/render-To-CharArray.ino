/*****************************************************************************
 This is an example for rendering TinyFonts to a CharArray to send over Serial

 Requires: SPIFFS
 
 Install fonts to SPIFFS
 *****************************************************************************/
#include "TinyFontRenderer.h"
#include "SPIFFS.h"
#define ARRAY_WIDTH 200  // character array width in pixels
#define ARRAY_HEIGHT 32  // character array height in pixels

int i = 0;
TinyFontRenderer *tinyFontRenderer;
char charArray[ARRAY_WIDTH * ARRAY_HEIGHT];


void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
}

void loop() {
  if(tinyFontRenderer) delete tinyFontRenderer;
  switch(i) {
    case 0:
      tinyFontRenderer = new TinyFontRenderer("/ComicSansAnti.tinyfnt");
      clearCharArray();
      tinyFontRenderer->drawString(0, tinyFontRenderer->getOffset(), "ComicSansAnti", charArrayCallback, false);
      drawCharArray();
      break;
    
    case 1:
      tinyFontRenderer = new TinyFontRenderer("/ComicSans.tinyfnt");
      clearCharArray();
      tinyFontRenderer->drawString(0, tinyFontRenderer->getOffset(), "ComicSans", charArrayCallback, false);
      drawCharArray();
      break;
    
    case 2:
      tinyFontRenderer = new TinyFontRenderer("/VideophreakAnti.tinyfnt");
      clearCharArray();
      tinyFontRenderer->drawString(0, tinyFontRenderer->getOffset(), "VideophreakAnti", charArrayCallback, false);
      drawCharArray();
      break;
    
    case 3:
      tinyFontRenderer = new TinyFontRenderer("/Videophreak.tinyfnt");
      clearCharArray();
      tinyFontRenderer->drawString(0, tinyFontRenderer->getOffset(), "Videophreak", charArrayCallback, false);
      drawCharArray();
      break;
    
    case 4:
      tinyFontRenderer = new TinyFontRenderer("/Arial5x16.tinyfnt");
      clearCharArray();
      tinyFontRenderer->drawString(0, tinyFontRenderer->getOffset(), "Arial5x16", charArrayCallback, false);
      break;
    
    case 5:
      tinyFontRenderer = new TinyFontRenderer("/Bahnschrift12x32.tinyfnt");
      clearCharArray();
      tinyFontRenderer->drawString(0, tinyFontRenderer->getOffset(), "Bahnschrift12x32", charArrayCallback, false);
      break;
  }
  i = (i + 1) % 6;
  delay(100);
 
  uint16_t count = tinyFontRenderer->getNumberOfDefinedCharacters();
  char* chars = tinyFontRenderer->getDefinedCharacters();

  Serial.print(count);Serial.println(" characters in font.");
  clearCharArray();

  uint16_t i = 0;
  String line = "";
  do {
    String test = line + chars[i];
    if(tinyFontRenderer->getStringWidth(test) > ARRAY_WIDTH) {
      clearCharArray();
	  tinyFontRenderer->drawString(0, 0, line, charArrayCallback, false);
      line = chars[i];
      drawCharArray();
      delay(250);
    }
    else {
      line = test;
    }
  } while(++i < count);

  clearCharArray();
  tinyFontRenderer->drawString(0, 0, line, charArrayCallback, false);
  drawCharArray();
  delay(10000);
  

  free(chars);
}

///////////////////////////////////////////////////////////
void charArrayCallback(const int x, const int y, const uint16_t opacity) {
  if(x > ARRAY_WIDTH - 1 || y > ARRAY_HEIGHT - 1) return;

  char c = '#';

  if(opacity == TinyFontRenderer::OpacityOpaque) c = '#';
  else if(opacity >= 192) c = '+';
  else if(opacity >= 128) c = '~';
  else if(opacity >= 64) c = '-';
  else c = ' ';

  charArray[x + y * ARRAY_WIDTH] = c;   
}

void clearCharArray() {
  for(int i = 0; i < ARRAY_WIDTH * ARRAY_HEIGHT; i++) {
    charArray[i] = ' ';
  }
}

void drawCharArray() {
  Serial.print('+');
  for(int i = 0; i < ARRAY_WIDTH; i++) {
    Serial.print('-');
  }
  Serial.println('+');
  
  for(int y = 0; y < ARRAY_HEIGHT; y++) {
    for(int x = 0; x < ARRAY_WIDTH; x++) {
      if(x == 0) Serial.print('|');
      Serial.print(charArray[x + y * ARRAY_WIDTH]);
    }
    Serial.println('|');
  }

  Serial.print('+');
  for(int i = 0; i < ARRAY_WIDTH; i++) {
    Serial.print('-');
  }
  Serial.println('+');
}
