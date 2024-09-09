#include <DCCSignal.h>

#if defined(MEGATINYCORE)
// ATtiny402/412 PORTA positions. Change these for the chip you're using
byte const dc = 7;
byte const mosi = 1;
byte const sck = 3;
byte const cs = 6;

// ATtiny 0-, 1-, and 2-series port manipulations - assumes all pins in same port
#define PORT_TOGGLE(x)  PORTA.OUTTGL = (x)
#define PORT_LOW(x)     PORTA.OUTCLR = (x)
#define PORT_HIGH(x)    PORTA.OUTSET = (x)
#define PORT_OUTPUT(x)  PORTA.DIRSET = (x)

#else
// ATtiny45/85 PORTB positions. Change these for the chip you're using
byte const dc = 0;
byte const mosi = 1;
byte const sck = 4;  // changed from 2 to 4 because 2 is used for external interrupt
byte const cs = 3;

// Classic ATtiny port manipulations - assumes all pins in same port
#define PORT_TOGGLE(x)  PINB = (x)
#define PORT_LOW(x)     PORTB = PORTB & ~((x));
#define PORT_HIGH(x)    PORTB = PORTB | ((x))
#define PORT_OUTPUT(x)  DDRB = (x)

#endif

// Adafruit 1.8" 160x128 display
short const xsize = 160, ysize = 128, xoff = 0, yoff = 0, invert = 0, rotate = 0, bgr = 1;

// Character set for text - stored in program memory
const uint8_t CharMap[96][6] PROGMEM = {
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 }, 
{ 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 }, 
{ 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 }, 
{ 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 }, 
{ 0x36, 0x49, 0x56, 0x20, 0x50, 0x00 }, 
{ 0x00, 0x08, 0x07, 0x03, 0x00, 0x00 }, 
{ 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 }, 
{ 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00 }, 
{ 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x80, 0x70, 0x30, 0x00, 0x00 }, 
{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 }, 
{ 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 }, 
{ 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 }, 
{ 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 }, 
{ 0x72, 0x49, 0x49, 0x49, 0x46, 0x00 }, 
{ 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00 }, 
{ 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 }, 
{ 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 }, 
{ 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00 }, 
{ 0x41, 0x21, 0x11, 0x09, 0x07, 0x00 }, 
{ 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00 }, 
{ 0x00, 0x00, 0x14, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x40, 0x34, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x14, 0x08, 0x00 }, 
{ 0x02, 0x01, 0x59, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00 }, 
{ 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 }, 
{ 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00 }, 
{ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 }, 
{ 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 }, 
{ 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 }, 
{ 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 }, 
{ 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00 }, 
{ 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 }, 
{ 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 }, 
{ 0x26, 0x49, 0x49, 0x49, 0x32, 0x00 }, 
{ 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00 }, 
{ 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 }, 
{ 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 }, 
{ 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00 }, 
{ 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 }, 
{ 0x03, 0x04, 0x78, 0x04, 0x03, 0x00 }, 
{ 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00 }, 
{ 0x00, 0x7F, 0x41, 0x41, 0x41, 0x00 }, 
{ 0x02, 0x04, 0x08, 0x10, 0x20, 0x00 }, 
{ 0x00, 0x41, 0x41, 0x41, 0x7F, 0x00 }, 
{ 0x04, 0x02, 0x01, 0x02, 0x04, 0x00 }, 
{ 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 }, 
{ 0x00, 0x03, 0x07, 0x08, 0x00, 0x00 }, 
{ 0x20, 0x54, 0x54, 0x78, 0x40, 0x00 }, 
{ 0x7F, 0x28, 0x44, 0x44, 0x38, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x44, 0x28, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x28, 0x7F, 0x00 }, 
{ 0x38, 0x54, 0x54, 0x54, 0x18, 0x00 }, 
{ 0x00, 0x08, 0x7E, 0x09, 0x02, 0x00 }, 
{ 0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x00 }, 
{ 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00 }, 
{ 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00 }, 
{ 0x20, 0x40, 0x40, 0x3D, 0x00, 0x00 }, 
{ 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00 }, 
{ 0x7C, 0x04, 0x78, 0x04, 0x78, 0x00 }, 
{ 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x44, 0x38, 0x00 }, 
{ 0xFC, 0x18, 0x24, 0x24, 0x18, 0x00 }, 
{ 0x18, 0x24, 0x24, 0x18, 0xFC, 0x00 }, 
{ 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00 }, 
{ 0x48, 0x54, 0x54, 0x54, 0x24, 0x00 }, 
{ 0x04, 0x04, 0x3F, 0x44, 0x24, 0x00 }, 
{ 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00 }, 
{ 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00 }, 
{ 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00 }, 
{ 0x44, 0x28, 0x10, 0x28, 0x44, 0x00 }, 
{ 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x00 }, 
{ 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00 }, 
{ 0x00, 0x08, 0x36, 0x41, 0x00, 0x00 }, 
{ 0x00, 0x00, 0x77, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x36, 0x08, 0x00, 0x00 }, 
{ 0x00, 0x06, 0x09, 0x06, 0x00, 0x00 },  // degree symbol = '~'
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 }
};

