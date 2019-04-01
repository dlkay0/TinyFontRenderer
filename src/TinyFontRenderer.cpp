#include "TinyFontRenderer.h"

TinyFontRenderer::TinyFontRenderer(String fontFilename) {
  File fontFile = SPIFFS.open(fontFilename, "r");
  if(fontFile) {
    font = new TinyFont(fontFile);
    fontFile.close();
  }
  else {
    for(;;) {
      Serial.println("Unable to open '" + fontFilename + "' on SPIFFS");
      delay(666);
    }
    
  }

  Serial.println("Font Details:");
  Serial.println("-------------");
  Serial.print("height          : ");Serial.println(font->description.metrics.height);
  Serial.print("Avg Width       : ");Serial.println(font->description.metrics.averageCharWidth);
  Serial.print("Max Width       : ");Serial.println(font->description.metrics.maximumCharWidth);
  Serial.print("offset          : ");Serial.println(font->description.metrics.offset);
  Serial.print("ascent          : ");Serial.println(font->description.metrics.ascent);
  Serial.print("descent         : ");Serial.println(font->description.metrics.descent);
  Serial.print("External Leading: ");Serial.println(font->description.metrics.externalLeading);
}

TinyFontRenderer::TinyFontRenderer(File fontFile) {
  font = (TinyFont*)malloc(sizeof(TinyFont));
  *font = TinyFont(fontFile);

  Serial.println("Font Details:");
  Serial.println("-------------");
  Serial.print("height          : ");Serial.println(font->description.metrics.height);
  Serial.print("Avg Width       : ");Serial.println(font->description.metrics.averageCharWidth);
  Serial.print("Max Width       : ");Serial.println(font->description.metrics.maximumCharWidth);
  Serial.print("offset          : ");Serial.println(font->description.metrics.offset);
  Serial.print("ascent          : ");Serial.println(font->description.metrics.ascent);
  Serial.print("descent         : ");Serial.println(font->description.metrics.descent);
  Serial.print("External Leading: ");Serial.println(font->description.metrics.externalLeading);
}

TinyFontRenderer::~TinyFontRenderer() {
  if(font) {
    delete font;
  }
}

void TinyFontRenderer::drawCharacter(uint32_t x, uint32_t y, char c, DrawPixelCallback drawPixelCallback, bool drawBackground) {
  FontCharacterInfo fci;

  font->getCharacterData(c, fci);

  if(!fci.isValid) {
    return;
  }

  drawCharacter(x, y, fci, drawPixelCallback, drawBackground);
}

uint8_t TinyFontRenderer::getCharacterWidth(char c) {
  FontCharacterInfo fci;

  font->getCharacterData(c, fci);

  return fci.isValid ? fci.innerWidth : 0;
}

uint16_t TinyFontRenderer::getHeight() {
  return font->getHeight();
}

uint16_t TinyFontRenderer::getOffset() {
  return font->getOffset();
}

uint16_t TinyFontRenderer::getAverageCharacterWidth() {
  return font->getAverageCharacterWidth();
}

uint16_t TinyFontRenderer::getMaximumCharacterWidth() {
  return font->getMaximumCharacterWidth();
}

char* TinyFontRenderer::getDefinedCharacters() {
  return font->getDefinedCharacters();
}

uint16_t TinyFontRenderer::getTopExternalMargin() {
  return font->getTopExternalMargin();
}

uint16_t TinyFontRenderer::getNumberOfDefinedCharacters() {
  return font->getNumberOfDefinedCharacters();
}


