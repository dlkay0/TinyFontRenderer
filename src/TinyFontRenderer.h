#include "TinyFont.h"

typedef void (*DrawPixelCallback) (const int x, const int y, const uint16_t opacity);

class TinyFontRenderer {
  public:
    TinyFontRenderer(String fontFilename);
    TinyFontRenderer(File fontFile);
    ~TinyFontRenderer();

    static const uint16_t OpacityOpaque = 256;
    static const uint16_t OpacityTransparent = 0;

    void drawString(uint32_t x, uint32_t y, String text, uint16_t maxChars, DrawPixelCallback drawPixelCallback, bool drawBackground = false);
    void drawString(uint32_t x, uint32_t y, String text, DrawPixelCallback drawPixelCallback, bool drawBackground = false);
    void drawCharacter(uint32_t x, uint32_t y, char c, DrawPixelCallback drawPixelCallback, bool drawBackground = false);
    uint32_t getStringWidth(String text);
    uint8_t getCharacterWidth(char c);
    
    uint16_t getHeight();
    uint16_t getOffset();
    uint16_t getAverageCharacterWidth();
    uint16_t getMaximumCharacterWidth();
    char* getDefinedCharacters();
    uint16_t getTopExternalMargin();
    uint16_t getNumberOfDefinedCharacters();

  private:
    void drawCharacter(uint32_t x, uint32_t y, FontCharacterInfo &fci, DrawPixelCallback drawPixelCallback, bool drawBackground = false);

    TinyFont* font;  

};