// TFT colour display **********************************************

byte const CASET = 0x2A; // Define column address
byte const RASET = 0x2B; // Define row address
byte const RAMWR = 0x2C; // Write to display RAM

short const White = 0xFFFF;
short const Black = 0;

// Current plot position and colours
short xpos, ypos;
short fore = White;
short back = Black;
short scale = 1;     // Text scale

// Send a byte to the display

void Data (uint8_t d) {
  for (uint8_t bit = 0x80; bit; bit >>= 1) {
    PORT_TOGGLE(1<<sck);
    if (d & bit) PORT_HIGH(1<<mosi); else PORT_LOW(1<<mosi);
    PORT_TOGGLE(1<<sck);
  }
}

// Send a command to the display
void Command (uint8_t c) {
  PORT_TOGGLE(1<<dc);
  Data(c);
  PORT_TOGGLE(1<<dc);
}

// Send a command followed by two data words
void Command2 (uint8_t c, uint16_t d1, uint16_t d2) {
  PORT_TOGGLE(1<<dc);
  Data(c);
  PORT_TOGGLE(1<<dc);
  Data(d1>>8); Data(d1); Data(d2>>8); Data(d2);
}
  
void InitDisplay () {
  PORT_OUTPUT(1<<dc | 1<<cs | 1<<mosi | 1<<sck); // All outputs
  PORT_HIGH(1<<dc | 1<<cs | 1<<sck);       // Outputs high
  PORT_TOGGLE(1<<cs);
  Command(0x01);                           // Software reset
  delay(250);                              // delay 250 ms
  Command(0x36); Data(rotate<<5 | bgr<<3); // Set orientation and rgb/bgr
  Command(0x3A); Data(0x55);               // Set color mode - 16-bit color
  Command(0x20+invert);                    // Invert
  Command(0x11);                           // Out of sleep mode
  delay(150);
  PORT_TOGGLE(1<<cs);
}

void DisplayOn () {
  PORT_TOGGLE(1<<cs);
  Command(0x29);                           // Display on
  delay(150);
  PORT_TOGGLE(1<<cs);
}

void ClearDisplay () {
  PORT_TOGGLE(1<<cs);
  Command2(CASET, yoff, yoff + ysize - 1);
  Command2(RASET, xoff, xoff + xsize - 1);
  Command(0x3A); Data(0x03);               // 12-bit colour
  Command(RAMWR);                          // Leaves mosi low
  for (short i=0; i<xsize*4; i++) {
    for (short j=0; j<ysize*3; j++) {
    PORT_TOGGLE(1<<sck);
    PORT_TOGGLE(1<<sck);
    }
  }
  Command(0x3A); Data(0x05);               // Back to 16-bit colour
  PORT_TOGGLE(1<<cs);
}

unsigned short Colour (short r, short g, short b) {
  return (r & 0xf8)<<8 | (g & 0xfc)<<3 | b>>3;
}

// Move current plot position to x,y
void MoveTo (short x, short y) {
  xpos = x; ypos = y;
}

// Plot point at x,y
void PlotPoint (short x, short y) {
  PORT_TOGGLE(1<<cs);
  Command2(CASET, yoff+y, yoff+y);
  Command2(RASET, xoff+x, xoff+x);
  Command(RAMWR); Data(fore>>8); Data(fore & 0xff);
  PORT_TOGGLE(1<<cs);
}

