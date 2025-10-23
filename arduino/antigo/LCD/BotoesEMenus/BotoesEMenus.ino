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
#define MARROM          0x79E0
#define AZULADO         0xB71C
#define clarinho        0xEF5D
#define Azuladoclaro    0xD6DD
#define Azulcontorn     0xAE7E
#define Azulfill        0xEF9E
TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define MINPRESSURE 10
#define MAXPRESSURE 5000

// Dimensoes da tela < talvez esteja invertido
int H=0; // altura
int W=0; // largura
#define MAXRES 50
// todas as Escalas
// passos em segundos...
#define ESCseResolucao1 2 
#define ESCseResolucao2 10
#define ESCseResolucao3 120
#define ESCseResolucao4 600
//....

#define ESCSTRIP 3 // numero de elementos que definem uma leitura
const byte Escalas[] PROGMEM = {
// NColunas     , Colincrem, ValorDoUltimo para aumentar 
    5           , 1        ,    0,// instantaneo
   (MAXRES-5)/10  ,10      ,    0,// passos de 2 seg
   (MAXRES-5)/6 , 1        ,   ESCseResolucao2/ESCseResolucao1,// passos de 10 seg
   (MAXRES-5)/5 , 10       ,   ESCseResolucao3/ESCseResolucao2,// passos de 2 MIN
   (MAXRES-5)/6 , 1        ,   ESCseResolucao4/ESCseResolucao3 // passos de 10 MIN
};
// ********************************** VALORES GUARDADOS NA MEMORIA FLASH ******************
//#define MAX_STRING 30
//char stringBuffer[MAX_STRING];
//const char BemVindo[] PROGMEM =         "Bem Vindo";
// ....
#define TELA_GLOBAL     0
#define TELA_Inicial    1
#define TELA_Sobre      2
#define TELA_Carregar   3
#define TELA_Salvar     4
#define TELA_Opcoes     5
#define TELA_Detalhes   6
#define TELA_Sensores   7
#define TELA_Ajuda      8
#define TELA_Grafico    9
//******************* janelinhas popup ******************
//#define POP_TecladoNum  101

#define POS_X 15
#define POS_Y 240/5
#define D 240/5


//******************* estilos de botao *******************
#define BOT_Normal      0
#define BOT_BarraMenus  1
#define BOT_BarraScroll 2
#define BOT_Contornoap  3
#define BOT_FundoMarrom 4
#define BOT_InputNum    5
#define BOT_Texto       6
//******************* Acoes de botoes *******************
#define BOT_ACAO_Apenasfundo 99
#define BOT_ACAO_SemAcao  100
#define BOT_ACAO_GraficoC  101
#define BOT_ACAO_GraficoS  102
#define BOT_ACAO_GraficoT  103
#define BOT_ACAO_ZOOM_IN   104
#define BOT_ACAO_ZOOM_OUT  105
#define EXIT               106
#define BOT_ACAO_InputNum  107
#define BOT_ACAO_Inputok    108


#define BOT_ACAO_N0         110
#define BOT_ACAO_N1         111
#define BOT_ACAO_N2         112
#define BOT_ACAO_N3         113
#define BOT_ACAO_N4         114
#define BOT_ACAO_N5         115
#define BOT_ACAO_N6         116
#define BOT_ACAO_N7         117
#define BOT_ACAO_N8         118
#define BOT_ACAO_N9         119
#define BOT_ACAO_NM         120

 // *********************************  ARRAYS DE ESTRUTURAS NA MEMORIA FLASH ******************************
