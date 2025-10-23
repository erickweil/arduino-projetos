//#include <MemoryFree.h>
//#include <avr/pgmspace.h>
#include <TFTLCD7781.h>
#include <TouchScreen.h>
// *********************************************************** Declarações iniciais de valores fixos ****************************************************************************************

#define YP A2  
#define XM A1 
#define YM 6 
#define XP 7   

#define TS_MINX 120
#define TS_MINY 120
#define TS_MAXX 930
#define TS_MAXY 950

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 400);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0 
#define LCD_RESET A4

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF
#define GREY            0xBDF7
#define GRAY            0x7BEF
#define MARROM          0x79E0
#define AZULADO         0xB71C
TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define MINPRESSURE 10
#define MAXPRESSURE 5000
#define W 320
#define H 240
//________________________________________________________AQUI TEMOS A INICIALIZAÇAO,                           ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void setup(void) 
{
  tft.reset();
  tft.initDisplay(); 
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  tft.fillScreen(WHITE);  
}
//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void loop()
{
  Point p = ts.getPoint();                                        
  pinMode(XM, OUTPUT);                                             
  pinMode(YP, OUTPUT);                                               
  if (p.z > MINPRESSURE ) // verifica se o toque está dentro da margem de erro 
  {
   // turn from 0->1023 to tft.width                                
   int y = H-map(p.y, TS_MINX, TS_MAXX, H, 0);                        
   int x = map(p.x, TS_MINY, TS_MAXY, W, 0);
   tft.drawPixel(x,y,BLACK); 
  }
}







