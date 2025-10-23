#include "TFTLCD7781.h"
#include "TouchScreen.h"

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
#define MARROM           0x79E0

TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define MINPRESSURE 10
#define MAXPRESSURE 5000

#define MAXRES 40 // a máxima resolução , ou seja, o tamanho do array

// Dimensoes da tela < talvez esteja invertido
int H=0; // altura
int W=0; // largura


 // *********************************  ARRAYS DE ESTRUTURAS ******************************
 
 
 // todos os botoes
 #define BSTRIP 6 // numero de elementos que definem um unico botao
byte botoes[] = {
// X  ,Y  ,Width,Height,acao,qualtela?

// botões da tela do grafico
  10  ,205 ,60  ,30    ,0   ,1, // ir pra tela principal
  80  ,205 ,60  ,30    ,1   ,1, // zoom out
  150 ,205 ,60  ,30    ,2   ,1, // zoom in
  // botões da tela principal
  0 ,55   ,150  ,30    ,3   ,0, // ir pra Temperatura
  0 ,85   ,150  ,30    ,4   ,0, // ir pra Umidade
  0 ,115  ,150  ,30    ,5   ,0  // ir pra Turbidez
};
// todas as leituras
#define LESTRIP 5 // numero de elementos que definem uma leitura
int Leituras[] = {
// LeituraMIN, LeituraMAX, Min   , Max   , Divporquantos 
   900       , 1100      ,-60    , 120   , 6           ,// Temperatura
    0        , 28        , 0     , 100   , 5           ,// Umidade
   20        , 80        , 0     , 1000  , 5            // Turbidez
};
// todas as Escalas
#define ESCSTRIP 2 // numero de elementos que definem uma leitura
int Escalas[] = {
// NColunas     , Colincrem,
    5           , 1        , // instantaneo
   (MAXRES-5)/5 , 5        , // passos de 1 seg
   (MAXRES-5)/6 , 1        , // passos de 10 seg
   (MAXRES-5)/5 , 10       , // passos de 2 MIN
   (MAXRES-5)/6 , 1          // passos de 10 MIN
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
void setup(void) {
  Serial.begin(9600);
  Serial.println("INICIANDO!");
  tft.reset();
  tft.initDisplay(); 
  tft.fillScreen(BLACK);
  W = tft.height();
  H = tft.width();
  telaInicial();
  delay(2000); 
  // exemplos dos codigos
  pinMode(13, OUTPUT); // n sei
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
}
// ********************** declarações secundárias de valores amplamente utilizados e variaveis *****************

unsigned int px=1;
int Leitura=0;
byte qual=0;
int perai=0;
unsigned long Mili=0;
int QuantasVezes=0;
int QuantosSegundo=0;
int Quantos10Segundo=0;
int Quantos2Minutos=0;
int Quantos10Minutos=0;
int QuantasHoras=0;
int quetela=0;
int resolucao=0;

boolean novo=true;


// aqui temos os arrays que receberão as leituras
int Temperatura [MAXRES*5] ={}; 
int Umidade     [MAXRES*5] ={};     
int Turbidez    [MAXRES*5] = {};  

//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void loop()
{
unsigned long StartTime = millis();
  digitalWrite(13, HIGH);
  Point p = ts.getPoint();
  digitalWrite(13, LOW);
  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
// >>>>>>>>>>>>>>>>>VERIFICAÇÂO DO TOQUE<<<<<<<<<<<<<<<<<<  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) // verifica se o toque está dentro da margem de erro 
  {
   // turn from 0->1023 to tft.width
   p.y = map(p.y, TS_MINX, TS_MAXX, H, 0); 
   p.x = map(p.x, TS_MINY, TS_MAXY, W, 0);
   apertou(p.y,p.x); // evoca o evento de toque
  }
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>()///  
// >>>>>>>>>>>>>>>>>ESCOLHA DAS TELAS<<<<<<<<<<<<<<<<<<
switch(quetela)
{
 case 0:
telaPrincipal();
break; 
case 1:
telaDoGrafico();
break;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>()///  
// perai é uma variavel que permite que entre 20 loops apenas 1 seja usado para desenhas na tela
// ou seja, 19 loops podem receber eventos de toque, onde cada um leva poucos milesimos para rodar, e então
// é feito o grafico ou atualização dos valores, que leva muito tempo sem responder a eventos de toque
    if(perai<20) 
perai++;
else
{
perai=0;
}
// >>>>>>>>>>>>>>>>>ATUALIZAÇÂO DAS LEITURAS<<<<<<<<<<<<<<<<<<
if(perai==0) // apenas a cada 20 loops é feito a manipulação grafica
{
  //sizeof(Temperatura)/sizeof(Temperatura[0])
Leitura = analogRead(A0); // LEITURA DA TEMPERATURA
Leitura = map(Leitura, Leituras[LESTRIP*0+0],Leituras[LESTRIP*0+1],Leituras[LESTRIP*0+2],Leituras[LESTRIP*0+3]);
Leitura = constrain(Leitura, Leituras[LESTRIP*0+2],Leituras[LESTRIP*0+3]);
empurrar(Temperatura,0,MAXRES);  

Leitura = analogRead(A1);// LEITURA DA UMIDADE
Leitura = map(Leitura, Leituras[LESTRIP*1+0],Leituras[LESTRIP*1+1],Leituras[LESTRIP*1+2],Leituras[LESTRIP*1+3]);
Leitura = constrain(Leitura, Leituras[LESTRIP*1+2],Leituras[LESTRIP*1+3]);
empurrar(Umidade,0,MAXRES);  

Leitura = analogRead(A2);// LEITURA DA TURBIDEZ
Leitura = map(Leitura, Leituras[LESTRIP*2+0],Leituras[LESTRIP*2+1],Leituras[LESTRIP*2+2],Leituras[LESTRIP*2+3]);
Leitura = constrain(Leitura, Leituras[LESTRIP*2+2],Leituras[LESTRIP*2+3]);
empurrar(Turbidez,0,MAXRES);  
QuantasVezes++;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>()///  

unsigned long CurrentTime = millis();
Mili += CurrentTime - StartTime;
if(Mili>=1000)
{
QuantosSegundo++;
Mili-=1000;
if(QuantasVezes>MAXRES)
QuantasVezes = MAXRES;
Leitura = media(Temperatura,0,QuantasVezes); // LEITURA DA TEMPERATURA
empurrar(Temperatura,MAXRES,MAXRES*2);  
Leitura = media(Umidade,0,QuantasVezes); // LEITURA DA Umidade
empurrar(Umidade,MAXRES,MAXRES*2);  
Leitura = media(Turbidez,0,QuantasVezes); // LEITURA DA Turbidez
empurrar(Turbidez,MAXRES,MAXRES*2);  
QuantasVezes=0;
}
if(QuantosSegundo>=10)
{
Quantos10Segundo++;
QuantosSegundo=0;
Leitura = media(Temperatura,0,10); // LEITURA DA TEMPERATURA
empurrar(Temperatura,MAXRES*2,MAXRES*3);  
Leitura = media(Umidade,0,10); // LEITURA DA Umidade
empurrar(Umidade,MAXRES*2,MAXRES*3);  
Leitura = media(Turbidez,0,10); // LEITURA DA Turbidez
empurrar(Turbidez,MAXRES*2,MAXRES*3);  
}
if(Quantos10Segundo>=12)
{
Quantos2Minutos++;
Quantos10Segundo=0;
Leitura = media(Temperatura,MAXRES*2,MAXRES*2+12); // LEITURA DA TEMPERATURA
empurrar(Temperatura,MAXRES*3,MAXRES*4);  
Leitura = media(Umidade,MAXRES*2,MAXRES*2+12); // LEITURA DA Umidade
empurrar(Umidade,MAXRES*3,MAXRES*4);  
Leitura = media(Turbidez,MAXRES*2,MAXRES*2+12); // LEITURA DA Turbidez
empurrar(Turbidez,MAXRES*3,MAXRES*4);  
}
if(Quantos2Minutos>=10)
{
Quantos10Minutos++;
Quantos2Minutos=0;
Leitura = media(Temperatura,MAXRES*3,MAXRES*3+10); // LEITURA DA TEMPERATURA
empurrar(Temperatura,MAXRES*4,MAXRES*5);  
Leitura = media(Umidade,MAXRES*3,MAXRES*3+10); // LEITURA DA Umidade
empurrar(Umidade,MAXRES*4,MAXRES*5);  
Leitura = media(Turbidez,MAXRES*3,MAXRES*3+10); // LEITURA DA Turbidez
empurrar(Turbidez,MAXRES*4,MAXRES*5);
}
if(Quantos10Minutos>=6)
{
QuantasHoras++;
Quantos10Minutos=0;
  
}
}
//________________________________________________________AQUI TEMOS AS TELAS,ORGANIZADAS COMO FUNÇÔES SEPARADAS___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

void telaInicial()//**************************************************************************** TELA INICIAL apenas aparece por 2 segundos no inicio
{
  tft.setRotation(1); 
  tft.setTextSize(4);
  tft.fillScreen(WHITE);
  tft.setCursor(20,80);
  tft.setTextColor(BLUE);
  tft.println("BEM VINDO");
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.println("       desenvolvido por ");
    tft.setTextColor(RED);
    tft.setTextSize(4);
  tft.println("  Erick Weil"); 
  tft.setTextSize(2); 
}

void telaPrincipal()//**************************************************************************** TELA PRINCIPAL
{
  if(novo)
  {
  tft.setTextSize(2);
  tft.fillScreen(WHITE);
  tft.setCursor(10,10);
  tft.setTextColor(BLACK);
  tft.println("Sensores");
  tft.println("");
  tft.println("");
  tft.print("Temperatura");
  tft.setCursor(130,58);
  tft.println("000  C");
    tft.println("");
  tft.print("Umidade");
    tft.setCursor(130,90);
  tft.println("000  %");
    tft.println("");
  tft.print("Turbidez");
    tft.setCursor(130,120);
  tft.println("000  NTU");
//  botao(3,false);
//  botao(4,false);
//  botao(5,false);
  novo = false;
    tft.setTextSize(3);
  }
  if(perai==0)
  {  
  tft.fillRect(130,58,50,20,WHITE);
  tft.setCursor(130,58);
  tft.println(Temperatura[0]);
  tft.fillRect(130,90,50,20,WHITE);
  tft.setCursor(130,90);
  tft.println(Umidade[0]);
  tft.fillRect(130,120,50,20,WHITE);
  tft.setCursor(130,120);
  tft.println(Turbidez[0]);
  delay(50);
  }
  
}
void telaDoGrafico()//**************************************************************************** TELA DO GRAFICO aqui teremos os graficos das leituras
{
 
  if(novo)
  {
 tft.fillScreen(BLACK);
  tft.fillRect(0,200,W,40,MARROM);  
  botao(0,true);
   botao(1,false);
    botao(2,false);
    qual = 0;
        tft.setCursor(230,210);
  tft.setTextColor(BLUE);
  tft.print("Temp");
    novo = false;
  }  
  

if(perai==0)
{

  switch(qual)//>>>>>>>>>>> VERIFICA QUAL GRAFICO DEVE SER DESENHADO
  {
  case 0 :
tft.fillRect(280,210,34,17,MARROM);
    tft.setCursor(280,210);
  tft.println(Temperatura[0]);
  desenharGrafico(Temperatura,resolucao,RED);
  break;
  case 1:
  tft.fillRect(280,210,34,17,MARROM);
    tft.setCursor(280,210);
  tft.println(Umidade[0]);
  desenharGrafico(Umidade,resolucao,RED);
  break;
  case 2:
    tft.fillRect(280,210,34,17,MARROM);
    tft.setCursor(280,210);
  tft.println(Turbidez[0]);
  desenharGrafico(Turbidez,resolucao,RED);
  break;
  }
}
}
//________________________________________________________AQUI TEMOS OS EVENTOS, COMO FUNÇÔES SEPARADAS          __________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________


void apertou(int y,int x)//***********************>>>>>>>>>> QUANDO CLICAR EM ALGUM LUGAR DA TELA
{
 y = H-y;
for(int i=0;i<6;i++) // passa por todos os botões
{
  if(botoes[BSTRIP*i+5]==quetela&&x>botoes[BSTRIP*i]&& y>botoes[BSTRIP*i+1]&&x<botoes[BSTRIP*i]+botoes[BSTRIP*i+2]&&y<botoes[BSTRIP*i+1]+botoes[BSTRIP*i+3] )
  {
    // >>>>>>> ativar analizador de ações já que apertou em um botão
    Serial.print("apertou em um botão  ");
    acao(botoes[BSTRIP*i+4]);  
  }
}
 Serial.print("apertou em ");
 Serial.print(x);
 Serial.print(",");
 Serial.println(y);
}
void acao(int q)//***********************>>>>>>>>>> QUANDO CLICAR EM ALGUM BOTÂO
{
  switch(q)
  {
    //++++++++++++++++++++++ quando clicar no botão voltar +++++++++++++++++++++++
    // que fica na tela dos graficos, volta a tela principal
  case 0 :
quetela = 0;
novo = true;
  break;
  //++++++++++++++++++++++ quando clicar no botão zoom out+++++++++++++++++++++++
  // que fica na tela dos graficos, aplica zoom out
  case 1:
  tft.fillRect(0,0,W,200,WHITE);
  if(resolucao+1<=4)
resolucao+=1;
desenharEscala(resolucao);
  break;
  //++++++++++++++++++++++ quando clicar no botão zoom in+++++++++++++++++++++++
  // que fica na tela dos graficos, aplica zoom in
  case 2:
  tft.fillRect(0,0,W,200,WHITE);
  if(resolucao-1>=0)
resolucao-=1;
desenharEscala(resolucao);
  break;
  //++++++++++++++++++++++ quando clicar no botão da Temperatura+++++++++++++++++++++++
  // que fica na tela principal, vai para tela dos graficos mostrando o grafico da temperatura
  case 3:
  tft.fillRect(0,200,W,40,MARROM);  
  quetela = 1;
  qual = 0;
botao(0,true);
botao(1,false);
botao(2,false);
tft.fillRect(0,0,W,200,WHITE);
 tft.fillRect(230,200,60,40,MARROM);
  tft.setTextSize(2); 
        tft.setCursor(230,210);
  tft.setTextColor(BLUE);
  tft.print("Temp");
  desenharEscala(resolucao);
  break;
  //++++++++++++++++++++++ quando clicar no botão da Umidade+++++++++++++++++++++++
  // que fica na tela principal, vai para tela dos graficos mostrando o grafico da Umidade
    case 4:
  tft.fillRect(0,200,W,40,MARROM);  
  quetela = 1;
  botao(0,false);
botao(1,true);
botao(2,false);
qual = 1;
tft.fillRect(0,0,W,200,WHITE);
 tft.fillRect(230,200,60,40,MARROM); 
   tft.setTextSize(2);
        tft.setCursor(230,210);
  tft.setTextColor(BLUE);
  tft.print("Umid");
  desenharEscala(resolucao);
  break;
  //++++++++++++++++++++++ quando clicar no botão da Turbidez+++++++++++++++++++++++
  // que fica na tela principal, vai para tela dos graficos mostrando o grafico da Turbidez
    case 5:
  tft.fillRect(0,200,W,40,MARROM);  
  quetela = 1;
botao(0,false);
botao(1,false);
botao(2,true);
qual = 2;
tft.fillRect(0,0,W,200,WHITE);
 tft.fillRect(230,200,60,40,MARROM);
  tft.setTextSize(2); 
        tft.setCursor(230,210);
  tft.setTextColor(BLUE);
  tft.print("Turb");
  desenharEscala(resolucao);
  break;
  }  
}
//________________________________________________________AQUI TEMOS MANIPULAÇÔES GRÀFICAS,                                 _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________


void botao(int index,boolean apertado)//*************************************** DESENHA O BOTÂO ESPECIFICADO 
{
if(apertado)  
tft.fillRoundRect(botoes[BSTRIP*index]    ,botoes[BSTRIP*index+1]   ,botoes[BSTRIP*index+2]   ,botoes[BSTRIP*index+3]   ,10,GREY);
else
tft.fillRoundRect(botoes[BSTRIP*index]    ,botoes[BSTRIP*index+1]   ,botoes[BSTRIP*index+2]   ,botoes[BSTRIP*index+3]   ,10,GRAY);
//tft.fillRoundRect(botoes[BSTRIP*index+1] +6 ,botoes[BSTRIP*index] +6,botoes[BSTRIP*index+3]-12,botoes[BSTRIP*index+2]-12,10,GRAY);
}
void desenharGrafico(int grafico[],int d,uint16_t cor)//************************ DESENHA UM GRAFICO DE ACORDO COM O ARRAY DADO
{
  int larg = W/(MAXRES-5);
  for(int i=MAXRES-5;i>0;i--)
  {
    unsigned int x = W-larg*i;
    unsigned int x2 = W-larg*(i+1);
    int y =grafico[(MAXRES*d)+i] ;
    int y2 =grafico[(MAXRES*d)+i+1];
    int y3 =grafico[(MAXRES*d)+i+2];
y = map(      y, Leituras[LESTRIP*qual+2],Leituras[LESTRIP*qual+3],0,198);
y = constrain(y, 0,198);
y2 = map(      y2, Leituras[LESTRIP*qual+2],Leituras[LESTRIP*qual+3],0,198);
y2 = constrain(y2, 0,198);
y3 = map(      y3, Leituras[LESTRIP*qual+2],Leituras[LESTRIP*qual+3],0,198);
y3 = constrain(y3, 0,198);
    y=198-y;
    y2=198-y2;
    y3=198-y3;
    //delay(1);
   // temos aqui um bug onde é necessario desenhar essas linhas duas vezes    NÃO MUDE ISSO
   // daqui
    tft.drawLine(x ,y2,x2,y3,WHITE);
    tft.drawLine(x ,y2,x2,y3,WHITE);
    tft.drawLine(x,y,x2,y2,cor);
   // até aqui só Deus sabe por que funciona

  }
}
void desenharEscala(int escala)
{
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
//   tft.drawHorizontalLine(X,Y,Width,Cor);
//   tft.drawVerticalLine  (X,Y,Height,Cor);
//  Escalas[ESCSTRIP];
int inc=Escalas[ESCSTRIP*escala+1];

//          tempo
for(int i=Escalas[ESCSTRIP*escala]-1;i>=0;i--)
{
tft.drawVerticalLine((W/Escalas[ESCSTRIP*escala])*i,170,10,GREEN);
tft.setCursor((W/Escalas[ESCSTRIP*escala])*i,H-55);
tft.println(inc);
inc+=Escalas[ESCSTRIP*escala+1];
}
//        Medição
//Leituras[LESTRIP*0+2],Leituras[LESTRIP*0+3];
inc = (Leituras[LESTRIP*qual+3]-Leituras[LESTRIP*qual+2])/Leituras[LESTRIP*qual+4];
int inicial = Leituras[LESTRIP*qual+2]+inc;
for(int i=Leituras[LESTRIP*qual+4]-1;i>=0;i--)
{
tft.drawHorizontalLine(20,(200/Leituras[LESTRIP*qual+4])*i,10,GREEN);
tft.setCursor(0,(200/Leituras[LESTRIP*qual+4])*i);
tft.println(inicial);
inicial += inc;
}
}
//________________________________________________________AQUI TEMOS OUTRAS FUNÇÔES                                         _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

void empurrar(int graf[],int de,int ate) // todos os elementos do array graf é empurrado em 1, e o index "de" recebe a Leitura, "ate" foi empurrado para fora
{
 unsigned int temp=0;
 unsigned int temp2=Leitura;
  for(unsigned int i=de;i<ate-2;i+=2)
  {
  temp = graf[i];
  graf[i] = temp2;
  temp2=graf[i+1];
  graf[i+1] = temp;
  } 
}
int media(int graf[],int de,int ate)
{
int retorno=0;
int q=0;
  for(unsigned int i=de;i<ate;i++)
  {
  retorno += graf[i];
  q++;
  } 
return retorno/(q);  
}

