#include <DCCSignal.h>

// graphics
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// DCC
const byte dccPin = 2;  // digital pin for the incoming dcc signal

// Graphics
byte d_width;  // width of diagramm
byte d_height;  // height of diagramm
byte graphicsPos; // offset for graphics
byte y1;  // height y = 0
byte y2;  // height y = 1
byte dccPos; // offset for dcc output

void setup(void) {
  // Graphics

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  // tft.setSPISpeed(40000000);

  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  
  d_width = tft.width() - 2;  // width of diagramm
  d_height = tft.height() / 8 - 2;  // height of diagramm
  graphicsPos = 20; // offset for graphics
  y1 = graphicsPos+d_height;  // height y = 0
  y2 = graphicsPos+1;         // height y = 1
  dccPos = graphicsPos + d_height + 10; // offset for dcc output

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextWrap(false);
  tft.setCursor(0, 5);
  tft.print("DCC Monitor");

  tft.drawRect(0, graphicsPos, d_width+2, d_height+2, ST77XX_WHITE);
    
  // DCC
  DCC::begin(dccPin);
}

void loop() {
  // analyze DCC data
  if(DCC::hasDCCData()) {    // dcc packet received
    DCC::dcc_packet dccp = DCC::readDCCPacket();
    // Graphics
    tft.fillRect(1, y2, d_width, d_height, ST77XX_BLACK);
    byte x_coord = 1;

    // preamble
    for(byte i = 0; i < dccp.preamble_size; i++) {
      x_coord = drawBit(1, x_coord);
    }

    // DCC output
    tft.setCursor(0, dccPos);
    for(byte i = 0; i <= dccp.data_size; i++) {
      tft.print(dccp.data[i], HEX);
      tft.print(" ");
      x_coord = drawBit(0, x_coord);
      for(short j = 7; j >=0; j--) {
        x_coord = drawBit(bitRead(dccp.data[i],j), x_coord);
      }
    }

    // end bit
    x_coord = drawBit(1, x_coord);
    
    for(byte i = dccp.data_size+1; i < DCC::maxNumberOfBytes; i++) {
      tft.print("   ");
    }
  }
}

// draw curve for bit at coordinate x and return new coordinate
byte drawBit(boolean bit, byte x) {
  if(bit) {
    tft.drawLine(x, y2, x+1, y1, ST77XX_GREEN);
    tft.drawLine(x+1, y1, x+2, y2, ST77XX_GREEN);
    return x + 2;
  }
  else {
    tft.drawLine(x, y2, x+1, y1, ST77XX_GREEN);
    tft.drawLine(x+1, y1, x+2, y1, ST77XX_GREEN);
    tft.drawLine(x+2, y1, x+3, y2, ST77XX_GREEN);
    tft.drawLine(x+3, y2, x+4, y2, ST77XX_GREEN);
    return x + 4;
  }
}
