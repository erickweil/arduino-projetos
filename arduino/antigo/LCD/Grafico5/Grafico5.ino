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

#define MAXRES 50 // a máxima resolução , ou seja, o tamanho do array
// Dimensoes da tela < talvez esteja invertido
int H=0; // altura
int W=0; // largura
// ********************************** VALORES GUARDADOS NA MEMORIA FLASH ******************
#define MAX_STRING 30
char stringBuffer[MAX_STRING];
const char BemVindo[] PROGMEM =         "Bem Vindo";
const char Nome[] PROGMEM =             "Erick Weil";
const char Desenvolvido[] PROGMEM =     "       desenvolvido por ";//24
const char Sensores[] PROGMEM =         "Sensores :";

const char Leitura0[] PROGMEM =         "Temperatura";
const char Leitura1[] PROGMEM =         "Umidade";
const char Leitura2[] PROGMEM =         "Turbidez";
// ....
const char LeituraMedida0[] PROGMEM =   "000  C";
const char LeituraMedida1[] PROGMEM =   "000  %";
const char LeituraMedida2[] PROGMEM =   "000  NTU";
// ....
const char reduzido0[] PROGMEM =        "Temp";
const char reduzido1[] PROGMEM =        "Umid";
const char reduzido2[] PROGMEM =        "Turb";
// ....

 // *********************************  ARRAYS DE ESTRUTURAS NA MEMORIA FLASH ******************************
 
 
 // todos os botoes
 #define BSTRIP 7 // numero de elementos que definem um unico botao
const byte botoes[] PROGMEM = {
// X  ,Y  ,Width,Height,acao,qualtela?,LETRA

// botões da tela do grafico
  10  ,205 ,60  ,30    ,0   ,1,'M', // ir pra tela principal
  80  ,205 ,60  ,30    ,1   ,1,'-', // zoom out
  150 ,205 ,60  ,30    ,2   ,1,'+', // zoom in
  // botões da tela principal
  0 ,55   ,150  ,30    ,3   ,0,0,// ir pra Temperatura
  0 ,85   ,150  ,30    ,4   ,0,0, // ir pra Umidade
  0 ,115  ,150  ,30    ,5   ,0,0 // ir pra Turbidez
};
// todas as leituras
#define LESTRIP 5 // numero de elementos que definem uma leitura
const int Leituras[] PROGMEM = {
// LeituraMIN, LeituraMAX, Min   , Max   , Divporquantos 
   900       , 1100      ,-60    , 120   , 6           ,// Temperatura
    0        , 28        , 0     , 100   , 5           ,// Umidade
   20        , 80        , 0     , 1000  , 5            // Turbidez
};
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
   (MAXRES-5)/5 , 5        ,    0,// passos de 1 seg
   (MAXRES-5)/6 , 1        ,   ESCseResolucao2/ESCseResolucao1,// passos de 10 seg
   (MAXRES-5)/5 , 10       ,   ESCseResolucao3/ESCseResolucao2,// passos de 2 MIN
   (MAXRES-5)/6 , 1        ,   ESCseResolucao4/ESCseResolucao3 // passos de 10 MIN
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
boolean des=true;
void(* resetFunc) (void) = 0; //declare reset function @ address 0
void setup(void) {
//  Serial.begin(9600);
//  boolean resetar = true;
//  mem();
//  Serial.println("INICIANDO!");
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
  
  tft.fillScreen(WHITE);
  W = tft.height();
  H = tft.width();
  tft.setRotation(1); 
  //******************** tela inicial
  /*
  tft.setTextSize(4);
  tft.setCursor(20,80);
  tft.setTextColor(BLUE);
  tft.println(Get(BemVindo));
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.println(Get(Desenvolvido));
  tft.setTextColor(RED);
  tft.setTextSize(4);
  tft.println(Get(Nome)); 
  tft.setTextSize(2); 
  delay(2000); 
  */
  //////////////////////////
//  pinMode(13, OUTPUT);
  pinMode(A0,INPUT);
//  pinMode(A1,INPUT);
//  pinMode(A2,INPUT);
//  mem();
}
// ********************** declarações de valores amplamente utilizados e variaveis *****************