// Draw a line to x,y
void DrawTo (short x, short y) {
  short sx, sy, e2, err;
  short dx = abs(x - xpos);
  short dy = abs(y - ypos);
  if (xpos < x) sx = 1; else sx = -1;
  if (ypos < y) sy = 1; else sy = -1;
  err = dx - dy;
  for (;;) {
    PlotPoint(xpos, ypos);
    if (xpos==x && ypos==y) return;
    e2 = err<<1;
    if (e2 > -dy) { err = err - dy; xpos = xpos + sx; }
    if (e2 < dx) { err = err + dx; ypos = ypos + sy; }
  }
}

void FillRect (short w, short h) {
  PORT_TOGGLE(1<<cs);
  Command2(CASET, ypos+yoff, ypos+yoff+h-1);
  Command2(RASET, xpos+xoff, xpos+xoff+w-1);
  Command(RAMWR);
  uint8_t hi = fore>>8;
  uint8_t lo = fore & 0xff;
  for (short i=0; i<w; i++) {
    for (short j=0; j<h; j++) {
      Data(hi); Data(lo);
    }
  }
  PORT_TOGGLE(1<<cs);
}

void DrawRect (short w, short h) {
  short x1 = xpos, y1 = ypos;
  FillRect(w-1, 1); MoveTo(x1, y1+1);
  FillRect(1, h-1); MoveTo(x1+1, y1+h-1);
  FillRect(w-1, 1); MoveTo(x1+w-1, y1);
  FillRect(1, h-1);
  xpos = x1; ypos = y1;
}

void FillCircle (short radius) {
  short x1 = xpos, y1 = ypos, dx = 1, dy = 1;
  short x = radius - 1, y = 0;
  short err = dx - (radius<<1);
  while (x >= y) {
    MoveTo(x1-x, y1+y); FillRect(x<<1, 1);
    MoveTo(x1-y, y1+x); FillRect(y<<1, 1);
    MoveTo(x1-y, y1-x); FillRect(y<<1, 1);
    MoveTo(x1-x, y1-y); FillRect(x<<1, 1);
    if (err > 0) {
      x = x - 1; dx = dx + 2;
      err = err - (radius<<1) + dx;
    } else {
      y = y + 1; err = err + dy;
      dy = dy + 2;
    }
  }
  xpos = x1; ypos = y1;
}

void DrawCircle (short radius) {
  short x1 = xpos, y1 = ypos, dx = 1, dy = 1;
  short x = radius - 1, y = 0;
  short err = dx - (radius<<1);
  while (x >= y) {
    PlotPoint(x1-x, y1+y); PlotPoint(x1+x, y1+y);
    PlotPoint(x1-y, y1+x); PlotPoint(x1+y, y1+x);
    PlotPoint(x1-y, y1-x); PlotPoint(x1+y, y1-x);
    PlotPoint(x1-x, y1-y); PlotPoint(x1+x, y1-y);
    if (err > 0) {
      x = x - 1; dx = dx + 2;
      err = err - (radius<<1) + dx;
    } else {
      y = y + 1; err = err + dy;
      dy = dy + 2;
    }
  }
}

// Plot an ASCII character with bottom left corner at x,y
void PlotChar (char c) {
  short colour;
  PORT_TOGGLE(1<<cs);
  Command2(CASET, yoff+ypos, yoff+ypos+8*scale-1);
  Command2(RASET, xoff+xpos, xoff+xpos+6*scale-1);
  Command(RAMWR);
  for (short xx=0; xx<6; xx++) {
    byte bits = pgm_read_byte(&CharMap[c-32][xx]);
    for (short xr=0; xr<scale; xr++) {
      for (short yy=0; yy<8; yy++) {
        if (bits>>(7-yy) & 1) colour = fore; else colour = back;
        for (short yr=0; yr<scale; yr++) {
          Data(colour>>8); Data(colour & 0xFF);
        }
      }
    }
  }
  PORT_TOGGLE(1<<cs);
  xpos = xpos + 6*scale;
}

