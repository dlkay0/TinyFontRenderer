#include "TinyFont.h"

TinyFont::TinyFont(File fontFile) {
  byteBuffer = (char*)malloc(8);

  if(readUInt32(fontFile) == 0xf995b0a8) {
    fontFile.seek(0);
    TinyResourceFileHeader fileHeader = loadTinyResourceFileHeaderFromFile(fontFile);
    TinyResourceHeader resourceHeader = loadTinyResourceHeaderFromFile(fontFile);
  }

  description = loadFontDescriptionFromFile(fontFile);
  bitmapDesc = loadBitmapDescriptionFromFile(fontFile);
  
  loadRangesFromFile(fontFile, description.ranges);
  loadCharactersFromFile(fontFile, description.characters);
  
  uint32_t bitmapLengthInBytes = ((((bitmapDesc.width * bitmapDesc.bitsPerPixel) + 31) / 32) * bitmapDesc.height) * 4;
  pixels = (uint32_t*)malloc(bitmapLengthInBytes);
  fontFile.readBytes((char*)pixels, bitmapLengthInBytes);

  //TODO Add Ex
  if((description.flags & 0x0008) > 0) {

    descriptionEx = (FontDescriptionEx*)malloc(sizeof(FontDescriptionEx));
    fontFile.readBytes((char*)descriptionEx, sizeof(FontDescriptionEx));
    
    rangesEx = (FontCharacterRangeEx*)malloc(sizeof(FontCharacterRangeEx) * description.ranges);
    fontFile.readBytes((char*)rangesEx, sizeof(FontCharacterRangeEx) * description.ranges);

    charactersEx = (FontCharacterEx*)malloc(sizeof(FontCharacterEx) * description.characters);
    fontFile.readBytes((char*)charactersEx, sizeof(FontCharacterEx) * description.characters);

    antiAliasingData = (uint8_t*)malloc(descriptionEx->antiAliassize);
    fontFile.readBytes((char*)antiAliasingData, descriptionEx->antiAliassize);
  }
  else {
    antiAliasingData = NULL;
    descriptionEx = NULL;
    rangesEx = NULL;
    charactersEx = NULL;
  }

  defaultCharacter.isValid = false;
  getCharacterData(TinyFont::UnicodeReplacementCharacter, defaultCharacter);

  free(byteBuffer);
}

TinyFont::~TinyFont() {
  if(antiAliasingData) {
    free(antiAliasingData);
  }
  
  if(charactersEx) {
    free(charactersEx);
  }
  
  if(rangesEx) {
    free(rangesEx);
  }
  
  if(descriptionEx) {
    free(descriptionEx);
  }
  
  if(pixels) {
    free(pixels);
  }
  
  if(characters) {
    free(characters);
  }

  if(ranges) {
    free(ranges);
  }
}


void TinyFont::getCharacterData(char c, FontCharacterInfo& fontCharacterInfo) {
  int32_t rangeIndex = -1;
  for(int32_t r = 0; r < description.ranges; r++) {
    if(ranges[r].firstChar <= c && ranges[r].lastChar >= c) {
      rangeIndex = r;
      break;
    }
  }

  if(rangeIndex < 0) {
    fontCharacterInfo = defaultCharacter;
    return;
  }

  FontCharacterRange *range = &ranges[rangeIndex];
  int32_t charIndex = range->indexOfFirstFontCharacter + (c - range->firstChar);
  if(charIndex > description.characters) {
   fontCharacterInfo = defaultCharacter;
    return;
  }

  FontCharacter* character = &characters[charIndex];
  fontCharacterInfo.isValid = true;
  fontCharacterInfo.marginLeft = character->marginLeft;
  fontCharacterInfo.marginRight = character->marginRight;
  fontCharacterInfo.offset = range->rangeoffset + character->offset;
  
  if(c == range->lastChar) {
    fontCharacterInfo.innerWidth = (uint16_t)((range[1].rangeoffset) + character[1].offset) - fontCharacterInfo.offset;
  }
  else {
    fontCharacterInfo.innerWidth = (uint16_t)character[1].offset - (uint16_t)character[0].offset;
  }

  fontCharacterInfo.width = (uint16_t)(fontCharacterInfo.marginLeft + fontCharacterInfo.marginRight + fontCharacterInfo.innerWidth);
  fontCharacterInfo.height = (uint16_t)(bitmapDesc.height);

  fontCharacterInfo.antiAliasCount = (uint8_t)((uint16_t)(description.flags & 0xF0) >> 4);

  if (fontCharacterInfo.antiAliasCount > 1 && charactersEx[charIndex].offsetAntiAlias != 0xFFFF)
  {
    uint32_t aaoffset = rangesEx[rangeIndex].offsetAntiAlias + charactersEx[charIndex].offsetAntiAlias;
    fontCharacterInfo.antiAlias = antiAliasingData + aaoffset;
  }
  else
  {
    fontCharacterInfo.antiAlias = 0;
    fontCharacterInfo.antiAliasCount = 1;
  }
}
  
  
uint8_t TinyFont::readUInt8(File fontFile) {
  fontFile.readBytes(byteBuffer, 1);

  return (uint8_t)(byteBuffer[0]);
}

