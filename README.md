# Tiny Font Renderer

Tiny Font Renderer is a library which renders Tiny Font (*.TinyFnt) files which have been created from Truetype fonts using the Tiny Font Tool GUI.

![Version](https://img.shields.io/badge/Version-v1.0.0-green.svg)
<br />

## Features

The main features of the Tiny Font Renderer library are:

* Read Tiny Font (*.tinyfnt) files from SPIFFS or anything else which
  implements *File*.
* Render to ANYTHING via a *DrawPixelCallback*
* Supports fonts of various sizes
* Uses proportional character spacing
* Fully supports anti-aliasing for greyscale/color displays

![Screenshot](https://github.com/dlkay0/TinyFontRenderer/blob/master/ASCII_Art.PNG)
Video: https://www.youtube.com/watch?v=05vwcNHRuQs
## Installing This Library

Open the Library Manager and search for "TinyFontRenderer" and install


## Creating Tiny Font (*.TinyFnt) files

Obtain the Tiny Font Tool GUI from http://informatix.miloush.net/microframework/Utilities/TinyFontTool.aspx
<br />Configure the font so it fits your display, and click *Convert* to save the file.


## Placing Tiny Font (*.TinyFnt) files into SPIFFS

Use your favorite tool (such as the Arduino IDE ESP32 Sketch Data Upload tool) to copy the file to SPIFFS.

## Usage

~~~cpp
#include "TinyFontRenderer.h"	//Include the library
#include "SPIFFS.h"				//Include SPIFFS
#define ARRAY_WIDTH 200  // character array width in pixels
#define ARRAY_HEIGHT 32  // character array height in pixels


TinyFontRenderer *tinyFontRenderer;
char charArray[ARRAY_WIDTH * ARRAY_HEIGHT];


void setup() {
  Serial.begin(115200);
  SPIFFS.begin();			//Initialize SPIFFS
  tinyFontRenderer = new TinyFontRenderer("/ComicSans.tinyfnt");	//Load Tiny Font
}

void loop() {

  //Clear the charArray buffer
  ClearCharArray();
  
  //Draw a string at (0,<fontOffset>) and do not draw the background
  tinyFontRenderer->DrawString(0, tinyFontRenderer->GetOffset(), "ComicSans", charArrayCallback, false);
  
  //Render the charArray to Serial
  DrawCharArray();

  delay(10000);
}

// Implement a DrawPixelCallback method with the following signature
// x, y - coordinate of pixel to set
// opacity - 0-256 value indicating how opaque the set the pixel
//  (only 0 and 256 are used when the font doesn't contain antialiasing data)
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

void ClearCharArray() {
  for(int i = 0; i < ARRAY_WIDTH * ARRAY_HEIGHT; i++) {
    charArray[i] = ' ';
  }
}

void DrawCharArray() {
...
}
~~~

## Donate

If you like and use Tiny Font Renderer in your project, please consider donating a mere $2 to me here.
<br/>
Donate Via PayPal: https://paypal.me/dlkay0?locale.x=en_US


## Contibuting

For bugs, make sure there isn't an active issue and then create one.  Provide as many details as possible.


## Credits

This work is derived from the Bitmap and Font classes in the Microsoft .NETMF (https://github.com/NETMF/netmf-interpreter)
which is also licensed under the Apache-2.0 license.
