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
#define MINPRESSURE 4
void Rot(int &x,int &y);
// Dimensoes da tela < talvez esteja invertido
int H=0; // altura
int W=0; // largura
// ********************************** VALORES GUARDADOS NA MEMORIA FLASH ******************
//#define MAX_STRING 30
//char stringBuffer[MAX_STRING];
//const char BemVindo[] PROGMEM =         "Bem Vindo";
// ....

 // *********************************  ARRAYS DE ESTRUTURAS NA MEMORIA FLASH ******************************

const char FORMAS[][20] = {
// npontos,x1,y1,x2,y2,x3,y3 ....
{4,0,20,-10,-10,0,0,10,-10},// player
{9,-2,-3,-4,-20,18,-10,23,10,-3,20,-15,12,-7,7,-18,4,-12,-20},// Asteroide 1
{9,2,-2,13,-11,-5,-18,-15,-7,-10,4,-14,14,0,20,15,10,15,-7},
{9,-12,5,-7,26,18,11,22,-1,20,-10,8,-13,2,-21,-21,-17,-26,-7},
{5,0,5,11,16,16,-3,8,-14,-15,-12,-20,7},
{5,-7,-24,-15,-14,-18,0,-5,14,10,16,22,9},
{5,-2,9,14,24,23,7,10,-16,-19,-12,-24,13}
};
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
  #define TELA_Inicial 0
  #define TELA_Asteroides 1
  #define TELA_Perdeu 2
  #define TELA_Sobre 3
  // ********************** declarações de valores amplamente utilizados e variaveis *****************  
byte perai=0;
byte quetela=TELA_Asteroides;

boolean novo=true;
// ************************* inicialização ***************************
#define Ent_strip 7 
byte Ent_count = 0;
int dificuldade =50;
int Entidades[100] = {
// index da forma, x , y , rot, vx, vy ,S  
};
void setup(void) {
  Serial.begin(9600);
  tft.reset();
  tft.initDisplay(); 
  //********************************************** consertando problema onde o primeiro pixel é desenhado com a cor anterior ***********************************************************
  for(byte i=0;i<20;i++)
  {
  tft.drawPixel(10,12,GREY);
   tft.writeRegister(TFTLCD7781_GRAM_HOR_AD, 10); // GRAM Address Set (Horizontal Address) (R20h)
 tft.writeRegister(TFTLCD7781_GRAM_VER_AD, 12); // GRAM Address Set (Vertical Address) (R21h)
 tft.writeCommand(TFTLCD7781_RW_GRAM);  // Write Data to GRAM (R22h)
int data = tft.readData();

  tft.drawPixel(10,12,GREY);
 tft.writeRegister(TFTLCD7781_GRAM_HOR_AD, 10); // GRAM Address Set (Horizontal Address) (R20h)
 tft.writeRegister(TFTLCD7781_GRAM_VER_AD, 12); // GRAM Address Set (Vertical Address) (R21h)
 tft.writeCommand(TFTLCD7781_RW_GRAM);  // Write Data to GRAM (R22h)
 data = tft.readData();
 
 delay(20);
//  Serial.print(data,BIN);
//  Serial.print(F(" Tinha de ser "));
//  Serial.println(0x52AA,BIN);  
  if(data==0x52AA)
  break;
  else
  tft.initDisplay(); 
  }
  W = tft.height();
  H = tft.width();
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  randomSeed(analogRead(0));
  AddEntidade(0,W/2,H/2,0,0,0,10);
 // AddEntidade(1,200,150,0,2,1,10);
  
}

void AddEntidade(int forma,int x,int y,int rot,int vx, int vy,int S)
{
if(Ent_count<10)
{
Entidades[Ent_count*Ent_strip+0] = forma;
Entidades[Ent_count*Ent_strip+1] = x;
Entidades[Ent_count*Ent_strip+2] = y;
Entidades[Ent_count*Ent_strip+3] = rot;
Entidades[Ent_count*Ent_strip+4] = vx;
Entidades[Ent_count*Ent_strip+5] = vy;
Entidades[Ent_count*Ent_strip+6] = S;
Ent_count++;
}
}