byte px=1;
//int Leitura=0;
byte qual=0;
byte perai=0;
unsigned int Mili=0;
byte QuantasVezes=0;
//byte QuantosSegundo=0;
//byte Quantos10Segundo=0;
//byte Quantos2Minutos=0;
//byte Quantos10Minutos=0;
//byte QuantasHoras=0;

byte quetela=0;
byte resolucao=0;

//boolean novo=true;
// aqui temos os arrays que receberão as leituras
#define YMAX 198
#define YMIN 0
#define NSENSORES 3
#define DiviDeTempo 5 // quantos resoluções de tempo teremos
byte contagens[DiviDeTempo] = {};
byte Temperatura [MAXRES*DiviDeTempo] ={}; 
byte Umidade     [MAXRES*DiviDeTempo] ={};     
byte Turbidez    [MAXRES*DiviDeTempo] = {};  
int Lei[NSENSORES]={};
//  MAXRES*DiviDeTempo*NSENSORES*TamanhodoTipo == BYTES alocados pelos arrays... portanto MAXRES deve ser assim:
//2048 - 578 == 1470
/* 
valor de MAXRES com 5 divisões de tempo
sensores byte int    float/long
1        294  147      73
2        144  72       36
3        98   49       25
4        72   36       18
5        58   29       14
com 4 divisões de tempo
sensores byte int    float/long
1        366 183      91 
2        182 91       45
3        122 61       30
4         90 45       22
5         72 36       18
com 3 divisões de tempo
sensores byte int    float/long
1        490 245     122
2        244 122      61
3        162 81       40
4        122 61       30
5        98  49       24
com 2 divisões de tempo
sensores byte int   float/long
1        734 367     183
2        366 183     91
3        244 122     61
4        182 91      45
5        146 73      36
*/