// Plot text starting at the current plot position
void PlotText(PGM_P p) {
  while (1) {
    char c = pgm_read_byte(p++);
    if (c == 0) return;
    PlotChar(c);
  }
}

void PlotInt(short n) {
  bool lead = false;
  for (short d=10000; d>0; d = d/10) {
    char j = (n/d) % 10;
    if (j!=0 || lead || d==1) { PlotChar(j + '0'); lead = true; }
  }
}

void TestChart () {
  DrawRect(xsize, ysize);
  scale = 8;
  fore = Colour(255, 0, 0);
  MoveTo((xsize-40)/2, (ysize-64)/2); PlotChar('F');
  scale = 1;
}
  
// DCC Monitor **********************************************

// Graphics
const short d_width = xsize - 2;  // width of diagramm
const short d_height = ysize / 8 - 2;  // height of diagramm
const short titlePos = ysize - 15;
const short graphicsPos = ysize - 35; // offset for graphics
const short dccPos = graphicsPos - 15;
const short y2 = graphicsPos + d_height;  // height y = 0
const short y1 = graphicsPos + 1;  // height y = 1

// DCC
const byte dccPin = 2;  // digital pin for the incoming dcc signal

void setup() {
  // Graphics
  InitDisplay();
  ClearDisplay();
  DisplayOn();
  
  MoveTo(0, titlePos);
  PlotText(PSTR("DCC Monitor"));
  MoveTo(0, graphicsPos);
  DrawRect(d_width+2, d_height+2);

  // DCC
  DCC::begin(dccPin);
}

void loop () {
  // analyze DCC data
  if(DCC::hasDCCData()){
    DCC::dcc_packet dccp = DCC::readDCCPacket();
    // Graphics
    MoveTo(1, y1);
    fore = Colour(0,0,0);
    FillRect(d_width, d_height);
    byte x_coord = 1;

    // preamble
    for(byte i = 0; i < dccp.preamble_size; i++){
      x_coord = drawBit(1, x_coord);
    }
    
    // DCC output
    MoveTo(0, dccPos);
    for(byte i = 0; i <= dccp.data_size; i++)
    {
      PlotHex(dccp.data[i]);
      PlotText(PSTR(" "));
    }
    for(byte i = dccp.data_size+1; i < DCC::maxNumberOfBytes; i++)
    {
      PlotText(PSTR("   "));
    }
    for(byte i = 0; i <= dccp.data_size; i++)
    {
      x_coord = drawBit(0, x_coord);
      for(short j = 7; j >=0; j--){
        x_coord = drawBit(bitRead(dccp.data[i],j), x_coord);
      }
    }

    // end bit
    x_coord = drawBit(1, x_coord);
  }
}

// convert one Byte DEC to HEX
char decToHex(byte z){
  if(z < 10)
  {
    return z + '0';
  }
  else
  {
    switch(z){
      case 10:
      return 'A';
      break;
      case 11:
      return 'B';
      break;
      case 12:
      return 'C';
      break;
      case 13:
      return 'D';
      break;
      case 14:
      return 'E';
      break;
      case 15:
      return 'F';
      break;     
    }
  }
}

// plot two digit HEX to the dcc output
void PlotHex(byte n) {
  fore = Colour(255,255,255);
  if(n == 0)
  {
    PlotChar(' ');
    PlotChar('0');
  }
  else {
    char z1 = decToHex(n / 16);
    z1 = (z1=='0'?' ':z1);
    PlotChar(z1);
    char z2 = decToHex(n % 16);
    PlotChar(z2);
  }
}

// drawLine function needed in the function drawBit
void drawLine(short x1, short y1, short x2, short y2)
{
  MoveTo(x1, y1);
  DrawTo(x2, y2);
}

// draw curve for bit at coordinate x and return new coordinate
byte drawBit(bool bit, byte x){
  fore = Colour(0, 255, 0);
  if(bit){
    drawLine(x, y2, x+1, y1);
    drawLine(x+1, y1, x+2, y2);
    return x + 2;
  }
  else
  {
    drawLine(x, y2, x+1, y1);
    drawLine(x+1, y1, x+2, y1);
    drawLine(x+2, y1, x+3, y2);
    drawLine(x+3, y2, x+4, y2);
    return x + 4;
  }
}