#define BSTRIP 19 // numero de bytes que definem um unico botao
#define NBotoes 43
#define BOT_H 35
#define BOT_W 80
#define Menu_W 320/4
const byte botoes[] PROGMEM = {
//X MSB   ,X LSB    ,Y MSB    ,Y LSB   ,Width     ,Height       ,chars 0-8                                  ,quetela          ,EstiloDobotao  ,Pai (NBotoes se não tiver), acao(0 se nao tiver)

  0       ,0        ,0        ,0       ,Menu_W     ,BOT_H        ,'M','e','n','u','\0','\0','\0','\0','\0'   ,TELA_GLOBAL   ,BOT_BarraMenus  ,NBotoes,BOT_ACAO_SemAcao,
  // filhos do menu
      0       ,0        ,0        ,BOT_H*1   ,BOT_W*2     ,BOT_H        ,'C','a','r','r','e','g','a','r','\0'     ,TELA_GLOBAL   ,BOT_BarraScroll  ,0,TELA_Carregar,
      0       ,0        ,0        ,BOT_H*2   ,BOT_W*2     ,BOT_H        ,'S','a','l','v','a','r','\0','\0','\0'   ,TELA_GLOBAL   ,BOT_BarraScroll  ,0,TELA_Salvar,
      0       ,0        ,0        ,BOT_H*3   ,BOT_W*2     ,BOT_H        ,'S','o','b','r','e','\0','\0','\0','\0'  ,TELA_GLOBAL   ,BOT_BarraScroll  ,0,TELA_Sobre,
      0       ,0        ,0        ,BOT_H*4   ,BOT_W*2     ,BOT_H        ,'S','a','i','r','\0','\0','\0','\0','\0' ,TELA_GLOBAL  ,BOT_BarraScroll   ,0,EXIT,
  0       ,Menu_W    ,0        ,0       ,Menu_W     ,BOT_H        ,'J','a','n','e','l','a','\0','\0','\0'    ,TELA_GLOBAL   ,BOT_BarraMenus  ,NBotoes,BOT_ACAO_SemAcao,
      0       ,Menu_W    ,0        ,BOT_H*1   ,BOT_W*2     ,BOT_H        ,'O','p','c','o','e','s','\0','\0','\0'   ,TELA_GLOBAL   ,BOT_BarraScroll  ,5,TELA_Opcoes,
      0       ,Menu_W    ,0        ,BOT_H*2   ,BOT_W*2     ,BOT_H        ,'D','e','t','a','l','h','e','s','\0'     ,TELA_GLOBAL   ,BOT_BarraScroll  ,5,TELA_Detalhes,
      0       ,Menu_W    ,0        ,BOT_H*3   ,BOT_W*2     ,BOT_H        ,'S','e','n','s','o','r','e','s','\0'     ,TELA_GLOBAL  ,BOT_BarraScroll   ,5,TELA_Sensores,
  0       ,Menu_W*2 ,0        ,0       ,Menu_W     ,BOT_H        ,'G','r','a','f','.','\0','\0','\0','\0'    ,TELA_GLOBAL   ,BOT_BarraMenus  ,NBotoes,BOT_ACAO_SemAcao,
      0     ,Menu_W*2    ,0        ,BOT_H*1   ,BOT_W*2  ,BOT_H  ,'T','e','m','p','e','r','a','t','u',TELA_GLOBAL,BOT_BarraScroll,9   ,BOT_ACAO_GraficoC,
      0     ,Menu_W*2    ,0        ,BOT_H*2   ,BOT_W*2  ,BOT_H  ,'S','a','l','i','n','i','d','a','d',TELA_GLOBAL,BOT_BarraScroll,9   ,BOT_ACAO_GraficoS, 
      0     ,Menu_W*2    ,0        ,BOT_H*3   ,BOT_W*2  ,BOT_H  ,'T','u','r','b','i','d','e','z','\0',TELA_GLOBAL,BOT_BarraScroll,9   ,BOT_ACAO_GraficoT,   
  0       ,Menu_W*3 ,0        ,0       ,Menu_W     ,BOT_H        ,'A','j','u','d','a','\0','\0','\0','\0'    ,TELA_GLOBAL   ,BOT_BarraMenus  ,NBotoes,TELA_Ajuda,
    0     ,0       ,0        ,55   ,150  ,30    ,'C','|','|','|','|','|','|','|','|',TELA_Sensores,BOT_Contornoap,NBotoes   ,BOT_ACAO_GraficoC,
    0     ,0       ,0        ,85   ,150  ,30    ,'S','|','|','|','|','|','|','|','|',TELA_Sensores,BOT_Contornoap,NBotoes   ,BOT_ACAO_GraficoS, 
    0     ,0       ,0        ,115  ,150  ,30    ,'T','|','|','|','|','|','|','|','|',TELA_Sensores,BOT_Contornoap,NBotoes   ,BOT_ACAO_GraficoT,   
    
    0     ,0       ,0        ,200  ,240  ,40    ,'\0','\0','\0','\0','\0','\0','\0','\0','\0',TELA_Grafico ,BOT_FundoMarrom,NBotoes   ,BOT_ACAO_Apenasfundo,
    0     ,240     ,0        ,200  ,100  ,40    ,'\0','\0','\0','\0','\0','\0','\0','\0','\0',TELA_Grafico ,BOT_FundoMarrom,NBotoes   ,BOT_ACAO_Apenasfundo,
    0     ,15       ,0        ,205 ,85   ,30 ,'V','o','l','t','a','r','\0','\0','\0',TELA_Grafico ,BOT_Normal    ,NBotoes   ,TELA_Sensores,
    0     ,105       ,0        ,205 ,40   ,30    ,'-','\0','\0','\0','\0','\0','\0','\0','\0',TELA_Grafico ,BOT_Normal    ,NBotoes   ,BOT_ACAO_ZOOM_IN,
    0     ,150      ,0        ,205 ,40   ,30     ,'+','\0','\0','\0','\0','\0','\0','\0','\0',TELA_Grafico ,BOT_Normal    ,NBotoes   ,BOT_ACAO_ZOOM_OUT,
    
    0     ,0       ,0        ,50         ,BOT_W  ,BOT_H    ,'T','e','m','p','\0','\0','\0','\0','\0',TELA_Opcoes,BOT_Texto,NBotoes   ,BOT_ACAO_Apenasfundo,
    0     ,0       ,0        ,50+BOT_H   ,BOT_W  ,BOT_H    ,'S','a','l','i','\0','\0','\0','\0','\0',TELA_Opcoes,BOT_Texto,NBotoes   ,BOT_ACAO_Apenasfundo, 
    0     ,0       ,0        ,50+BOT_H*2 ,BOT_W  ,BOT_H    ,'T','u','r','b','\0','\0','\0','\0','\0',TELA_Opcoes,BOT_Texto,NBotoes   ,BOT_ACAO_Apenasfundo,

    0     ,POS_X+D*2+10 +D      ,0        ,50         ,50  ,BOT_H    ,'\0','\0','\0','\0','\0','\0','\0','<','C',TELA_Opcoes,BOT_InputNum,NBotoes   ,BOT_ACAO_InputNum,
    0     ,POS_X+D*2+10 +D      ,0        ,50+BOT_H   ,50  ,BOT_H    ,'\0','\0','\0','\0','\0','\0','\0','<','S',TELA_Opcoes,BOT_InputNum,NBotoes   ,BOT_ACAO_InputNum, 
    0     ,POS_X+D*2+10 +D      ,0        ,50+BOT_H*2 ,50  ,BOT_H    ,'\0','\0','\0','\0','\0','\0','\0','<','T',TELA_Opcoes,BOT_InputNum,NBotoes   ,BOT_ACAO_InputNum,    
    0     ,POS_X+D*2+10+50+D    ,0        ,50         ,50  ,BOT_H    ,'\0','\0','\0','\0','\0','\0','\0','>','C',TELA_Opcoes,BOT_InputNum,NBotoes   ,BOT_ACAO_InputNum,
    0     ,POS_X+D*2+10+50+D    ,0        ,50+BOT_H   ,50  ,BOT_H    ,'\0','\0','\0','\0','\0','\0','\0','>','S',TELA_Opcoes,BOT_InputNum,NBotoes   ,BOT_ACAO_InputNum, 
    0     ,POS_X+D*2+10+50+D    ,0        ,50+BOT_H*2 ,50  ,BOT_H    ,'\0','\0','\0','\0','\0','\0','\0','>','T',TELA_Opcoes,BOT_InputNum,NBotoes   ,BOT_ACAO_InputNum,
    
    
    //*** popup de teclado numerico ****
  0,POS_X+D*0  ,0,POS_Y+D*0   ,D     ,D      ,'7','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N7,
  0,POS_X+D*1  ,0,POS_Y+D*0   ,D     ,D      ,'8','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N8,
  0,POS_X+D*2  ,0,POS_Y+D*0   ,D     ,D      ,'9','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N9,

  0,POS_X+D*0  ,0,POS_Y+D*1   ,D     ,D      ,'4','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N4,
  0,POS_X+D*1  ,0,POS_Y+D*1   ,D     ,D      ,'5','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N5,
  0,POS_X+D*2  ,0,POS_Y+D*1   ,D     ,D      ,'6','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N6,
    
  0,POS_X+D*0  ,0,POS_Y+D*2   ,D     ,D      ,'1','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N1,
  0,POS_X+D*1  ,0,POS_Y+D*2   ,D     ,D      ,'2','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N2,
  0,POS_X+D*2  ,0,POS_Y+D*2   ,D     ,D      ,'3','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N3,

  0,POS_X+D*0  ,0,POS_Y+D*3   ,D     ,D      ,'0','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_N0,
  0,POS_X+D*1  ,0,POS_Y+D*3   ,D     ,D      ,'-','\0','\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_NM,
  0,POS_X+D*2  ,0,POS_Y+D*3   ,D     ,D      ,'O','k' ,'\0','\0','\0','\0','\0','\0','\0',TELA_GLOBAL,BOT_Normal,NBotoes+1  ,BOT_ACAO_Inputok
};
boolean Estado[NBotoes+2]={};
// todas as leituras
#define LESTRIP 5 // numero de elementos que definem uma leitura
const int Leituras[] PROGMEM= {
// LeituraMIN, LeituraMAX, Min   , Max   , Divporquantos 
   900       , 1100      ,-60    , 120   , 6           ,// Temperatura
    0        , 28        , 0     , 100   , 5           ,// Umidade
   20        , 80        , 0     , 1000  , 5            // Turbidez
};
int Leituras2[] = {
// LeituraMIN, LeituraMAX, Min   , Max   , Divporquantos 
   -60      , 120       ,-60    , 120   , 6           ,// Temperatura
   0        , 100       , 0     , 100   , 5           ,// Umidade
   0        , 1000      , 0     , 1000  , 5            // Turbidez
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
// ************************* inicialização ***************************

byte quetela = TELA_Sensores;
byte quejanelinha = 0;
boolean novo = true;
// programa que calcula uma expressão numérica
String Texto="";
byte umbyte;
byte at;
// ********************** declarações de valores amplamente utilizados e variaveis *****************

byte px=1;
byte qual=0;
byte perai=0;
unsigned int Mili=0;
byte QuantasVezes=0;
byte resolucao=0;
#define YMAX 198
#define YMIN 35
#define NSENSORES 3
#define DiviDeTempo 5 // quantos resoluções de tempo teremos
byte contagens[DiviDeTempo] = {};
byte Temperatura [MAXRES*DiviDeTempo] ={}; 
byte Umidade     [MAXRES*DiviDeTempo] ={};     
byte Turbidez    [MAXRES*DiviDeTempo] = {};  
int Lei[NSENSORES]={};
boolean des=true;
boolean submenuaberto = false;
void(* resetFunc) (void) = 0; //declare reset function @ address 0
void setup(void) 
{
Estado[NBotoes] = true;  
//pinMode(led,OUTPUT);//inicia o led
//Serial.begin(9600);// permite a leitura da porta serial a 9600 bits/s
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
      /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA INICIAL               @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
#define umchar 4*6
#define passo 4*6
tft.setTextSize(4);
tft.setTextColor(BLACK);
tft.fillScreen(WHITE);
for(int x=W;x>10;x-=passo)
{  
tft.setCursor(x+passo+umchar*0,H/2);  
tft.setTextColor(WHITE);tft.print(F("B"));
tft.setCursor(x+umchar*0,H/2);  
tft.setTextColor(MARROM);tft.print(F("B"));
tft.setCursor(x+passo+umchar*1,H/2);  
tft.setTextColor(WHITE);tft.print(F("E"));
tft.setCursor(x+umchar*1,H/2);  
tft.setTextColor(GREEN);tft.print(F("E"));
tft.setCursor(x+passo+umchar*2,H/2);  
tft.setTextColor(WHITE);tft.print(F("M"));
tft.setCursor(x+umchar*2,H/2);  
tft.setTextColor(AZULADO);tft.print(F("M"));
tft.setCursor(x+passo+umchar*3,H/2);  
tft.setTextColor(WHITE);tft.print(F(" "));
tft.setCursor(x+umchar*3,H/2);  
tft.setTextColor(MARROM);tft.print(F(" "));
tft.setCursor(x+passo+umchar*4,H/2);  
tft.setTextColor(WHITE);tft.print(F("V"));
tft.setCursor(x+umchar*4,H/2);  
tft.setTextColor(GREEN);tft.print(F("V"));
tft.setCursor(x+passo+umchar*5,H/2);  
tft.setTextColor(WHITE);tft.print(F("I"));
tft.setCursor(x+umchar*5,H/2);  
tft.setTextColor(AZULADO);tft.print(F("I"));
tft.setCursor(x+passo+umchar*6,H/2);  
tft.setTextColor(WHITE);tft.print(F("N"));
tft.setCursor(x+umchar*6,H/2);  
tft.setTextColor(MARROM);tft.print(F("N"));
tft.setCursor(x+passo+umchar*7,H/2);  
tft.setTextColor(WHITE);tft.print(F("D"));
tft.setCursor(x+umchar*7,H/2);  
tft.setTextColor(GREEN);tft.print(F("D"));
tft.setCursor(x+passo+umchar*8,H/2);  
tft.setTextColor(WHITE);tft.print(F("O"));
tft.setCursor(x+umchar*8,H/2);  
tft.setTextColor(AZULADO);tft.println(F("O"));
}
tft.setTextSize(2);
tft.setTextColor(BLACK);
tft.println(F("Desenvolvido por Erick Weil"));
delay(1000);
  Tela(TELA_Sensores);

  // Zerando os arrays
  for(int i=0;i<MAXRES*DiviDeTempo;i++)
  {
    Temperatura [i] =YMAX; 
    Umidade     [i] =YMAX;     
    Turbidez    [i] =YMAX;  
  }
}


//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void loop()
{
  unsigned long StartTime = millis();
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
//________________________________________________________AQUI TEMOS AS TELAS DO PROGRAMA                                   _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________  
  switch(quetela)
  {

        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA SOBRE                 @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Sobre:
    if(novo)
    {
    tft.setCursor(0,BOT_H);
tft.setTextSize(2);
tft.setTextColor(BLACK);
tft.println(F("Programador : Erick L Weil"));
tft.println(F("Objetivo : "));tft.setTextSize(2);
tft.println(F(" providenciar um ponto de\n partida para criacao de \n menus em um ambiente ardu-\n ino com uma tela lcd tft\n sem gpu incluido, ou seja\n toda operacao grafica e\n feita no proprio arduino"));
    }
    break;
        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA CARREGAR              @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Carregar:
    if(novo)
    {
    tft.setCursor(0,BOT_H);  
    tft.println(F("Nao tem nada para Carregar '-'"));
    }
    break;
        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA SALVAR                @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Salvar:
    if(novo)
    {
    tft.setCursor(0,BOT_H);    
    tft.println(F("Nao tem nada para Salvar '-'"));
    }
    break;
        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA Opcoes                @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Opcoes:
    if(novo)
    {

    }
    break;
        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA Detalhes              @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Detalhes:
    if(novo)
    {  

    }
    break;
        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA SENSORES              @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Sensores:
  if(novo)
  {
//  resolucao = -1;  
  tft.setTextSize(2);
//  tft.fillScreen(WHITE);
  tft.setCursor(120,BOT_H);
  tft.setTextColor(BLACK);
  tft.println(F("Sensores"));
  tft.setCursor(0,58);
  tft.print(F("Temperatura"));
  tft.setCursor(130,58);
  tft.println(F("000  C"));
  tft.println();
  tft.print(F("Salinidade"));
  tft.setCursor(130,90);
  tft.println(F("000  Mg/L"));
  tft.println();
  tft.print(F("Turbidez"));
  tft.setCursor(130,120);
  tft.print(F("000 NTU"));
  tft.setTextSize(3);
  }
  if(perai==0&&!submenuaberto)
  { 
  tft.setTextSize(3); 
  for(byte i=0;i<NSENSORES;i++)
  {
  tft.setCursor(130,58+32*i);  
  tft.fillRect(130,58+32*i-2,51,24,WHITE);
  tft.println(Lei[i]);
  }
//delay(60);
  }
    break;
        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA AJUDA                 @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Ajuda:
    if(novo)
    {
    tft.setCursor(0,BOT_H);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);    
    tft.println(F("** Navegacao **"));
    tft.println(F("para mudar as telas use"));
    tft.println(F("o menu dropdown 'Janela'"));
    tft.println(F("** Sensores **"));
    tft.println(F("na tela dos sensores"));
    tft.println(F("voce vera as medidas"));
    tft.println(F("em tempo real, clique"));
    tft.println(F("nelas para entrar no"));
    tft.println(F("grafico respectivo"));
    }
    break;
        /*
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                                                @@@@@@@@@@@@@@@@@@@@@@@               TELA GRAFICO               @@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
                                                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                 
    */
    case TELA_Grafico:
    if(novo)
    {
//      resolucao = 0;
//      tft.fillRect(0,200,W,40,MARROM);  
//    tft.fillRect(0,0,W,200-BOT_H,WHITE);
      tft.fillRect(230,200,60,40,MARROM);
      tft.setTextSize(2); 
      tft.setCursor(230,210);
      tft.setTextColor(BLUE);
                if(qual ==0)
      tft.print(F("Temp"));
       else     if(qual ==1)
      tft.print(F("Sali"));
       else     if(qual ==2)
      tft.print(F("Turb"));
      desenharEscala(resolucao);  
    }
    if(des&&!submenuaberto)
    {
  tft.setTextSize(2);
  tft.setCursor(280,210);
  tft.setTextColor(BLUE);
  tft.fillRect(280,210,34,17,MARROM);
  tft.println(Lei[qual]);
  switch(qual)//>>>>>>>>>>> VERIFICA QUAL GRAFICO DEVE SER DESENHADO
  {
    case 0 : desenharGrafico(Temperatura);   break;
    case 1 : desenharGrafico(Umidade    );   break;
    case 2 : desenharGrafico(Turbidez   );   break;
  }
  des=false;
    }
  break;
  }
//________________________________________________________AQUI TEMOS AS ATUALIZAÇÔES DE LEITURAS DOS SENSORES               _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________  
  novo=false;
  if(perai<20) 
perai++;
else       // apenas a cada 20 loops é feito a leitura dos valores
{
perai=0;
int Leitura;
//sizeof(Temperatura)/sizeof(Temperatura[0])
Leitura = analogRead(A0); //                                                                                                  LEITURA DA TEMPERATURA
Leitura = map(Leitura, Get(Leituras,LESTRIP*0+0),Get(Leituras,LESTRIP*0+1),Get(Leituras,LESTRIP*0+2),Get(Leituras,LESTRIP*0+3));
Leitura = constrain(Leitura, Get(Leituras,LESTRIP*0+2),Get(Leituras,LESTRIP*0+3));
Lei[0] = Leitura;
Leitura = map(Leitura, Get(Leituras,LESTRIP*0+2),Get(Leituras,LESTRIP*0+3),YMIN,YMAX);  
Leitura = constrain(Leitura,YMIN,YMAX);
Leitura = YMAX-Leitura+YMIN; 
empurrar(Temperatura,0,MAXRES,Leitura);  

Leitura = analogRead(A1);//                                                                                                   LEITURA DA SALINIDADE
Leitura = map(Leitura, Get(Leituras,LESTRIP*1+0),Get(Leituras,LESTRIP*1+1),Get(Leituras,LESTRIP*1+2),Get(Leituras,LESTRIP*1+3));
Leitura = constrain(Leitura, Get(Leituras,LESTRIP*1+2),Get(Leituras,LESTRIP*1+3));
Lei[1] = Leitura;
Leitura = map(Leitura, Get(Leituras,LESTRIP*1+2),Get(Leituras,LESTRIP*1+3),YMIN,YMAX);  
Leitura = constrain(Leitura,YMIN,YMAX);
Leitura = YMAX-Leitura+YMIN;  
empurrar(Umidade,0,MAXRES,Leitura);  

Leitura = analogRead(A2);//                                                                                                   LEITURA DA TURBIDEZ
Leitura = map(Leitura, Get(Leituras,LESTRIP*2+0),Get(Leituras,LESTRIP*2+1),Get(Leituras,LESTRIP*2+2),Get(Leituras,LESTRIP*2+3));
Leitura = constrain(Leitura, Get(Leituras,LESTRIP*2+2),Get(Leituras,LESTRIP*2+3));
Lei[2] = Leitura;
Leitura = map(Leitura, Get(Leituras,LESTRIP*2+2),Get(Leituras,LESTRIP*2+3),YMIN,YMAX);  
Leitura = constrain(Leitura,YMIN,YMAX);
Leitura = YMAX-Leitura+YMIN;  
empurrar(Turbidez,0,MAXRES,Leitura);  

QuantasVezes++;
if(resolucao==0)
des=true;
}



unsigned long CurrentTime = millis();
Mili += CurrentTime - StartTime;

if(Mili>=1000*ESCseResolucao1)
{  
  //mem();
  contagens[1]++;
  Mili-=1000*ESCseResolucao1;
  QuantasVezes = constrain(QuantasVezes,1,MAXRES);
  AtualizarTudo(0,QuantasVezes,1);
  QuantasVezes=0;
  if(resolucao==1)
  des=true;
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Contagens <<<<<<<<<<<<<<<<<<<<<<<<<<<<
  for(byte i=2;i<DiviDeTempo;i++)
  {
    byte ai = i-1;
    if(contagens[ai]>=Get(Escalas,ESCSTRIP*i+2))
    {
      contagens[ai]=0;
      contagens[i]++;
      AtualizarTudo(MAXRES*ai,MAXRES*ai+Get(Escalas,ESCSTRIP*i+2),i);
      if(resolucao==i)
        des=true;
    }
  }
}



} //     Fim loop()
                             // >>>>>>>>>>>>>>> Função que Troca as Telas
