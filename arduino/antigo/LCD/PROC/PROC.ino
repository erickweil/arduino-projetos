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

//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 400);

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
unsigned int coratual = BLACK;
//________________________________________________________AQUI TEMOS A INICIALIZAÇAO,                           ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
#define PMAX 300
int count=1;
int PontosX[PMAX] = {};
int PontosY[PMAX] = {};
void setup(void) 
{
  Serial.begin(9600);
  tft.reset();
  tft.initDisplay(); 
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  tft.fillScreen(WHITE);  
  randomSeed(analogRead(0));
  PontosX[0] = random(W);
PontosY[0] = random(H);
}
//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
#define distrandom 30
void loop()
{
//  Point p = ts.getPoint();                                        
//  pinMode(XM, OUTPUT);                                             
//  pinMode(YP, OUTPUT);                                               
//  if (p.z > MINPRESSURE ) // verifica se o toque está dentro da margem de erro 
//  {
//   // turn from 0->1023 to tft.width                                
//   int y = H-map(p.y, TS_MINX, TS_MAXX, H, 0);                        
//   int x = map(p.x, TS_MINY, TS_MAXY, W, 0);
//   tft.drawPixel(x,y,BLACK); 
//  }
if(count==PMAX-1)
{
count = 1;
PontosX[0] = PontosX[PMAX-2];
PontosY[0] = PontosY[PMAX-2];
PontosX[PMAX-2] = 0;
PontosY[PMAX-2] = 0;
coratual = tft.Color565(random(255),random(255),random(255));
//tft.fillScreen(WHITE); 
}
LinhaPontoprox(PontosX[count-1]+random(distrandom)-random(distrandom),PontosY[count-1]+random(distrandom)-random(distrandom));
//delay(20);
}

void LinhaPontoprox(int x,int y)
{
if(x>W||x<0||y>H||y<0)
return;
PontosX[count]=x;
PontosY[count]=y;  
count++;  
int xm=x;
int ym=y;
long md=1000000;
for(int i=0;i<PMAX;i++)
{
if((PontosX[i] ==0&&PontosY[i]==0)||i==count-1)
continue;
long d = Dist(x,y,PontosX[i],PontosY[i]);
if(md>d)
{
Serial.print(md);
Serial.print(" > ");
Serial.println(d);
md =d;
xm =PontosX[i];
ym =PontosY[i];
}
}
tft.drawLine(xm,ym,x,y,coratual);
}

long Dist(int x1, int y1, int x2, int y2)
{
long rx = x2-x1;
long ry = y2-y1;
return (rx*rx)+(ry*ry);  
}