//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void loop()
{
unsigned long StartTime = millis();


// ************ isso deve ser deixado desta exata ordem, ou o touchscreen não funciona **************
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> daqui
//  digitalWrite(13, HIGH);                                          //>
  Point p = ts.getPoint();                                           //>
//  digitalWrite(13, LOW);                                           //>
  pinMode(XM, OUTPUT);                                               //>
  pinMode(YP, OUTPUT);                                               //>
// >>>>>>>>>>>>>>>>>VERIFICAÇÂO DO TOQUE<<<<<<<<<<<<<<<<<<           //>  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) // verifica se o toque está dentro da margem de erro 
  {                                                                  //>
   // turn from 0->1023 to tft.width                                 //>
   p.y = map(p.y, TS_MINX, TS_MAXX, H, 0);                           //>
   p.x = map(p.x, TS_MINY, TS_MAXY, W, 0);                           //>
   apertou(p.y,p.x); // evoca o evento de toque                      //>
  }                                                                  //>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ate aqui deve ser nessa ordem.


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
  resolucao = -1;  
  tft.setTextSize(2);
  tft.fillScreen(WHITE);
  tft.setCursor(10,10);
  tft.setTextColor(BLACK);
  tft.println(Get(Sensores));
  tft.println();
  tft.println();
  tft.print(Get(Leitura0));
  tft.setCursor(130,58);
  tft.println(Get(LeituraMedida0));
  tft.println();
  tft.print(Get(Leitura1));
  tft.setCursor(130,90);
  tft.println(Get(LeituraMedida1));
  tft.println();
  tft.print(Get(Leitura2));
  tft.setCursor(130,120);
  tft.print(Get(LeituraMedida2));
  tft.setTextSize(3);
  des = false;
  }
  if(perai==0)
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
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: TELA DO GRAFICO :::::::::::::::::::::::::::::
case 1:
if(des)
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
//_______________________________________________________// >>>>>>>>>>>>>>>>>ATUALIZAÇÂO DAS LEITURAS<<<<<<<<<<<<<<<<<<___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>()///  
// perai é uma variavel que permite que entre 20 loops apenas 1 seja usado para desenhas na tela
// ou seja, 19 loops podem receber eventos de toque, onde cada um leva poucos milesimos para rodar, e então
// é feito o grafico ou atualização dos valores, que leva muito tempo sem responder a eventos de toque
if(perai<20) 
perai++;
else       // apenas a cada 20 loops é feito a leitura dos valores
{
perai=0;
int Leitura;
//sizeof(Temperatura)/sizeof(Temperatura[0])
Leitura = analogRead(A0); // LEITURA DA TEMPERATURA
Leitura = map(Leitura, Get(Leituras,LESTRIP*0+0),Get(Leituras,LESTRIP*0+1),Get(Leituras,LESTRIP*0+2),Get(Leituras,LESTRIP*0+3));
Leitura = constrain(Leitura, Get(Leituras,LESTRIP*0+2),Get(Leituras,LESTRIP*0+3));
Lei[0] = Leitura;
Leitura = map(Leitura, Get(Leituras,LESTRIP*0+2),Get(Leituras,LESTRIP*0+3),YMIN,YMAX);  
Leitura = constrain(Leitura,YMIN,YMAX);
Leitura = YMAX-Leitura; 
empurrar(Temperatura,0,MAXRES,Leitura);  

Leitura = analogRead(A1);// LEITURA DA UMIDADE
Leitura = map(Leitura, Get(Leituras,LESTRIP*1+0),Get(Leituras,LESTRIP*1+1),Get(Leituras,LESTRIP*1+2),Get(Leituras,LESTRIP*1+3));
Leitura = constrain(Leitura, Get(Leituras,LESTRIP*1+2),Get(Leituras,LESTRIP*1+3));
Lei[1] = Leitura;
Leitura = map(Leitura, Get(Leituras,LESTRIP*1+2),Get(Leituras,LESTRIP*1+3),YMIN,YMAX);  
Leitura = constrain(Leitura,YMIN,YMAX);
Leitura = YMAX-Leitura; 
empurrar(Umidade,0,MAXRES,Leitura);  

Leitura = analogRead(A2);// LEITURA DA TURBIDEZ
Leitura = map(Leitura, Get(Leituras,LESTRIP*2+0),Get(Leituras,LESTRIP*2+1),Get(Leituras,LESTRIP*2+2),Get(Leituras,LESTRIP*2+3));
Leitura = constrain(Leitura, Get(Leituras,LESTRIP*2+2),Get(Leituras,LESTRIP*2+3));
Lei[2] = Leitura;
Leitura = map(Leitura, Get(Leituras,LESTRIP*2+2),Get(Leituras,LESTRIP*2+3),YMIN,YMAX);  
Leitura = constrain(Leitura,YMIN,YMAX);
Leitura = YMAX-Leitura; 
empurrar(Turbidez,0,MAXRES,Leitura);  

QuantasVezes++;
if(resolucao==0)
des=true;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>()///  

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
/*
if(QuantosSegundo>=10)
{
Quantos10Segundo++;
QuantosSegundo=0;
AtualizarTudo(MAXRES,MAXRES+10,2);
if(resolucao==2)
des=true;
if(Quantos10Segundo>=12)
{
Quantos2Minutos++;
Quantos10Segundo=0;
AtualizarTudo(MAXRES*2,MAXRES*2+12,3);
if(resolucao==3)
des=true;
if(Quantos2Minutos>=5)
{
Quantos10Minutos++;
Quantos2Minutos=0;
AtualizarTudo(MAXRES*3,MAXRES*3+10,4);
if(resolucao==4)
des=true;
if(Quantos10Minutos>=6)
{
QuantasHoras++;
Quantos10Minutos=0;
  
}
}
}
}
*/
}


 // Serial.print(F("freeMemory()="));
 //   Serial.println(freeMemory());
}
/*
void mem()
{
Serial.print(F(" memoria total 2048  "));    
Serial.print(F(" array :"));  
Serial.print(MAXRES*DiviDeTempo*NSENSORES);
int l =freeMemory();
Serial.print(F(" usada :"));  
Serial.print(2048-l);
Serial.print(F(" livre :"));
Serial.println(l); 
}
*/
//________________________________________________________AQUI TEMOS OS EVENTOS, COMO FUNÇÔES SEPARADAS          __________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