void Tela(byte qualT)
{
quetela = qualT;
tft.fillScreen(WHITE);
DesenharMenus();
novo = true;
}
//________________________________________________________AQUI TEMOS OS EVENTOS, COMO FUNÇÔES SEPARADAS          __________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  QUANDO CLICAR EM ALGUM LUGAR DA TELA @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void apertou(int y,int x)
{
y = H-y;
boolean nonada = true;
boolean algoclicado = false;
for(byte i=0;i<NBotoes;i++) // passa por todos os botões
{
if((Get(botoes,BSTRIP*i+15)==quetela||Get(botoes,BSTRIP*i+15) == TELA_GLOBAL)&&Estado[Get(botoes,BSTRIP*i+17)]&&Get(botoes,BSTRIP*i+18)!=BOT_ACAO_Apenasfundo)
{  
int px =Get(botoes,BSTRIP*i+0)*256 + Get(botoes,BSTRIP*i+1);
int py =Get(botoes,BSTRIP*i+2)*256 + Get(botoes,BSTRIP*i+3);
byte width  =Get(botoes,BSTRIP*i+4);
byte height =Get(botoes,BSTRIP*i+5);
if(Estado[i])
algoclicado = true;
  if(x>px && y>py && x<px+width && y<py+height)
  {
    nonada = false;
    Estado[i] =  !Estado[i] ;
    // >>>>>>> ativar analizador de ações já que apertou em um botão
    botao(i,false);
    delay(120);
    if(Get(botoes,BSTRIP*i+16)==BOT_Normal)
    {
    Estado[i] =  false; 
    botao(i,false);  
    }
    acao(Get(botoes,BSTRIP*i+18)); 
    // desapertar outros botoes, que compartilham o mesmo pai
    for(byte k=0;k<NBotoes;k++)
    {
      if(k!=i&&(Get(botoes,BSTRIP*k+15)==quetela||Get(botoes,BSTRIP*k+15) == TELA_GLOBAL)&&Get(botoes,BSTRIP*k+17)==Get(botoes,BSTRIP*i+17))
      {
        if(Estado[k])
        {
        Estado[k] =  false;
        ApagarSubMenus(k,Get(botoes,BSTRIP*k+16)); 
        botao(k,false);
        }
      }
    }
    DesenharSubMenus(i,Get(botoes,BSTRIP*i+16));
    break;
  }
}
}
if(nonada&&algoclicado)
{
Tela(quetela);
}
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  QUANDO ABRIR OU FECHAR UM SUBMENU    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void ApagarSubMenus(byte filhodequem,byte estilo)
{
// primeiro apaga possivel menu aberto anteriormente
if(estilo != BOT_BarraMenus)
return;
for(byte i=0;i<NBotoes;i++)
{
  if(Get(botoes,BSTRIP*i+15) == quetela||Get(botoes,BSTRIP*i+15) == TELA_GLOBAL)
  {
    if(Get(botoes,BSTRIP*i+17)==filhodequem)  
    {
    submenuaberto = false;    
    botao(i,true);
    }
  }
}
}
void DesenharSubMenus(byte filhodequem,byte estilo)
{
// depois desenha o novo menu aberto
for(byte i=0;i<NBotoes;i++)
{
if((Get(botoes,BSTRIP*i+15) == quetela||Get(botoes,BSTRIP*i+15) == TELA_GLOBAL)&&Estado[Get(botoes,BSTRIP*i+17)]&&Get(botoes,BSTRIP*i+17)==filhodequem)
{
submenuaberto = true;  
botao(i,false);
}
}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  QUANDO TROCAR DE TELA                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DesenharMenus()
{
submenuaberto = false;  
for(byte i=0;i<NBotoes;i++)
{
Estado[i]=false;  
if((Get(botoes,BSTRIP*i+15) == quetela||Get(botoes,BSTRIP*i+15) == TELA_GLOBAL)&&Get(botoes,BSTRIP*i+17)==NBotoes)
{  
botao(i,false);
}
}
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  QUANDO CLICAR EM ALGUM BOTÃO         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
String NumBuffer = "";
void acao(char queacao)
{
  if(queacao>=110&&queacao<=120)
  {
  if(queacao<120)  
  NumBuffer += queacao-110; 
  else if(queacao==120)
  NumBuffer += '-'; 
  }
  else
  {
  switch(queacao)
  {
    case BOT_ACAO_Inputok:
    Estado[NBotoes+1] = false;
    for(byte k=0;k<NBotoes;k++)
    {
      if((Get(botoes,BSTRIP*k+15)==quetela)&&Get(botoes,BSTRIP*k+16)==BOT_InputNum)
      {
        if(Estado[k])
        {
           switch(Get(botoes,BSTRIP*k+14))
  {
  case 'C':
  if(Get(botoes,BSTRIP*k+13) == '<')
   Leituras2[LESTRIP*0+2] = proxnum(NumBuffer);
  else
   Leituras2[LESTRIP*0+3] = proxnum(NumBuffer);
  break;
  case 'S':
  if(Get(botoes,BSTRIP*k+13) == '<')
   Leituras2[LESTRIP*1+2] = proxnum(NumBuffer);
  else
   Leituras2[LESTRIP*1+3] = proxnum(NumBuffer);
  break;  
  case 'T':
  if(Get(botoes,BSTRIP*k+13) == '<')
   Leituras2[LESTRIP*2+2] = proxnum(NumBuffer);
  else
   Leituras2[LESTRIP*2+3] = proxnum(NumBuffer);
  break;    
  }
        }
      }
    }
    NumBuffer="";
    Tela(quetela);
    break;
    case BOT_ACAO_InputNum:
    Estado[NBotoes+1] = true;
    NumBuffer = "";   
    DesenharSubMenus(NBotoes+1,BOT_InputNum);
    break;
    case BOT_ACAO_Apenasfundo :
    break;
    case BOT_ACAO_SemAcao  :
    break; 
    case BOT_ACAO_GraficoC  :
    qual = 0;
    Tela(TELA_Grafico);
    break;
    case BOT_ACAO_GraficoS  :
    qual = 1;
    Tela(TELA_Grafico);
    break;
    case BOT_ACAO_GraficoT  :
    qual = 2;
    Tela(TELA_Grafico);
    break;
    case BOT_ACAO_ZOOM_IN   :
    tft.fillRect(0,BOT_H,W,200-BOT_H,WHITE);
    if(resolucao+1<=DiviDeTempo-1)
    resolucao+=1;
    desenharEscala(resolucao);
    des=true;
//    Estado[15]=false;
//    botao(15,false);
    break;
    case BOT_ACAO_ZOOM_OUT  :
    tft.fillRect(0,BOT_H,W,200-BOT_H,WHITE);
    if(resolucao-1>=0)
    resolucao-=1;
    desenharEscala(resolucao);
    des=true;
//   Estado[16]=false;
//   botao(16,false);
    break;
    case EXIT:
resetFunc(); //call reset 
    break;
    default:
    Tela(queacao);
    break;
  }
  }
}
int proxnum(String Texto)
{
byte at=0;  
umbyte =Texto.charAt(at++); // Lê o prox byte na porta serial
String num="";
if(umbyte == '-'||umbyte == '+')
{
num += (char)umbyte;
umbyte= Texto.charAt(at++); 
}
while(isDigit(umbyte)||umbyte=='.') // enquanto os bytes que forem lidos ainda forem digitos ou um ponto
{
num += (char)umbyte; // converte o byte ASCII em um char, e então concatena a string 'num'
umbyte = Texto.charAt(at++);  // Lê o prox byte na porta serial
}
if(num == ""){
//Serial.print(" ---- Faltou um numero aqui! ----- ");
return(0);}
return(atoi(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atof é abreviatura de "ASCII to Float" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atof) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um float
*/
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  QUANDO FOR ALTERADO O ESTADO DE ALGUM BOTÃO OU PRECISAR APAGAR UM BOTAO   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void botao(byte index,boolean limpar)
{     
  //0   1     2     3        4      5         6-7-8-9-10-11-12-13-14  15                16              17                      18
//X MSB,X LSB,Y MSB,Y LSB   ,Width ,Height   ,chars 0-4              ,quetela          ,EstiloDobotao  ,Pai (255 se não tiver) ,Acao 

//            MSB                                  LSB
int px =Get(botoes,BSTRIP*index+0)*256 + Get(botoes,BSTRIP*index+1);
int py =Get(botoes,BSTRIP*index+2)*256 + Get(botoes,BSTRIP*index+3);

byte width  =Get(botoes,BSTRIP*index+4) ;
byte height =Get(botoes,BSTRIP*index+5) ;
if(limpar)
{
tft.fillRect(px,py,width,height,WHITE);  
return;  
}
String txt="";
txt +=(char)Get(botoes,BSTRIP*index+6);
txt +=(char)Get(botoes,BSTRIP*index+7);
txt +=(char)Get(botoes,BSTRIP*index+8);
txt +=(char)Get(botoes,BSTRIP*index+9);
txt +=(char)Get(botoes,BSTRIP*index+10);
txt +=(char)Get(botoes,BSTRIP*index+11);
txt +=(char)Get(botoes,BSTRIP*index+12);
txt +=(char)Get(botoes,BSTRIP*index+13);
txt +=(char)Get(botoes,BSTRIP*index+14);
switch(Get(botoes,BSTRIP*index+16))
{
 case BOT_Normal: // ****************************************
 
  if(Estado[index])
  tft.fillRoundRect(px,py,width,height,5,AZULADO);
  else
  tft.fillRoundRect(px,py,width,height,5,GREY);
  
  tft.drawRoundRect(px,py,width,height,5,BLACK);
  tft.setTextSize(2);
  tft.setCursor(px+10,py+5);
  tft.setTextColor(BLACK);
  tft.print(txt);
  
 break; 
 case BOT_BarraMenus:// ****************************************
   if(Estado[index])
   {
  tft.fillRoundRect(px,py,width,height,5,0xCE7A);
  tft.drawRoundRect(px,py,width,height,5,0x4A6A);
   }
  else
  {
  tft.fillRect(px,py,width,height,Azuladoclaro);
  tft.drawLine(px,py+5,px,py+height-5,BLACK);
  tft.drawLine(px+width,py+5,px+width,py+height-5,BLACK);
  }
  
  tft.setTextSize(2);
  tft.setCursor(px+10,py+5);
  tft.setTextColor(BLACK);
  tft.print(txt);
  break;
  case BOT_BarraScroll: // *************************************
  if(Estado[index])
  {
  tft.fillRoundRect(px,py,width,height,5,Azulfill);
  tft.drawRoundRect(px,py,width,height,5,Azulcontorn);
  }
  else
  tft.fillRect(px,py,width,height,clarinho);
  tft.setTextSize(2);
  tft.setCursor(px+10,py+5);
  tft.setTextColor(BLACK);
  tft.print(txt);
  break;
  case BOT_Contornoap://*****************************************
  if(Estado[index])
  {
  tft.drawRoundRect(px,py,width,height,5,Azulcontorn);
  }
  else
  tft.drawRoundRect(px,py,width,height,5,clarinho);
  break;
  case BOT_FundoMarrom://****************************************
  tft.fillRect(px,py,width,height,MARROM);
  break;
  case BOT_InputNum ://******************************************
    if(Estado[index])
  {
  tft.fillRoundRect(px,py,width,height,5,Azulfill);
  tft.drawRoundRect(px,py,width,height,5,Azulcontorn);
  tft.drawRect(px+4,py+4,width-8,height-8,Azulcontorn);
  }
  else
  tft.fillRect(px,py,width,height,clarinho);
  
  tft.fillRect(px+5,py+5,width-10,height-10,WHITE);
  tft.setTextSize(2);
  tft.setCursor(px+6,py+6);
  tft.setTextColor(BLACK);
  switch(txt.charAt(8))
  {
  case 'C':
  if(txt.charAt(7) == '<')
  tft.print(Leituras2[LESTRIP*0+2]);
  else
  tft.print(Leituras2[LESTRIP*0+3]);
  break;
  case 'S':
  if(txt.charAt(7) == '<')
  tft.print(Leituras2[LESTRIP*1+2]);
  else
  tft.print(Leituras2[LESTRIP*1+3]);
  break;  
  case 'T':
  if(txt.charAt(7) == '<')
  tft.print(Leituras2[LESTRIP*2+2]);
  else
  tft.print(Leituras2[LESTRIP*2+3]);
  break;    
  }
  //tft.print(txt);
  break;
  case BOT_Texto: //************************************
  tft.setTextSize(2);
  tft.setCursor(px+5,py+2);
  tft.setTextColor(BLACK);
  tft.print(txt);
  break;
}


}
//________________________________________________________AQUI TEMOS ACESSOS A MEMORIA FLASH                                _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

byte Get(const byte* ref,int k) {
 return pgm_read_byte(&ref[k]);
}
int Get(const int* ref,int k) {
 return pgm_read_word(&ref[k]);
}
int Get(int* ref,int k) {
 return ref[k];
}
float Get(const float* ref,int k) {
 return pgm_read_dword(&ref[k]);
}
long Get(const long* ref,int k) {
 return pgm_read_dword(&ref[k]);
}
//________________________________________________________AQUI TEMOS MANIUPULAÇÕES GRAFICAS                                 _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
byte Remapear(byte v,byte q)
{
int m =(int) v;
//Leitura = YMAX-Leitura+YMIN; 
m = YMIN+YMAX-m;
m = map(m,YMIN,YMAX,Get(Leituras,LESTRIP*q+2),Get(Leituras,LESTRIP*q+3));
m = constrain(m,Get(Leituras,LESTRIP*q+2),Get(Leituras,LESTRIP*q+3));
//m = map(m,Get(Leituras,LESTRIP*q+2),Get(Leituras,LESTRIP*q+3),Leituras2[LESTRIP*q+2],Leituras2[LESTRIP*q+3]);
m = constrain(m,Leituras2[LESTRIP*q+2],Leituras2[LESTRIP*q+3]);
m = map(m,Leituras2[LESTRIP*q+2],Leituras2[LESTRIP*q+3],YMIN,YMAX);
m = constrain(m,YMIN,YMAX);
v =(byte)m;
v = YMAX-v+YMIN; 
return v;
}
void desenharGrafico(byte grafico[])//************************ DESENHA UM GRAFICO DE ACORDO COM O ARRAY DADO
{
  int larg = W/(MAXRES-5)+1;
  for(byte i=MAXRES-5;i>0;i--)
  {
    int x = W-(larg*i);
    int x2 = W-(larg*(i+1));
    byte y  = grafico[(MAXRES*resolucao)+i];
    byte y2 = grafico[(MAXRES*resolucao)+i+1];
    byte y3 = grafico[(MAXRES*resolucao)+i+2];
    y=Remapear(y,qual);
    y2=Remapear(y2,qual);
    y3=Remapear(y3,qual);
    tft.drawLine(x ,y2,x2,y3,WHITE);
    tft.drawLine(x ,y ,x2,y2,RED);
  }  
}

void desenharEscala(byte escala)
{ 
tft.setTextSize(1);
tft.setTextColor(BLACK);
  //        Medição
int inc = (Get(Leituras2,LESTRIP*qual+3)-Get(Leituras2,LESTRIP*qual+2))/Get(Leituras2,LESTRIP*qual+4);
int inicial = Get(Leituras2,LESTRIP*qual+2)+inc;
for(int i=Get(Leituras2,LESTRIP*qual+4)-1;i>=0;i--)
{
tft.drawHorizontalLine(20,YMIN+((YMAX-YMIN)/Get(Leituras2,LESTRIP*qual+4))*i,10,GREEN);
tft.setCursor(0,YMIN+((YMAX-YMIN)/Get(Leituras2,LESTRIP*qual+4))*i);
tft.println(inicial);
inicial += inc;
}
//   tft.drawHorizontalLine(X,Y,Width,Cor);
//   tft.drawVerticalLine  (X,Y,Height,Cor);
//  Escalas[ESCSTRIP];
inc=0;
//          tempo
for(int i=Get(Escalas,ESCSTRIP*escala);i>0;i--)
{
tft.drawVerticalLine((W/Get(Escalas,ESCSTRIP*escala))*i,170,10,GREEN);
tft.setCursor((W/Get(Escalas,ESCSTRIP*escala))*i,H-55);
tft.println(inc);
inc+=Get(Escalas,ESCSTRIP*escala+1);
}

}

//________________________________________________________AQUI TEMOS OUTRAS FUNÇÔES                                         _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

void empurrar(byte graf[],byte de,byte ate,byte Leitura) // todos os elementos do array graf é empurrado em 1, e o index "de" recebe a Leitura, "ate" foi empurrado para fora
{ 
 byte temp=0;
 byte temp2=Leitura;
  for(byte i=de;i<ate-2;i+=2)
  {
  temp = graf[i];
  graf[i] = temp2;
  temp2=graf[i+1];
  graf[i+1] = temp;
  }
}
byte media(byte graf[],byte de,byte ate)
{
int retorno=0;
byte q=0;
  for(byte i=de;i<ate;i++)
  {
  retorno += graf[i];
  q++;
  }
return retorno/(q);  
}
void AtualizarTudo(byte a,byte b,byte c)
{
// LEITURA DA TEMPERATURA
empurrar(Temperatura,MAXRES*c,MAXRES*(c+1),media(Temperatura,a,b));  
// LEITURA DA Umidade
empurrar(Umidade,MAXRES*c,MAXRES*(c+1),media(Umidade,a,b));  
// LEITURA DA Turbidez
empurrar(Turbidez,MAXRES*c,MAXRES*(c+1),media(Turbidez,a,b));  
}