void RemoverEntidade(byte index)
{
if(index ==0)
{
Tela(TELA_Perdeu);  
return;
}
if(Ent_count>1)
{
char S = Entidades[Ent_strip*index+6] ;  
int x=Entidades[Ent_strip*index+1];
int y=Entidades[Ent_strip*index+2];
DesEntidade(index,BLACK);
while(random(1000)>40)
{
tft.drawPixel(x+random(30)-random(30),y+random(30)-random(30),random(0xFFFF)); 
}
  if(index!=Ent_count)
  {
Entidades[Ent_strip*index+0] =Entidades[(Ent_count-1)*Ent_strip+0];
Entidades[Ent_strip*index+1] =Entidades[(Ent_count-1)*Ent_strip+1];
Entidades[Ent_strip*index+2] =Entidades[(Ent_count-1)*Ent_strip+2];
Entidades[Ent_strip*index+3] =Entidades[(Ent_count-1)*Ent_strip+3];
Entidades[Ent_strip*index+4] =Entidades[(Ent_count-1)*Ent_strip+4];
Entidades[Ent_strip*index+5] =Entidades[(Ent_count-1)*Ent_strip+5];
Entidades[Ent_strip*index+6] =Entidades[(Ent_count-1)*Ent_strip+6];
  }
Ent_count--;
if(S>=7)
{
while(random(S)<S*0.75)  
AddEntidade(random(3,7),x+random(20)-random(20),y+random(20)-random(20),0,random(10)-random(10),random(10)-random(10),(float)S*0.5); 
}
}
}
void Rotacionar(byte index,int angulo)
{
Entidades[index*Ent_strip+3] += angulo;  
while(Entidades[index*Ent_strip+3] >=360)
Entidades[index*Ent_strip+3] -= 360;  
}
void AplicarV(byte index)
{
Entidades[index*Ent_strip+1] += Entidades[index*Ent_strip+4];
Entidades[index*Ent_strip+2] += Entidades[index*Ent_strip+5];
if(Entidades[index*Ent_strip+1]<0)
Entidades[index*Ent_strip+1] += W;
if(Entidades[index*Ent_strip+2]<0)
Entidades[index*Ent_strip+2] += H;
if(Entidades[index*Ent_strip+1]>W)
Entidades[index*Ent_strip+1] -= W;
if(Entidades[index*Ent_strip+2]>H)
Entidades[index*Ent_strip+2] -= H;
}
boolean VerificarSeBate(int x1,int y1,int x2, int y2, int D)
{
    int dx=x2-x1;
    int dy=y2-y1;  
    if(sqrt((dx*dx)+(dy*dy))<D)
    {
    //  Serial.print(sqrt((dx*dx)+(dy*dy)));
    //  Serial.print("<");
    //  Serial.println(D);
      return true;
    }
return false;
}
//________________________________________________________AQUI TEMOS ACESSOS A MEMORIA FLASH                                _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

