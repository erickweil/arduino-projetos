#include "TFTLCD7781.h"
#include "TouchScreen.h"


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



TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  Serial.begin(9600);
  Serial.println("Paint!");
  tft.reset();
  tft.initDisplay(); 
  tft.fillScreen(WHITE);
  // exemplos dos codigos
  
  // ************************* formas geometricas ******************************
  /*
   tft.fillRect(X,Y,Width,Height,COR);  faz um quadrado preenchido
   tft.drawRect(X,Y,Width,Height,COR);  faz apenas o contorno de um quadrado
   tft.fillCircle(X,Y,RAIO,COR); faz um circulo preenchido
   tft.drawRoundRect(X,Y,Width,Height, Raio,Cor);
   tft.drawCircle(X,Y,RAIO,COR); faz um circulo contornado apenas
   tft.drawLine(X,Y,X2,Y2,COR); faz uma linha de x,y para x2,y2
   tft.drawTriangle(X1,Y1,X2,Y2,X3,Y3,Cor);
   tft.drawHorizontalLine(X,Y,Width,Cor);
   tft.drawVerticalLine(X,Y,Height,Cor);
  */
  //************************** operações na tela *******************************
  /*
  tft.setRotation(0-4);  rotaciona a tela
  */
  pinMode(13, OUTPUT); // n sei
  pinMode(A0,INPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000
int px;
int Leitura;
void loop()
{
  digitalWrite(13, HIGH);
  Point p = ts.getPoint();
  digitalWrite(13, LOW);
  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
   // turn from 0->1023 to tft.width
   p.y = map(p.y, TS_MINX, TS_MAXX, tft.width(), 0);
   p.x = map(p.x, TS_MINY, TS_MAXY, tft.height(), 0);
   apertou(p.y,p.x);
  }
// aqui vem seu codigo
Leitura = analogRead(A0);
Leitura = map(Leitura,TS_MINX, TS_MAXX, tft.width(), 0);
tft.drawLine(0,px,tft.width(),px,WHITE);
tft.drawLine(0,px,Leitura,px,GREEN);
if(px<tft.height())
px++;
else
{
px=0;
}
}
void apertou(int x,int y)
{
  
//tft.fillCircle(x,y,6,BLACK);
}
