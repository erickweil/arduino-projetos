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
TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define MINPRESSURE 10
#define MAXPRESSURE 5000

// Dimensoes da tela < talvez esteja invertido
int H=0; // altura
int W=0; // largura
// ********************************** VALORES GUARDADOS NA MEMORIA FLASH ******************
//#define MAX_STRING 30
//char stringBuffer[MAX_STRING];
//const char BemVindo[] PROGMEM =         "Bem Vindo";
// ....

 // *********************************  ARRAYS DE ESTRUTURAS NA MEMORIA FLASH ******************************
#define POS_X 15
#define POS_Y 240/5
#define D 240/5
#define BSTRIP 5 // numero de elementos que definem um unico botao
#define NBotoes 22
const byte botoes[] PROGMEM = {
// X         ,Y           ,Width ,Height ,caractere

  POS_X+D*0  ,POS_Y+D*0   ,D     ,D      ,'7'  ,
  POS_X+D*1  ,POS_Y+D*0   ,D     ,D      ,'8'  ,
  POS_X+D*2  ,POS_Y+D*0   ,D     ,D      ,'9'  ,
  POS_X+D*3  ,POS_Y+D*0   ,D     ,D      ,'/'  ,
  POS_X+D*4  ,POS_Y+D*0   ,D     ,D      ,'('  ,
  POS_X+D*5  ,POS_Y+D*0   ,D     ,D      ,')'  ,

  POS_X+D*0  ,POS_Y+D*1   ,D     ,D      ,'4'  ,
  POS_X+D*1  ,POS_Y+D*1   ,D     ,D      ,'5'  ,
  POS_X+D*2  ,POS_Y+D*1   ,D     ,D      ,'6'  ,
  POS_X+D*3  ,POS_Y+D*1   ,D     ,D      ,'*'  ,
  POS_X+D*4  ,POS_Y+D*1   ,D     ,D      ,'^'  ,
  POS_X+D*5  ,POS_Y+D*1   ,D     ,D      ,'R'  ,
    
  POS_X+D*0  ,POS_Y+D*2   ,D     ,D      ,'1'  ,
  POS_X+D*1  ,POS_Y+D*2   ,D     ,D      ,'2'  ,
  POS_X+D*2  ,POS_Y+D*2   ,D     ,D      ,'3'  ,
  POS_X+D*3  ,POS_Y+D*2   ,D     ,D      ,'-'  ,
  POS_X+D*4  ,POS_Y+D*2   ,D     ,D      ,'<'  ,
  POS_X+D*5  ,POS_Y+D*2   ,D     ,D*2    ,'='  ,

  POS_X+D*0  ,POS_Y+D*3   ,D*2   ,D      ,'0'  ,
  //      1
  POS_X+D*2  ,POS_Y+D*3   ,D     ,D      ,'.'  ,
  POS_X+D*3  ,POS_Y+D*3   ,D     ,D      ,'+'  ,
  POS_X+D*4  ,POS_Y+D*3   ,D     ,D      ,'C'  
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
// programa que calcula uma expressão numérica
String Texto="";
byte umbyte;
byte at;
void setup(void) 
{
//pinMode(led,OUTPUT);//inicia o led
//Serial.begin(9600);// permite a leitura da porta serial a 9600 bits/s
  tft.reset();
  tft.initDisplay(); 
  W = tft.height();
  H = tft.width();
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  tft.fillScreen(WHITE);  
  for(byte i=0;i<NBotoes;i++)
  botao(i,false);
  atualizarTexto(true);
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
}

//________________________________________________________AQUI TEMOS OS EVENTOS, COMO FUNÇÔES SEPARADAS          __________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void atualizarTexto(boolean tudomesmo)
{
if(Texto.length()<=15)
tft.setTextSize(3);  
else if(Texto.length()<=23)
tft.setTextSize(2); 
else
tft.setTextSize(1);
if(tudomesmo||Texto.length()==16||Texto.length()==24)
{
tft.fillRoundRect(0 ,0,W,D,5,WHITE);   
tft.drawRoundRect(0 ,0,W,D,5,BLACK);   
}

tft.setCursor(POS_X +10,10+5);
tft.println(Texto);
}
void apertou(int y,int x)//***********************>>>>>>>>>> QUANDO CLICAR EM ALGUM LUGAR DA TELA
{
y = H-y;
for(byte i=0;i<NBotoes;i++) // passa por todos os botões
{
  if(x>Get(botoes,BSTRIP*i)&& y>Get(botoes,BSTRIP*i+1)&&x<Get(botoes,BSTRIP*i)+Get(botoes,BSTRIP*i+2)&&y<Get(botoes,BSTRIP*i+1)+Get(botoes,BSTRIP*i+3) )
  {
    // >>>>>>> ativar analizador de ações já que apertou em um botão
   botao(i,true);
   acao((char)Get(botoes,BSTRIP*i+4)); 
   delay(120);
   botao(i,false); 
  }
}
}

void acao(char queacao)
{
  float numero;
  char charVal[20];
  switch(queacao)
  {
    case '=':
    numero = fazerconta();
    at=0;
    dtostrf(numero, 1, res(numero-(int)numero), charVal);
    Texto="";
    do
    {  
    Texto+=charVal[at++]; 
    }
    while(charVal[at]!='\0');
    at=0;
    atualizarTexto(true);
    break;
    case 'C':
    Texto = "";
    at=0;
    atualizarTexto(true);
    break;
    case '<':
    Texto = Texto.substring(0, Texto.length() - 1);
    atualizarTexto(true);
    break;
    default:
    if(Texto.length()<80)
    Texto+= queacao;
    atualizarTexto(false);
    break;
  }
}
void botao(byte index,boolean estado)
{
//                x                            y                             w                             h

if(estado)
tft.fillRoundRect(Get(botoes,BSTRIP*index)    ,Get(botoes,BSTRIP*index+1)   ,Get(botoes,BSTRIP*index+2)   ,Get(botoes,BSTRIP*index+3)   ,5,AZULADO);
else
tft.fillRoundRect(Get(botoes,BSTRIP*index)    ,Get(botoes,BSTRIP*index+1)   ,Get(botoes,BSTRIP*index+2)   ,Get(botoes,BSTRIP*index+3)   ,5,GREY);
tft.drawRoundRect(Get(botoes,BSTRIP*index)    ,Get(botoes,BSTRIP*index+1)   ,Get(botoes,BSTRIP*index+2)   ,Get(botoes,BSTRIP*index+3)   ,5,BLACK);
tft.setTextSize(3);
tft.setCursor(Get(botoes,BSTRIP*index)+10 , Get(botoes,BSTRIP*index+1)+5);
tft.setTextColor(BLACK);
tft.print((char)Get(botoes,BSTRIP*index+4));

}
//________________________________________________________AQUI TEMOS ACESSOS A MEMORIA FLASH                                _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

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

// funcao que retorna o resultado da expressao que esta esperando na porta serial
float fazerconta()
{
float res=0;
//float t=0;
res = proxnum();// chama o proximo numero que esta esperando na serial
while(Texto.length()-at> 0) // enquanto houver mais bytes de entrada
{
//  while(umbyte == ' ')
//  umbyte =Serialread();
  switch(umbyte)
  {
  case ')': 
  umbyte=Texto.charAt(at++); 
  return res; // retorna o resultado se encontrar o fim de parenteses 
  break;
  case '+': // caso encontrar o sinal de soma executar a soma
    res += proxnum(); // outra possibilidade sem nenhum print de estado
  break;
  case '-': // caso encontrar o sinal de subtração executa a subtração
    res -= proxnum(); 
  break;
  case '*': // caso encontrar o sinal de vezes, faz a multiplicação
    res *= proxnum(); 
  break;
  case '/':// caso encontrar o sinal de divisão faz a divisão 
    res /= proxnum(); 
  break;
  case '^': // caso encontrar esse sinal faz a potenciação  
    res = pow(res,proxnum()); 
  break;
  case 'R': // caso encontrar esse sinal faz a raiz quadrada  
    res = sqrt(proxnum()); 
  break;
  case '\0': // caso encontrar esse sinal faz a raiz quadrada  
  return res;
  break;
  default :
  return -1;
//umbyte=Texto.charAt(at++); 
  break;
  }
}
return res; // retorna o resultado
}

// Funçao que recebe o proximo numero disponível
float proxnum()
{  
umbyte =Texto.charAt(at++); // Lê o prox byte na porta serial
String num="";
if(umbyte == '(') 
return(fazerconta()); // examina se encontrou um inicio de parenteses, e se sim, retorna o valor da conta dentro desse parenteses
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
return(atof(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atof é abreviatura de "ASCII to Float" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atof) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um float
*/
}
byte res(float d)
{
   if(d != 0)
  {
  if(d > 0.1||d <= -0.1)  
  return 2;
  else if(d >= 0.01||d <= -0.01)  
  return 3; 
  else if(d >= 0.001||d <= -0.001)  
  return 4; 
  else if(d >= 0.0001||d <= -0.0001)  
  return 5; 
  else if(d >= 0.00001||d <= -0.00001)  
  return 6; 
  else if(d >= 0.000001||d <= -0.000001)  
  return 8;   
  }
  else
  {
  return 0;  
  } 
}
