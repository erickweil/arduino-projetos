//#include <MemoryFree.h>
#include <avr/pgmspace.h>
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
//________________________________________________________AQUI TEMOS ESTRUTURAS NA MEMORIA FLASH                ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
#define Des_MensaI 0
#define Des_MensaF 22
const char Formas[] PROGMEM = {
-44,20,-44,-20,-7,3,-44,20,44,20,44,-20,7,3,0,0,-7,3,-44,-20,44,-20,  
};
#define BOTSTR 5
#define NBotoes 1
const int botoesPos[] PROGMEM = {
// x , y , w , h ,
   0 , 0 , 0 , 0 ,
};
const byte botoesInf[] PROGMEM = {
//quetela      ,EstiloDobotao  ,queTexto  , acao(0 se nao tiver)

0              ,0              ,0         ,0                   ,
}; 
const char Texto1[] PROGMEM = "Texto1";
const char Texto2[] PROGMEM = "Texto2";
const char Texto3[] PROGMEM = "Texto3";
const char Texto4[] PROGMEM = "Texto4";
const char Texto5[] PROGMEM = "Texto5";
const char Texto6[] PROGMEM = "Texto6";
const char Texto7[] PROGMEM = "Texto7";
const char Texto8[] PROGMEM = "Texto8";
const char Texto9[] PROGMEM = "Texto9";
const char Texto10[] PROGMEM = "Texto10";
const char* TodoTexto[] PROGMEM = {
Texto1,
Texto2,
Texto3,
Texto4,
Texto5,
Texto6,
Texto7,
Texto8,
Texto9,
Texto10
}; 
byte Estado[NBotoes+2]={};




//________________________________________________________AQUI TEMOS A INICIALIZAÇAO,                           ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
#define ESTILO_VermelhoPAmarelo 0
#define ESTILO_AmareloPVerde 1   
#define ESTILO_VerdePCiano 2   
#define ESTILO_CianoPAzul 3   
#define ESTILO_AzulPVioleta 4  
#define ESTILO_VioletaPVermelho 5 


byte quetela =0;

void setup(void) 
{
  tft.reset();
  tft.initDisplay(); 
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  tft.fillScreen(BLACK);  
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
   //tft.drawPixel(x,y,BLACK); 
//________________________________________________________QUANDO CLICAR EM ALGUM LUGAR DA TELA                           _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________









  }  //***************
}
//________________________________________________________AQUI TEMOS ACESSOS A MEMORIA FLASH                                _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

byte Get(const byte* ref,byte k) {
 return pgm_read_byte(&ref[k]);
}
String Get(const char* ref[],byte k) {
String n;
int i;
char c;
while((c=pgm_read_byte(&ref[k][i++])))
n+=c;
return n;
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
//________________________________________________________AQUI TEMOS FUNÇÕES GRAFICAS ESPECIAIS                             _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

void Gradiente(int x,int y,int width,int height,byte estilo,boolean prabaixo)
{
//unsigned int cor;
float passo =((float)height)/255.0;
for(float i=0;i<height;i+=passo)
{
int p;  
if(prabaixo)  
p = i/passo;
else
p = 255-i/passo;
switch(estilo)
{
case ESTILO_VermelhoPAmarelo:    
//cor = tft.Color565(255,p,0);  
drawGorizontalLine(x,y+i,width,255,p,0);
break;
case ESTILO_AmareloPVerde:    
//cor = tft.Color565(255-p,255,0); 
drawGorizontalLine(x,y+i,width,255-p,255,0);
break;
case ESTILO_VerdePCiano:    
//cor = tft.Color565(0,255,p);  
drawGorizontalLine(x,y+i,width,0,255,p);
break;
case ESTILO_CianoPAzul:    
//cor = tft.Color565(0,255-p,255);  
drawGorizontalLine(x,y+i,width,0,255-p,255);
break;
case ESTILO_AzulPVioleta:    
//cor = tft.Color565(p,0,255);  
drawGorizontalLine(x,y+i,width,p,0,255);
break;
case ESTILO_VioletaPVermelho:    
//cor = tft.Color565(255,0,255-p);  
drawGorizontalLine(x,y+i,width,255,0,255-p);
break;
}
//tft.drawHorizontalLine(x,y+i,width,cor);
}
}
void drawGorizontalLine(int x,int y,int width,byte R,byte G,byte B)
{

for(float i=0;i<width;i++)
{
float p;  
p = i/width;
tft.drawPixel(x+i,y,tft.Color565(R*p,G*p,B*p));
}
}