void apertou(int x,int y)//***********************>>>>>>>>>> QUANDO CLICAR EM ALGUM LUGAR DA TELA
{
 y = H-y;
for(byte i=0;i<6;i++) // passa por todos os botões
{
  if(Get(botoes,BSTRIP*i+5)==quetela&&x>Get(botoes,BSTRIP*i)&& y>Get(botoes,BSTRIP*i+1)&&x<Get(botoes,BSTRIP*i)+Get(botoes,BSTRIP*i+2)&&y<Get(botoes,BSTRIP*i+1)+Get(botoes,BSTRIP*i+3) )
  {
    // >>>>>>> ativar analizador de ações já que apertou em um botão
    //Serial.print("apertou em um botão  ");
    acao(Get(botoes,BSTRIP*i+4));  
  }
}
 //Serial.print("apertou em ");
// Serial.print(x);
// Serial.print(",");
// Serial.println(y);
}

void acao(byte q)//***********************>>>>>>>>>> QUANDO CLICAR EM ALGUM BOTÂO
{
  switch(q)
  {
    //++++++++++++++++++++++ quando clicar no botão voltar +++++++++++++++++++++++
    // que fica na tela dos graficos, volta a tela principal
  case 0 :
botao(0,true);  
quetela = 0;
des=true;
  break;
  //++++++++++++++++++++++ quando clicar no botão zoom out+++++++++++++++++++++++
  // que fica na tela dos graficos, aplica zoom out
  case 1:
botao(1,true);   
  tft.fillRect(0,0,W,200,WHITE);
  if(resolucao+1<=DiviDeTempo-1)
resolucao+=1;
desenharEscala(resolucao);
des=true;
botao(1,false); 
  break;
  //++++++++++++++++++++++ quando clicar no botão zoom in+++++++++++++++++++++++
  // que fica na tela dos graficos, aplica zoom in
  case 2:
botao(2,true);   
  tft.fillRect(0,0,W,200,WHITE);
  if(resolucao-1>=0)
resolucao-=1;
desenharEscala(resolucao);
des=true;
botao(2,false); 
  break;
  //++++++++++++++++++++++ quando clicar no botão da Temperatura+++++++++++++++++++++++
  // que fica na tela principal, vai para tela dos graficos mostrando o grafico da temperatura
  case 3:     
  resolucao = 0;
  tft.fillRect(0,200,W,40,MARROM);  
  quetela = 1;
  qual = 0;
tft.fillRect(0,0,W,200,WHITE);
 tft.fillRect(230,200,60,40,MARROM);
  tft.setTextSize(2); 
        tft.setCursor(230,210);
  tft.setTextColor(BLUE);
  tft.print(Get(reduzido0));
  desenharEscala(resolucao);
botao(0,false);
botao(1,false);
botao(2,false);
  break;
  //++++++++++++++++++++++ quando clicar no botão da Umidade+++++++++++++++++++++++
  // que fica na tela principal, vai para tela dos graficos mostrando o grafico da Umidade
    case 4: 
   resolucao = 0; 
  tft.fillRect(0,200,W,40,MARROM);  
  quetela = 1;
qual = 1;
tft.fillRect(0,0,W,200,WHITE);
 tft.fillRect(230,200,60,40,MARROM); 
   tft.setTextSize(2);
        tft.setCursor(230,210);
  tft.setTextColor(BLUE);
  tft.print(Get(reduzido1));
  desenharEscala(resolucao);
botao(0,false);
botao(1,false);
botao(2,false);
  break;
  //++++++++++++++++++++++ quando clicar no botão da Turbidez+++++++++++++++++++++++
  // que fica na tela principal, vai para tela dos graficos mostrando o grafico da Turbidez
    case 5:     
   resolucao = 0; 
  tft.fillRect(0,200,W,40,MARROM);  
  quetela = 1;
qual = 2;
tft.fillRect(0,0,W,200,WHITE);
 tft.fillRect(230,200,60,40,MARROM);
  tft.setTextSize(2); 
        tft.setCursor(230,210);
  tft.setTextColor(BLUE);
  tft.print(Get(reduzido2));
  desenharEscala(resolucao);
botao(0,false);
botao(1,false);
botao(2,false);
  break;
  }  
}
//________________________________________________________AQUI TEMOS MANIPULAÇÔES GRÀFICAS,                                 _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________


