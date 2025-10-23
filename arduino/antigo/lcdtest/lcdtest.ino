//#include <MemoryFree.h>
#include <avr/pgmspace.h>
#include <TFTLCD7781.h>
#include <TouchScreen.h>
// *********************************************************** Declarações iniciais de valores fixos ****************************************************************************************
//#define Serialprintln(x) SerialPrint_P(PSTR(x))

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
#define MARROM           0x79E0

TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define MINPRESSURE 10
#define MAXPRESSURE 5000

#define BOXSIZE 40
int PENRADIUS =5;
int oldcolor, currentcolor;

// Dimensoes da tela < talvez esteja invertido
int H=0; // altura
int W=0; // largura
// ********************************** VALORES GUARDADOS NA MEMORIA FLASH ******************
#define MAX_STRING 30
char stringBuffer[MAX_STRING];
const char BemVindo[] PROGMEM =         "Bem Vindo";
// ....

 // *********************************  ARRAYS DE ESTRUTURAS NA MEMORIA FLASH ******************************

//                                                                               para não esquecer exemplos de codigos
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
// ************************* inicialização ***************************
boolean des=true;
void setup(void) {
  Serial.begin(9600);
  tft.reset();
  tft.initDisplay(); 
  W = tft.height();
  H = tft.width();
  tft.setRotation(1); 
  tft.fillScreen(WHITE);
//  linha(100,50,150,200,5,BLACK);
//  delay(2000);
//  linha(20,43,20,80,5,GREEN);
//  delay(2000);
//  linha(130,25,123,124,5,RED);
// delay(2000);
//  linha(75,60,24,38,67,BLUE);
//  delay(2000);
  pinMode(A0,INPUT);
}
// ********************** declarações de valores amplamente utilizados e variaveis *****************
byte perai=0;
byte quetela=0;
unsigned int apert=0;
int ay;
int ax;

//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void loop()
{
// ************ isso deve ser deixado desta exata ordem, ou o touchscreen não funciona **************
//  digitalWrite(13, HIGH);                                          //>
  Point p = ts.getPoint();                                           //>
//  digitalWrite(13, LOW);                                           //>
  pinMode(XM, OUTPUT);                                               //>
  pinMode(YP, OUTPUT);                                               //>
// >>>>>>>>>>>>>>>>>VERIFICAÇÂO DO TOQUE<<<<<<<<<<<<<<<<<<           //>  
  if (p.z > MINPRESSURE ) // verifica se o toque está dentro da margem de erro 
  {
   // turn from 0->1023 to tft.width                                 //>
   p.y = map(p.y, TS_MINX, TS_MAXX, H, 0);                           //>
   p.x = map(p.x, TS_MINY, TS_MAXY, W, 0);  
   apert=0;
   apertou(p.y,p.x); // evoca o evento de toque  
   //>
  }
  apert++;
  if(apert>200)
  {
    ay =0;
    ax =0; 
  }
//>
// >>>>>>>>>>>>>>>>>ESCOLHA DAS TELAS<<<<<<<<<<<<<<<<<<
//________________________________________________________AQUI TEMOS AS TELAS,ORGANIZADAS dentro do switch case____________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
switch(quetela)
{
 case 0:
 //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: TELA PRINCIPAL :::::::::::::::::::::::::::::
  if(des)
  {
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
 // tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, WHITE);
 
 tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
 currentcolor = RED;
  des = false;
  }
  if(perai==0)
  { 

  }
break; 
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: TELA DO GRAFICO :::::::::::::::::::::::::::::
case 1:
if(des)
{
  des=false;
}
break;
}
// perai é uma variavel que permite que entre 20 loops apenas 1 seja usado para desenhas na tela
// ou seja, 19 loops podem receber eventos de toque, onde cada um leva poucos milesimos para rodar, e então
// é feito o grafico ou atualização dos valores, que leva muito tempo sem responder a eventos de toque
if(perai<20) 
perai++;
else       // apenas a cada 20 loops é feito a leitura dos valores
{
perai=0;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>()///  
}

//________________________________________________________AQUI TEMOS OS EVENTOS, COMO FUNÇÔES SEPARADAS          __________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

void linha(int x0, int y0, int x1, int y1, float wd, unsigned int cor)
{ 
   int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1; 
   int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1; 
   int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
   float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);
   
   for (wd = (wd+1)/2; ; ) {                                   /* pixel loop */
      tft.drawPixel(x0,y0,cor);//max(0,255*(abs(err-dx+dy)/ed-wd+1)));
      e2 = err; x2 = x0;
      if (2*e2 >= -dx) {                                           /* x step */
         for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
             tft.drawPixel(x0, y2 += sy,cor); //max(0,255*(abs(e2)/ed-wd+1)));
         if (x0 == x1) break;
         e2 = err; err -= dy; x0 += sx; 
      } 
      if (2*e2 <= dy) {                                            /* y step */
         for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
             tft.drawPixel(x2 += sx, y0,cor);// max(0,255*(abs(e2)/ed-wd+1)));
         if (y0 == y1) break;
         err += dx; y0 += sy; 
      }
   }
   tft.fillCircle(x0,y0,wd/2,cor);
   tft.fillCircle(x1,y1,wd/2,cor);
}
void apertou(int y,int x)//***********************>>>>>>>>>> QUANDO CLICAR EM ALGUM LUGAR DA TELA
{
  y = H-y; 
      if (y < BOXSIZE) {
       oldcolor = currentcolor;
 
  
       if (x < BOXSIZE) { 
         currentcolor = RED; 
         tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
       } 
       else if (x < BOXSIZE*2) {
         currentcolor = YELLOW; 
         tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
       }
       else if (x < BOXSIZE*3) {
         currentcolor = GREEN; 
         tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
       }
       else if (x < BOXSIZE*4) {
         currentcolor = CYAN; 
         tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
       }
       else if (x < BOXSIZE*5) {
         currentcolor = BLUE; 
         tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
       }
       else if (x < BOXSIZE*6) {
         currentcolor = MAGENTA; 
         tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
       }
        else if (x < BOXSIZE*7) {
PENRADIUS++;
       }
        else if (x < BOXSIZE*8) {
          if(PENRADIUS>0)
PENRADIUS--;
       }
       
       if (oldcolor != currentcolor) {
          if (oldcolor == RED) tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
          if (oldcolor == YELLOW) tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
          if (oldcolor == GREEN) tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
          if (oldcolor == CYAN) tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
          if (oldcolor == BLUE) tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
          if (oldcolor == MAGENTA) tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
       }
    }
       if (((y-PENRADIUS) > BOXSIZE) && ((y+PENRADIUS) < tft.height()))
      { 
 if(ay==0||ax==0)
 {
ay = y;
ax = x; 
 }
 if((ax-x)*(ax-x)+(ay-y)*(ay-y)<PENRADIUS*3)
 return;
linha(x,y,ax,ay,PENRADIUS,currentcolor);
ay = y;
ax = x;
       }
}
//________________________________________________________AQUI TEMOS ACESSOS A MEMORIA FLASH                                _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________


char* Get(const char* str) {
	strcpy_P(stringBuffer, (char*)str);
	return stringBuffer;
}
byte Get(const byte* ref,byte k) {
 return pgm_read_byte(&ref[k]);
}
int Get(const int* ref,byte k) {
 return pgm_read_word(&ref[k]);
}
float Get(const float* ref,byte k) {
 return pgm_read_dword(&ref[k]);
}
long Get(const long* ref,byte k) {
 return pgm_read_dword(&ref[k]);
}