byte Get(const byte* ref,byte k) {
 return pgm_read_byte(&ref[k]);
}
char Get(const char* ref,byte k) {
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
//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void loop()
{
  Point p = ts.getPoint();                                           //>
  pinMode(XM, OUTPUT);                                               //>
  pinMode(YP, OUTPUT);                                               //>  
  if (p.z > MINPRESSURE ) // verifica se o toque está dentro da margem de erro 
  {
   // turn from 0->1023 to tft.width                                 //>
   p.y = map(p.y, TS_MINX, TS_MAXX, H, 0);                           //>
   p.x = map(p.x, TS_MINY, TS_MAXY, W, 0);  
   apertou(p.y,p.x); // evoca o evento de toque  
  }

//________________________________________________________AQUI TEMOS AS TELAS,ORGANIZADAS dentro do switch case____________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
switch(quetela)
{
 case TELA_Inicial:
 //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: TELA Inicial :::::::::::::::::::::::::::::
if(novo)
{
tft.fillScreen(BLACK);  
novo=false;
}
if(perai==0)
{

}
break; 
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: TELA Dos Asteroides :::::::::::::::::::::::::::::
case TELA_Asteroides:
if(novo)
{
tft.fillScreen(BLACK);    
novo=false;
}
if(perai==0)
{
 for(byte i=0;i<Ent_count;i++)
{
DesEntidade(i,BLACK);  
Rotacionar(i,6);
AplicarV(i);
DesEntidade(i,WHITE);
if(i>0)
{
if(VerificarSeBate(Entidades[0*Ent_strip+1],Entidades[0*Ent_strip+2],Entidades[i*Ent_strip+1],Entidades[i*Ent_strip+2],25))
{
Tela(TELA_Perdeu);  
}
}
}
if(random(1000)<dificuldade)
{
int x=random(W);
int y=random(H);
if(!VerificarSeBate(Entidades[0*Ent_strip+1],Entidades[0*Ent_strip+2],x,y,50))
{
 if(random(2)==1) 
AddEntidade(random(1,4),x,y,0,random(8)-random(8),random(8)-random(8),random(7,11)); 
else
AddEntidade(random(4,7),x,y,0,random(12)-random(12),random(12)-random(12),random(3,8)); 
}
if(dificuldade<500)
dificuldade+=1;
}
//delay(200); 
}
break;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: TELA Perdeu :::::::::::::::::::::::::::::
case TELA_Perdeu:
if(novo)
{
Ent_count=1;  
dificuldade=50;  
//tft.fillScreen(BLACK);  
tft.setCursor(20,H/2);
tft.setTextSize(5);
tft.setTextColor(WHITE);
tft.print(F("G"));delay(200);tft.setTextColor(GREY);
tft.print(F("A"));delay(200);tft.setTextColor(WHITE);
tft.print(F("M"));delay(200);tft.setTextColor(GREY);
tft.print(F("E"));delay(200);tft.setTextColor(WHITE);
tft.print(F(" "));delay(200);tft.setTextColor(GREY);
tft.print(F("O"));delay(200);tft.setTextColor(WHITE);
tft.print(F("V"));delay(200);tft.setTextColor(GREY);
tft.print(F("E"));delay(200);tft.setTextColor(WHITE);
tft.print(F("R"));delay(200);
novo=false;
}
if(perai==0)
{ 
}
break;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: TELA Sobre :::::::::::::::::::::::::::::
case TELA_Sobre:
if(novo)
{
tft.fillScreen(BLACK);   
novo=false;
}
if(perai==0)
{ 
}
break;
}
// perai é uma variavel que permite que entre 20 loops apenas 1 seja usado para desenhas na tela
// ou seja, 19 loops podem receber eventos de toque, onde cada um leva poucos milesimos para rodar, e então
// é feito o grafico ou atualização dos valores, que leva muito tempo sem responder a eventos de toque
if(perai<20) 
perai++;
else       // apenas a cada 20 loops é feito a leitura dos valores
perai=0; 
}

//________________________________________________________AQUI TEMOS OS EVENTOS, COMO FUNÇÔES SEPARADAS          __________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void apertou(int y,int x)//***********************>>>>>>>>>> QUANDO CLICAR EM ALGUM LUGAR DA TELA
{
y = H-y;

switch(quetela)
{
case TELA_Asteroides:
//tft.drawCircle(x,y,4,GRAY);
for(byte i=0;i<Ent_count;i++)
{
if(VerificarSeBate(x,y,Entidades[i*Ent_strip+1],Entidades[i*Ent_strip+2],15))
{
//tft.drawCircle(x,y,10,GREY);
RemoverEntidade(i);
}
}
break;
case TELA_Perdeu:

Tela(TELA_Asteroides);  
break;
}
//delay(200);
}
void Tela(byte que)
{
quetela=que;
novo=true;
}
void DesEntidade(byte qual,uint16_t color)
{
//#define Ent_strip 4 
//byte Ent_count = 0;
//int Entidades[40]  
// index da forma, x , y , rot,
int q =Entidades[qual*Ent_strip];
float S =(float)Entidades[qual*Ent_strip+6]/10;
for(byte i=1;i<=(FORMAS[q][0]*2)-3;i+=2)
{
int px1 =(int)FORMAS[q][i]*S ;
int py1 =(int)FORMAS[q][i+1]*S ;
int px2 =(int)FORMAS[q][i+2]*S ;
int py2 =(int)FORMAS[q][i+3]*S ;
Rot(px1,py1,Entidades[qual*Ent_strip+3]);
Rot(px2,py2,Entidades[qual*Ent_strip+3]);
px1 +=Entidades[qual*Ent_strip+1];
py1 +=Entidades[qual*Ent_strip+2];
px2 +=Entidades[qual*Ent_strip+1];
py2 +=Entidades[qual*Ent_strip+2];
tft.drawLine(px1,py1,px2,py2,color);
            if( i==(FORMAS[q][0]*2)-3)
            {
            int pxI =(int)FORMAS[q][1]*S ;
            int pyI =(int)FORMAS[q][1+1]*S ;  
            Rot(pxI,pyI,Entidades[qual*Ent_strip+3]);
            pxI +=Entidades[qual*Ent_strip+1];
            pyI +=Entidades[qual*Ent_strip+2];  
            tft.drawLine(pxI ,pyI ,px2 ,py2 ,color); 
            }           
}
}
void Rot(int &x,int &y,int theta)
{
int temp = x;  
x=(float)temp*cos(theta * PI / 180.0 ) - (float)y*sin(theta * PI / 180.0 );
y=(float)temp*sin(theta * PI / 180.0 ) + (float)y*cos(theta * PI / 180.0 );
}