void botao(byte index,boolean estado)//*************************************** DESENHA O BOTÂO ESPECIFICADO 
{
//                x                            y                             w                             h
if(estado)
tft.fillRoundRect(Get(botoes,BSTRIP*index)    ,Get(botoes,BSTRIP*index+1)   ,Get(botoes,BSTRIP*index+2)   ,Get(botoes,BSTRIP*index+3)   ,10,GREY);
else
tft.fillRoundRect(Get(botoes,BSTRIP*index)    ,Get(botoes,BSTRIP*index+1)   ,Get(botoes,BSTRIP*index+2)   ,Get(botoes,BSTRIP*index+3)   ,10,GRAY);
tft.setTextSize(2);
tft.setCursor(Get(botoes,BSTRIP*index)+10 , Get(botoes,BSTRIP*index+1));
tft.setTextColor(BLACK);
tft.print((char)Get(botoes,BSTRIP*index+6));
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
   // byte y = map(grafico[(MAXRES*resolucao)+i] , Leituras[LESTRIP*qual+2],Leituras[LESTRIP*qual+3],YMIN,YMAX);
   // byte y2 =map(grafico[(MAXRES*resolucao)+i+1], Leituras[LESTRIP*qual+2],Leituras[LESTRIP*qual+3],YMIN,YMAX);
   // byte y3 =map(grafico[(MAXRES*resolucao)+i+2], Leituras[LESTRIP*qual+2],Leituras[LESTRIP*qual+3],YMIN,YMAX);
   // y=YMAX-y;
   // y2=YMAX-y2;
   // y3=YMAX-y3;   
    //delay(1);
   // temos aqui um bug onde o primeiro ou os dois primeiros pixels são desenhados com a cor anterior, 
   // aqui tentando fazer com que a cor anterior seja pintada por cima pela cor certa NÃO MUDE ISSO
   // daqui
//    tft.drawPixel(x,y2,WHITE);
//    tft.drawPixel(x,y2,WHITE);
//    tft.drawPixel(x,y2,WHITE);
 // tft.drawPixel(x,y2,WHITE);
    tft.drawLine(x ,y2,x2,y3,WHITE);
//  tft.drawLine(x ,y2,x2,y3,WHITE);
//    tft.drawPixel(x,y,RED);
//    tft.drawPixel(x,y,RED);
//    tft.drawPixel(x,y,RED);
//  tft.drawPixel(x,y,RED);
    tft.drawLine(x ,y ,x2,y2,RED);
//    if(resolucao >0)
//    tft.drawLine(x ,y ,x2,y2,RED);
   // até aqui só Deus sabe por que funciona

  }
  
}
/*
void GraficoRapido(int Leitura)
{
if(px<W) 
px++;
else
{
px=0;
}
Leitura= map(      Leitura, Leituras[LESTRIP*qual+2],Leituras[LESTRIP*qual+3],0,198);
Leitura= constrain(Leitura, 0,198);
Leitura=198-Leitura;
tft.drawVerticalLine(px,Leitura,5,RED);
}
*/
void desenharEscala(byte escala)
{ 
tft.setTextSize(1);
tft.setTextColor(BLACK);
  //        Medição
int inc = (Get(Leituras,LESTRIP*qual+3)-Get(Leituras,LESTRIP*qual+2))/Get(Leituras,LESTRIP*qual+4);
int inicial = Get(Leituras,LESTRIP*qual+2)+inc;
for(int i=Get(Leituras,LESTRIP*qual+4)-1;i>=0;i--)
{
tft.drawHorizontalLine(20,(200/Get(Leituras,LESTRIP*qual+4))*i,10,GREEN);
tft.setCursor(0,(200/Get(Leituras,LESTRIP*qual+4))*i);
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