void TinyFontRenderer::drawCharacter(uint32_t xoffset, uint32_t yoffset, FontCharacterInfo &fci, DrawPixelCallback drawPixelCallback, bool drawBackground) {
  uint32_t* pFirstWord = font->pixels + (fci.offset / 32);
  uint32_t bitmapWidthInWords = ((font->bitmapDesc.width * font->bitmapDesc.bitsPerPixel + 31) / 32);
  uint32_t* pCurWord = 0;
  uint32_t firstPixelMask = (uint32_t)(1 << (fci.offset % 32));
  uint32_t curPixelMask = 1;

  if(!fci.isValid) return;
//
  uint8_t* antiAlias;
  uint8_t antiAliaspixelsPerByte;
  uint8_t antiAliasShiftFirstPixel;
  uint8_t antiAliasShift;
  uint8_t antiAliasStep;
  uint32_t antiAliasMaskFirstPixel;
  uint32_t antiAliasMask;
  uint16_t opacity;
  uint32_t alpha;
  
  if(fci.antiAliasCount > 1) {
    antiAlias = fci.antiAlias;
    antiAliaspixelsPerByte = 8 / fci.antiAliasCount;
    antiAliasShiftFirstPixel = fci.antiAliasCount * (antiAliaspixelsPerByte - 1);
    antiAliasShift = antiAliasShiftFirstPixel;
    antiAliasStep = TinyFontRenderer::OpacityOpaque / (fci.antiAliasCount * fci.antiAliasCount);
    antiAliasMaskFirstPixel = ((1 << fci.antiAliasCount) - 1 ) << antiAliasShift;
    antiAliasMask = antiAliasMaskFirstPixel;
  }
  
  for(uint32_t y=0; y < fci.height - font->description.metrics.externalLeading; y++) {
    for(uint32_t x=0; x < fci.innerWidth; x++) {
      
      if(x == 0) {
        pCurWord = pFirstWord;
        curPixelMask = firstPixelMask;
        pCurWord += (y * bitmapWidthInWords);
      }

      if((*pCurWord & curPixelMask) > 0) {
        if(fci.antiAliasCount > 1) {
          alpha = ((*antiAlias & antiAliasMask) >> antiAliasShift) + 1;
          opacity = alpha * antiAliasStep;
    
          antiAliasMask >>= fci.antiAliasCount;
          antiAliasShift -= fci.antiAliasCount;
  
          if(antiAliasMask == 0) {
            antiAliasMask = antiAliasMaskFirstPixel;
            antiAlias++;
            antiAliasShift = antiAliasShiftFirstPixel;
          }
        }
        else {
          opacity = TinyFontRenderer::OpacityOpaque;
        }
        drawPixelCallback(x + xoffset, y + yoffset, opacity);
      }
      else {
        if(drawBackground) {
          drawPixelCallback(x + xoffset, y + yoffset, TinyFontRenderer::OpacityTransparent);
        }
      }

      curPixelMask <<= 1;

      if(curPixelMask == 0) {
        pCurWord += 1;
        curPixelMask = 0x1;
      }
    }
  }
}

void TinyFontRenderer::drawString(uint32_t x, uint32_t y, String text, DrawPixelCallback drawPixelCallback, bool drawBackground) {
  TinyFontRenderer::drawString(x, y, text, text.length(), drawPixelCallback, drawBackground);
}

void TinyFontRenderer::drawString(uint32_t x, uint32_t y, String text, uint16_t maxChars, DrawPixelCallback drawPixelCallback, bool drawBackground) {

  uint32_t curX = 0;
  uint16_t textLength = text.length();
  for(uint16_t i=0; i < textLength; i++) {
    if(maxChars-- <= 0) {
      return;
    }
    
    char c = text[i];

    FontCharacterInfo fci;
    font->getCharacterData(c, fci);

    if(!fci.isValid) {
      continue;
    }

    if(drawBackground) {
      for(uint32_t clearX=0; clearX < fci.marginLeft; clearX++)
        for(uint32_t clearY=0; clearY < fci.height; clearY++)
          drawPixelCallback(x + curX + clearX, y + clearY, 0.0);
    }

    if(fci.innerWidth > 0) {
      if(fci.marginLeft < 0 && curX >= -fci.marginLeft)
        curX += fci.marginLeft;

      TinyFontRenderer::drawCharacter(x + curX, y, fci, drawPixelCallback, drawBackground);
    }

    if(drawBackground && fci.marginRight > 0 && i < textLength) {
      for(uint32_t clearX=fci.width - fci.marginRight; clearX < fci.width; clearX++)
        for(uint32_t clearY=0; clearY < fci.height; clearY++)
          drawPixelCallback(x + curX + clearX, y + clearY, 0.0);
    }

    if(curX == 0) {
      curX = fci.innerWidth + fci.marginRight;
    }
    else {
      curX = (((curX + fci.marginLeft) < 0) ? curX : (curX + fci.marginLeft)) + fci.innerWidth + fci.marginRight;
    }
  }
}

uint32_t TinyFontRenderer::getStringWidth(String text) {
  uint32_t width = 0;
  int8_t lastMarginRight = 0;
  uint16_t textLength = text.length();
  FontCharacterInfo fci;
  
  for(uint16_t i=0; i < textLength; i++) {
    char c = text[i];
    font->getCharacterData(c, fci);

      if(width == 0) {
        width = fci.innerWidth + fci.marginRight;
      }
      else {
        width = ((width + fci.marginLeft < 0) ? width : (width + fci.marginLeft)) + fci.innerWidth + fci.marginRight;
      }

      lastMarginRight = fci.marginRight;
  }

  width -= lastMarginRight;

  return width;
}