int8_t TinyFont::readInt8(File fontFile) {
  fontFile.readBytes(byteBuffer, 1);

  return (int8_t)(byteBuffer[0]);
}

uint16_t TinyFont::readUInt16(File fontFile) {
  fontFile.readBytes(byteBuffer, 2);

  return (uint16_t)(byteBuffer[0] | (byteBuffer[1] << 8));
}

uint32_t TinyFont::readUInt32(File fontFile) {
  fontFile.readBytes(byteBuffer, 4);

  return (uint32_t)(byteBuffer[0] | (byteBuffer[1] << 8) | (byteBuffer[2] << 16) | (byteBuffer[3] << 24) );
}

TinyResourceFileHeader TinyFont::loadTinyResourceFileHeaderFromFile(File fontFile) {
  TinyResourceFileHeader fileHeader;

  fileHeader.magicNumber = readUInt32(fontFile);
  fileHeader.version = readUInt32(fontFile);
  fileHeader.sizeOfHeader = readUInt32(fontFile);
  fileHeader.sizeOfResourceHeader = readUInt32(fontFile);
  fileHeader.numberOfResources = readUInt32(fontFile);

  return fileHeader;
}

TinyResourceHeader TinyFont::loadTinyResourceHeaderFromFile(File fontFile) {
  TinyResourceHeader resourceHeader;

  resourceHeader.id = readUInt16(fontFile);
  resourceHeader.kind = readUInt8(fontFile);
  resourceHeader.flags = readUInt8(fontFile);
  resourceHeader.size = readUInt32(fontFile);

  return resourceHeader;
}


FontMetrics TinyFont::loadFontMetricsFromFile(File fontFile) {
  FontMetrics metrics;

  metrics.height = readUInt16(fontFile);
  metrics.offset = readUInt16(fontFile);
  metrics.ascent = readUInt16(fontFile);
  metrics.descent = readUInt16(fontFile);
  metrics.internalLeading = readUInt16(fontFile);
  metrics.externalLeading = readUInt16(fontFile);
  metrics.averageCharWidth = readUInt16(fontFile);
  metrics.maximumCharWidth = readUInt16(fontFile);

  return metrics;
}

FontDescription TinyFont::loadFontDescriptionFromFile(File fontFile) {
  FontDescription desc;
 
  desc.metrics = loadFontMetricsFromFile(fontFile);
  desc.ranges = readUInt16(fontFile);
  desc.characters = readUInt16(fontFile);
  desc.flags = readUInt16(fontFile);
  desc.appendices = readUInt16(fontFile);

  return desc;
}

BitmapDescription TinyFont::loadBitmapDescriptionFromFile(File fontFile) {
  BitmapDescription desc;
 
  desc.width = readUInt32(fontFile);
  desc.height = readUInt32(fontFile);
  desc.flags = readUInt16(fontFile);
  desc.bitsPerPixel = readUInt8(fontFile);
  desc.type = readUInt8(fontFile);

  return desc;
}

void TinyFont::loadRangesFromFile(File fontFile, int count) {
  
  ranges = (FontCharacterRange*) malloc((count + 1) * sizeof(FontCharacterRange));
  fontFile.readBytes((char*)ranges, (count + 1) * sizeof(FontCharacterRange));
}

void TinyFont::loadCharactersFromFile(File fontFile, int count) {
  
  characters = (FontCharacter*) malloc((count + 1) * sizeof(FontCharacter));
  fontFile.readBytes((char*)characters, (count + 1) * sizeof(FontCharacter));
}

uint16_t TinyFont::getHeight() {
  return description.metrics.height;
}

uint16_t TinyFont::getOffset() {
  return description.metrics.offset;
}

uint16_t TinyFont::getAverageCharacterWidth() {
  return description.metrics.averageCharWidth;
}

uint16_t TinyFont::getMaximumCharacterWidth() {
  return description.metrics.maximumCharWidth;
}

char* TinyFont::getDefinedCharacters() {
  uint16_t numChars = 0;

  for(uint16_t r = 0; r <= description.ranges; r++)
    numChars += ranges[r].lastChar - ranges[r].firstChar + 1;

  char* chars = (char*)malloc(numChars);
  uint16_t i = 0;
  for(uint16_t r = 0; r <= description.ranges; r++)
    for(uint16_t c = ranges[r].firstChar; c <= ranges[r].lastChar; c++)
      chars[i++] = c;

  return chars;
}

uint16_t TinyFont::getTopExternalMargin() {
  return description.metrics.externalLeading;
}

uint16_t TinyFont::getNumberOfDefinedCharacters() {
  return description.characters;
}
