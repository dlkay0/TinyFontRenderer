#include "SPIFFS.h"


//Tiny Resource File Header
typedef struct {
  uint32_t magicNumber;
  uint32_t version;
  uint32_t sizeOfHeader;
  uint32_t sizeOfResourceHeader;
  uint32_t numberOfResources;
} TinyResourceFileHeader;

//Tiny Resource Header
typedef struct {
  uint16_t id;
  uint8_t kind;
  uint8_t flags;
  uint32_t size;
} TinyResourceHeader;

//TinyFont Resource
typedef struct {
  uint16_t height;
  uint16_t offset;
  uint16_t ascent;
  uint16_t descent;
  uint16_t internalLeading;
  uint16_t externalLeading;
  uint16_t averageCharWidth;
  uint16_t maximumCharWidth;
} FontMetrics;

typedef struct {
  FontMetrics metrics;
  uint16_t ranges;
  uint16_t characters;
  uint8_t flags;
  uint16_t appendices;
} FontDescription;

typedef struct {
  uint32_t indexOfFirstFontCharacter;
  uint16_t firstChar;
  uint16_t lastChar;

  uint32_t rangeoffset;
} FontCharacterRange;

typedef struct {
  uint16_t offset;
  int8_t marginLeft;
  int8_t marginRight;
} FontCharacter;

typedef struct {
  bool isValid;
  int8_t marginLeft;
  int8_t marginRight;
  uint32_t offset;
  uint16_t innerWidth;
  uint16_t width;
  uint16_t height;
  uint8_t* antiAlias;
  uint8_t antiAliasCount;
} FontCharacterInfo;

typedef struct {
  uint32_t antiAliassize;
} FontDescriptionEx;

typedef struct {
  int32_t offsetAntiAlias;
} FontCharacterRangeEx;

typedef struct {
  uint16_t offsetAntiAlias;
} FontCharacterEx;

typedef struct {
  uint32_t width;
  uint32_t height;
  uint16_t flags;
  uint8_t bitsPerPixel;
  uint8_t type;
} BitmapDescription;

class TinyFont {
  public:
    TinyFont(File fontFile);
    ~TinyFont();

    static const char UnicodeReplacementCharacter = (char)0xFFFD;

    void getCharacterData(char c, FontCharacterInfo &fontCharacterInfo);

    uint16_t getHeight();
    uint16_t getOffset();
    uint16_t getAverageCharacterWidth();
    uint16_t getMaximumCharacterWidth();
    char* getDefinedCharacters();
    uint16_t getTopExternalMargin();
    uint16_t getNumberOfDefinedCharacters();
    FontDescription description;
    BitmapDescription bitmapDesc;
    uint32_t* pixels;
    uint8_t* antiAliasingData;

  private:
    char* byteBuffer;

    FontCharacterRange* ranges;
    FontCharacter* characters;
    FontCharacterInfo defaultCharacter;
    FontDescriptionEx* descriptionEx;
    FontCharacterRangeEx* rangesEx;
    FontCharacterEx* charactersEx;

    uint8_t readUInt8(File fontFile);
    int8_t readInt8(File fontFile);
    uint16_t readUInt16(File fontFile);
    uint32_t readUInt32(File fontFile);

    TinyResourceFileHeader loadTinyResourceFileHeaderFromFile(File fontFile);
    TinyResourceHeader loadTinyResourceHeaderFromFile(File fontFile);
    FontMetrics loadFontMetricsFromFile(File fontFile);
    FontDescription loadFontDescriptionFromFile(File fontFile);
    BitmapDescription loadBitmapDescriptionFromFile(File fontFile);
    FontCharacterRange loadRangeFromFile(File fontFile);
    void loadRangesFromFile(File fontFile, int count);
    FontCharacter loadCharacterFromFile(File fontFile);
    void loadCharactersFromFile(File fontFile, int count);
    
};
