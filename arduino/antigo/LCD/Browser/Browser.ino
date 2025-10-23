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

#define CHARSLINE 26
//________________________________________________________AQUI TEMOS A INICIALIZAÇAO,                           ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
int led = 13; //Led a servir como sinalizador de estado


uint16_t font_posX=0;
uint16_t font_posY=5;
uint16_t font_color = BLACK;
uint8_t font_size = 2;

//String texto=""; //variável a receber o texto de entrada
void setup()
{
pinMode(led,OUTPUT);//inicia o led
  tft.reset();
  tft.initDisplay(); 
  //********************************************** consertando problema onde o primeiro pixel é desenhado com a cor anterior ***********************************************************
  /*
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
    if(data==0x52AA)
    {
      break;
    }
    else
    {
      tft.initDisplay(); 
      tft.fillScreen(RED);
    }
  }
  */
  
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  tft.fillScreen(BLACK);
  tft.fillScreen(WHITE);
  //tft.setCursor(0,5);
  //tft.setTextSize(2);
  //tft.setTextColor(BLACK);
  //tft.println(F(" Programador : Erick L Weil"));
  //tft.println("012345678901234567890123456789");
  Serial.begin(9600);// permite a leitura da porta serial a 300 bits/s lento assim pra poder escrever enquanto recebe
  Serial.println(F("Iniciou!"));
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
   tft.fillScreen(WHITE);
   tft.drawPixel(x,y,BLACK); 
  }
  
  if(Serial.available()>0)// examina se há algo na entrada serial
  { 
  //delay(200);//esperarfluxo(); // espera até que a entrada serial receba todos os bytes de entrada
  digitalWrite(led,HIGH);  //liga o led
  //Serial.print("analisarei ");  
  //int caracteres = (int)proxnum(); 
  //Serial.print(caracteres);
  //Serial.print(" caracteres"); // faz uma linha que diz qual o tamanho da linha a ser analisada
  
  //Serial.println();
  //Serial.print("foi escrito : \'");
  
  tft.fillScreen(WHITE);
  //tft.setCursor(0,5);
  //tft.setTextSize(2);
  //tft.setTextColor(BLACK);
  font_posX = 0;
  font_posY = 5;
  
  lerlinha(); // chama a função que converte cada byte de entra em um char e o concatena a string 'texto'
  //Serial.println("\'");
  Serial.print(F("Free RAM: "));
  Serial.println(freeRam());
  digitalWrite(led,LOW);  // apaga o led, mostra que terminou de examinar a entrada
  }
  
}

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int charsRead = 0;
unsigned int c='?';
char c0;
char c1;
char c2;
char c3;
char c4;
char c5;
boolean fechando=false;
unsigned int lastc='?';
void lerlinha()
{
  while(c!='\0') // enquanto houver mais bytes de entrada
  {
    lastc=c;
    c = readASCII(); 
    charsRead++;
    switch(c)
    {
      // CODE HTML
      case '&':
        c0 = (char)readASCII();
        if(c0 != ';')
        {
          c1 = (char)readASCII();
          if(c1 != ';')
          {
            c2 = (char)readASCII();
            if(c2 != ';')
            {
              c3 = (char)readASCII();
              if(c3 != ';')
              {
                c4 = (char)readASCII();
                if(c4 != ';')
                {
                  c5 = (char)readASCII();
                  if(c5!=';')
                  {
                     skipASCIIUntil(';'); // pula os caracteres ate o fim
                  }
                }
              }
            }
          }
        }
        c = parseCODE(c0,c1,c2,c3,c4,c5);
        PrintChar(c);
      break;
      //TAG HTML
      case '<':
        c = lastc; // para nao acumular espaços seguidos
        c0 = (char)readASCII();
        if(c0 == '/')
        {
          fechando = true;
          c0 = (char)readASCII();
        }
        else
        {
          fechando = false;
        }
        if(c0 != '>')
        {
          c1 = (char)readASCII();
          if(c1 != '>')
          {
            c2 = (char)readASCII();
            if(c2 != '>')
            {
              c3 = (char)readASCII();
              if(c3 != '>')
              {
                c4 = (char)readASCII();
                if(c4 != '>')
                {
                  c5 = (char)readASCII();
                  if(c5!='>')
                  {
                     skipASCIIUntil('>'); // pula os caracteres ate o fim
                  }
                }
              }
            }
          }
        }
        parseHTML(c0,c1,c2,c3,c4,c5,fechando);
        
      break;
      // Texto Normal
      default:
        if(c<127 && !isGraph(c))
        {
          if(!isGraph(lastc))
          {
            // espaços seguidos nao sao escritos
          }
          else
          {
            PrintChar(' ');  
          }
        }
        else
        {
          PrintChar(c);
        }
      break;
    }
  }
  c = '?'; // para que na proxima nao fique sendo '\0';
  //tft.print(tex);
  //tft.println(F("."));
  //Serial.println(tex);  // dá um feedback de quantos caracteres foram analisados
  //tex = "";
}


void Println()
{
  font_posX = 0;
  font_posY += font_size*8;
}
void PrintChar(unsigned int c)
{
  DrawChar(c);
  font_posX += font_size*6;
  if(font_posX + font_size*6 > W)
  {
    Println();
  }
}
void DrawChar(unsigned int c,int tamanho)
{
int prevtamanho = font_size;
font_size = tamanho;
DrawChar(c);
font_size = prevtamanho;
}
void DrawChar(unsigned int c)
{
switch(c)
{
// Acentos
case 180:tft.drawLine(font_posX+font_size, font_posY+font_size,  font_posX+font_size*3, font_posY-font_size, font_color);break; // '´' agudo
case '`':tft.drawLine(font_posX+font_size*3, font_posY+font_size,  font_posX+font_size, font_posY-font_size, font_color);break;
case '^':tft.drawChar( font_posX, font_posY-font_size*2, (char)c, font_color, font_size );break;
case '~':tft.drawChar( font_posX, font_posY-font_size*2, (char)c, font_color, font_size );break;
case 176: case 186:tft.drawChar( font_posX+font_size, font_posY-font_size*2, 'o', font_color, 1 ); break; // '°'
case 168: // '¨'
tft.drawPixel( font_posX+font_size, font_posY+font_size, font_color);
tft.drawPixel( font_posX+font_size*3, font_posY+font_size, font_color);
break;

// Especiais
case 161:	//TODO action Case ¡ --> '¡' cod:161 inverted exclamation mark
	DrawChar('i');
break;
case 162:	//TODO action Case ¢ --> '¢' cod:162 cent sign
	DrawCompose('c','|');
break;
case 163:	//TODO action Case £ --> '£' cod:163 pound sign
	DrawCompose('-','L');
break;
case 164:	//TODO action Case ¤ --> '¤' cod:164 currency sign
	DrawCompose('X','O');
break;
case 165:	//TODO action Case ¥ --> '¥' cod:165 yen sign
	DrawCompose('=','Y');
break;
case 166:	//TODO action Case ¦ --> '¦' cod:166 broken vertical bar
	DrawChar('|');
break;
case 167:	//TODO action Case § --> '§' cod:167 section sign
	DrawCompose('\\','S');
break;
case 169:	//TODO action Case © --> '©' cod:169 copyright sign
	DrawCompose('O','c');
break;
case 170:	//TODO action Case ª --> 'ª' cod:170 feminine ordinal indicator
	DrawChar('a',1);
break;
case 171:	//TODO action Case « --> '«' cod:171 left double angle quotes
	DrawChar('"');
break;
case 8364:	//TODO action Case € --> '€' cod:8364 euro
	DrawCompose('C','=');
break;
case 172:	//TODO action Case ¬ --> '¬' cod:172 not sign
	DrawCompose('-',',');
break;
case 174:	//TODO action Case ® --> '®' cod:174 registered trade mark sign
	DrawCompose('O','r');
break;
case 175:	//TODO action Case ¯ --> '¯' cod:175 spacing macron - overline
	DrawChar('_');
break;
case 177:	//TODO action Case ± --> '±' cod:177 plus-or-minus sign
	DrawCompose('+','-');
break;
case 178:	//TODO action Case ² --> '²' cod:178 superscript two - squared
	DrawChar('2',1);
break;
case 179:	//TODO action Case ³ --> '³' cod:179 superscript three - cubed
	DrawChar('3',1);
break;
case 181:	//TODO action Case µ --> 'µ' cod:181 micro sign
	DrawCompose('|','u');
break;
case 182:	//TODO action Case ¶ --> '¶' cod:182 pilcrow sign - paragraph sign
	DrawCompose('P','|');
break;
case 183:	//TODO action Case · --> '·' cod:183 middle dot - Georgian comma
	DrawChar('.');
break;
case 184:	//TODO action Case ¸ --> '¸' cod:184 spacing cedilla
	DrawChar(',');
break;
case 185:	//TODO action Case ¹ --> '¹' cod:185 superscript one
	DrawChar('1',1);
break;
case 187:	//TODO action Case » --> '»' cod:187 right double angle quotes
	DrawChar('"');
break;
case 188:	//TODO action Case ¼ --> '¼' cod:188 fraction one quarter
	DrawChar('1',1);PrintChar('/');DrawChar('4',1);
break;
case 189:	//TODO action Case ½ --> '½' cod:189 fraction one half
	DrawChar('1',1);PrintChar('/');DrawChar('2',1);
break;
case 190:	//TODO action Case ¾ --> '¾' cod:190 fraction three quarters
	DrawChar('3',1);PrintChar('/');DrawChar('4',1);
break;
case 191:	//TODO action Case ¿ --> '¿' cod:191 inverted question mark
	DrawChar('?');
break;
case 192:	//TODO action Case À --> 'À' cod:192 latin capital letter A with grave
	DrawCompose('`','A');
break;
case 193:	//TODO action Case Á --> 'Á' cod:193 latin capital letter A with acute
	DrawCompose(180,'A');
break;
case 194:	//TODO action Case Â --> 'Â' cod:194 latin capital letter A with circumflex
	DrawCompose('^','A');
break;
case 195:	//TODO action Case Ã --> 'Ã' cod:195 latin capital letter A with tilde
	DrawCompose('~','A');
break;
case 196:	//TODO action Case Ä --> 'Ä' cod:196 latin capital letter A with diaeresis
	DrawCompose(168,'A');
break;
case 197:	//TODO action Case Å --> 'Å' cod:197 latin capital letter A with ring above
	DrawCompose(186,'A');
break;
case 198:	//TODO action Case Æ --> 'Æ' cod:198 latin capital letter AE
	DrawCompose('A','E');
break;
case 199:	//TODO action Case Ç --> 'Ç' cod:199 latin capital letter C with cedilla
	DrawCompose(',','C');
break;
case 200:	//TODO action Case È --> 'È' cod:200 latin capital letter E with grave
	DrawCompose('`','E');
break;
case 201:	//TODO action Case É --> 'É' cod:201 latin capital letter E with acute
	DrawCompose(180,'E');
break;
case 202:	//TODO action Case Ê --> 'Ê' cod:202 latin capital letter E with circumflex
	DrawCompose('^','E');
break;
case 203:	//TODO action Case Ë --> 'Ë' cod:203 latin capital letter E with diaeresis
	DrawCompose(168,'E');
break;
case 204:	//TODO action Case Ì --> 'Ì' cod:204 latin capital letter I with grave
	DrawCompose('`','I');
break;
case 205:	//TODO action Case Í --> 'Í' cod:205 latin capital letter I with acute
	DrawCompose(180,'I');
break;
case 206:	//TODO action Case Î --> 'Î' cod:206 latin capital letter I with circumflex
	DrawCompose('^','I');
break;
case 207:	//TODO action Case Ï --> 'Ï' cod:207 latin capital letter I with diaeresis
	DrawCompose(168,'I');
break;
case 208:	//TODO action Case Ð --> 'Ð' cod:208 latin capital letter ETH
	DrawCompose('-','D');
break;
case 209:	//TODO action Case Ñ --> 'Ñ' cod:209 latin capital letter N with tilde
	DrawCompose('~','N');
break;
case 210:	//TODO action Case Ò --> 'Ò' cod:210 latin capital letter O with grave
	DrawCompose('`','O');
break;
case 211:	//TODO action Case Ó --> 'Ó' cod:211 latin capital letter O with acute
	DrawCompose(180,'O');
break;
case 212:	//TODO action Case Ô --> 'Ô' cod:212 latin capital letter O with circumflex
	DrawCompose('^','O');
break;
case 213:	//TODO action Case Õ --> 'Õ' cod:213 latin capital letter O with tilde
	DrawCompose('~','O');
break;
case 214:	//TODO action Case Ö --> 'Ö' cod:214 latin capital letter O with diaeresis
	DrawCompose(168,'O');
break;
case 215:	//TODO action Case × --> '×' cod:215 multiplication sign
	DrawChar('*');
break;
case 216:	//TODO action Case Ø --> 'Ø' cod:216 latin capital letter O with slash
	DrawCompose('/','O');
break;
case 217:	//TODO action Case Ù --> 'Ù' cod:217 latin capital letter U with grave
	DrawCompose('`','U');
break;
case 218:	//TODO action Case Ú --> 'Ú' cod:218 latin capital letter U with acute
	DrawCompose(180,'U');
break;
case 219:	//TODO action Case Û --> 'Û' cod:219 latin capital letter U with circumflex
	DrawCompose('^','U');
break;
case 220:	//TODO action Case Ü --> 'Ü' cod:220 latin capital letter U with diaeresis
	DrawCompose(168,'U');
break;
case 221:	//TODO action Case Ý --> 'Ý' cod:221 latin capital letter Y with acute
	DrawCompose(180,'Y');
break;
case 222:	//TODO action Case Þ --> 'Þ' cod:222 latin capital letter THORN
	DrawCompose('|','o');
break;
case 223:	//TODO action Case ß --> 'ß' cod:223 latin small letter sharp s - ess-zed
	DrawCompose('l','3');
break;
case 224:	//TODO action Case à --> 'à' cod:224 latin small letter a with grave
	DrawCompose('`','a');
break;
case 225:	//TODO action Case á --> 'á' cod:225 latin small letter a with acute
	DrawCompose(180,'a');
break;
case 226:	//TODO action Case â --> 'â' cod:226 latin small letter a with circumflex
	DrawCompose('^','a');
break;
case 227:	//TODO action Case ã --> 'ã' cod:227 latin small letter a with tilde
	DrawCompose('~','a');
break;
case 228:	//TODO action Case ä --> 'ä' cod:228 latin small letter a with diaeresis
	DrawCompose(168,'a');
break;
case 229:	//TODO action Case å --> 'å' cod:229 latin small letter a with ring above
	DrawCompose(186,'a');
break;
case 230:	//TODO action Case æ --> 'æ' cod:230 latin small letter ae
	DrawCompose('a','e');
break;
case 231:	//TODO action Case ç --> 'ç' cod:231 latin small letter c with cedilla
	DrawCompose(',','c');
break;
case 232:	//TODO action Case è --> 'è' cod:232 latin small letter e with grave
	DrawCompose('`','e');
break;
case 233:	//TODO action Case é --> 'é' cod:233 latin small letter e with acute
	DrawCompose(180,'e');
break;
case 234:	//TODO action Case ê --> 'ê' cod:234 latin small letter e with circumflex
	DrawCompose('^','e');
break;
case 235:	//TODO action Case ë --> 'ë' cod:235 latin small letter e with diaeresis
	DrawCompose(168,'e');
break;
case 236:	//TODO action Case ì --> 'ì' cod:236 latin small letter i with grave
	DrawCompose('`','i');
break;
case 237:	//TODO action Case í --> 'í' cod:237 latin small letter i with acute
	DrawCompose(180,'i');
break;
case 238:	//TODO action Case î --> 'î' cod:238 latin small letter i with circumflex
	DrawCompose('^','i');
break;
case 239:	//TODO action Case ï --> 'ï' cod:239 latin small letter i with diaeresis
	DrawCompose(168,'i');
break;
case 240:	//TODO action Case ð --> 'ð' cod:240 latin small letter eth
	DrawCompose('~','o');
break;
case 241:	//TODO action Case ñ --> 'ñ' cod:241 latin small letter n with tilde
	DrawCompose('~','n');
break;
case 242:	//TODO action Case ò --> 'ò' cod:242 latin small letter o with grave
	DrawCompose('`','o');
break;
case 243:	//TODO action Case ó --> 'ó' cod:243 latin small letter o with acute
	DrawCompose(180,'o');
break;
case 244:	//TODO action Case ô --> 'ô' cod:244 latin small letter o with circumflex
	DrawCompose('^','o');
break;
case 245:	//TODO action Case õ --> 'õ' cod:245 latin small letter o with tilde
	DrawCompose('~','o');
break;
case 246:	//TODO action Case ö --> 'ö' cod:246 latin small letter o with diaeresis
	DrawCompose(168,'o');
break;
case 247:	//TODO action Case ÷ --> '÷' cod:247 division sign
	DrawCompose('-',':');
break;
case 248:	//TODO action Case ø --> 'ø' cod:248 latin small letter o with slash
	DrawCompose('/','o');
break;
case 249:	//TODO action Case ù --> 'ù' cod:249 latin small letter u with grave
	DrawCompose('`','u');
break;
case 250:	//TODO action Case ú --> 'ú' cod:250 latin small letter u with acute
	DrawCompose(180,'u');
break;
case 251:	//TODO action Case û --> 'û' cod:251 latin small letter u with circumflex
	DrawCompose('^','u');
break;
case 252:	//TODO action Case ü --> 'ü' cod:252 latin small letter u with diaeresis
	DrawCompose(168,'u');
break;
case 253:	//TODO action Case ý --> 'ý' cod:253 latin small letter y with acute
	DrawCompose(180,'y');
break;
case 254:	//TODO action Case þ --> 'þ' cod:254 latin small letter thorn
	DrawCompose('|','o');
break;
case 255:	//TODO action Case ÿ --> 'ÿ' cod:255 latin small letter y with diaeresis
	DrawCompose(168,'y');
break;
default:
  tft.drawChar( font_posX, font_posY, (char)c, font_color, font_size );
  break;
  }
}

void DrawCompose(unsigned int Acento,unsigned int Letra)
{
	DrawChar(Acento);
	if(Letra < 127)
	DrawChar(Letra);
	else
	DrawChar('?');
}

byte BlockReadChar()
{
  byte ret;
  while (Serial.available() == 0)
  {
    delayMicroseconds(100);
  }
  ret = Serial.read();
  //Serial.println(ret);
  return ret;
}
byte BlockPeekChar()
{
  byte ret;
  while (Serial.available() == 0)
  {
    delayMicroseconds(100);
  }
  return ret = Serial.peek();
}

const void skipASCIIUntil(char ate)
{
  while( readASCII() != ate )
  {
    // just skip;
  }
} 

const byte readASCII()
{
 return BlockReadChar();
}
const byte peekASCII()
{
 return BlockPeekChar();
}
// Funçao que recebe o proximo numero disponível
int proxnum()
{  
byte umbyte =BlockReadChar();// Lê o prox byte na porta serial
while(umbyte == ' ')
umbyte =BlockReadChar();
String num="";
if(umbyte == '-'||umbyte == '+')
{
num += (char)umbyte;
umbyte= BlockReadChar();
}
while(isDigit(umbyte)||umbyte==' ') // enquanto os bytes que forem lidos ainda forem digitos
{
if(umbyte != ' ')  
num += (char)umbyte; // converte o byte ASCII em um char, e então concatena a string 'num'
umbyte = BlockReadChar(); // Lê o prox byte na porta serial
}
if(num == ""){
Serial.print(F(" ---- Faltou um numero aqui! ----- "));return(0);}
return(atoi(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atof é abreviatura de "ASCII to Float" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atof) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um float
*/
}

int parseCODE(char c0,char c1,char c2,char c3,char c4,char c5)
{
switch(c0)
{
case 'A':
	switch(c1)
	{
	case 'a':
		//TODO action Case Aacute --> 'Á' cod:193 latin capital letter A with acute
		return 193;
	break;
	case 'r':
		//TODO action Case Aring --> 'Å' cod:197 latin capital letter A with ring above
		return 197;
	break;
	case 'c':
		//TODO action Case Acirc --> 'Â' cod:194 latin capital letter A with circumflex
		return 194;
	break;
	case 't':
		//TODO action Case Atilde --> 'Ã' cod:195 latin capital letter A with tilde
		return 195;
	break;
	case 'u':
		//TODO action Case Auml --> 'Ä' cod:196 latin capital letter A with diaeresis
		return 196;
	break;
	case 'E':
		//TODO action Case AElig --> 'Æ' cod:198 latin capital letter AE
		return 198;
	break;
	case 'g':
		//TODO action Case Agrave --> 'À' cod:192 latin capital letter A with grave
		return 192;
	break;
	}
break;
case 'C':
	//TODO action Case Ccedil --> 'Ç' cod:199 latin capital letter C with cedilla
	return 199;
break;
case 'E':
	switch(c1)
	{
	case 'a':
		//TODO action Case Eacute --> 'É' cod:201 latin capital letter E with acute
		return 201;
	break;
	case 'c':
		//TODO action Case Ecirc --> 'Ê' cod:202 latin capital letter E with circumflex
		return 202;
	break;
	case 'T':
		//TODO action Case ETH --> 'Ð' cod:208 latin capital letter ETH
		return 208;
	break;
	case 'u':
		//TODO action Case Euml --> 'Ë' cod:203 latin capital letter E with diaeresis
		return 203;
	break;
	case 'g':
		//TODO action Case Egrave --> 'È' cod:200 latin capital letter E with grave
		return 200;
	break;
	}
break;
case 'I':
	switch(c1)
	{
	case 'a':
		//TODO action Case Iacute --> 'Í' cod:205 latin capital letter I with acute
		return 205;
	break;
	case 'c':
		//TODO action Case Icirc --> 'Î' cod:206 latin capital letter I with circumflex
		return 206;
	break;
	case 'u':
		//TODO action Case Iuml --> 'Ï' cod:207 latin capital letter I with diaeresis
		return 207;
	break;
	case 'g':
		//TODO action Case Igrave --> 'Ì' cod:204 latin capital letter I with grave
		return 204;
	break;
	}
break;
case 'N':
	//TODO action Case Ntilde --> 'Ñ' cod:209 latin capital letter N with tilde
	return 209;
break;
case 'O':
	switch(c1)
	{
	case 'a':
		//TODO action Case Oacute --> 'Ó' cod:211 latin capital letter O with acute
		return 211;
	break;
	case 'c':
		//TODO action Case Ocirc --> 'Ô' cod:212 latin capital letter O with circumflex
		return 212;
	break;
	case 's':
		//TODO action Case Oslash --> 'Ø' cod:216 latin capital letter O with slash
		return 216;
	break;
	case 't':
		//TODO action Case Otilde --> 'Õ' cod:213 latin capital letter O with tilde
		return 213;
	break;
	case 'u':
		//TODO action Case Ouml --> 'Ö' cod:214 latin capital letter O with diaeresis
		return 214;
	break;
	case 'g':
		//TODO action Case Ograve --> 'Ò' cod:210 latin capital letter O with grave
		return 210;
	break;
	}
break;
case 'T':
	//TODO action Case THORN --> 'Þ' cod:222 latin capital letter THORN
	return 222;
break;
case 'U':
	switch(c1)
	{
	case 'a':
		//TODO action Case Uacute --> 'Ú' cod:218 latin capital letter U with acute
		return 218;
	break;
	case 'c':
		//TODO action Case Ucirc --> 'Û' cod:219 latin capital letter U with circumflex
		return 219;
	break;
	case 'u':
		//TODO action Case Uuml --> 'Ü' cod:220 latin capital letter U with diaeresis
		return 220;
	break;
	case 'g':
		//TODO action Case Ugrave --> 'Ù' cod:217 latin capital letter U with grave
		return 217;
	break;
	}
break;
case 'Y':
	//TODO action Case Yacute --> 'Ý' cod:221 latin capital letter Y with acute
	return 221;
break;
case 'a':
	switch(c1)
	{
	case 'a':
		//TODO action Case aacute --> 'á' cod:225 latin small letter a with acute
		return 225;
	break;
	case 'r':
		//TODO action Case aring --> 'å' cod:229 latin small letter a with ring above
		return 229;
	break;
	case 'c':
		switch(c2)
		{
		case 'u':
			//TODO action Case acute --> '´' cod:180 acute accent - spacing acute
			return 180;
		break;
		case 'i':
			//TODO action Case acirc --> 'â' cod:226 latin small letter a with circumflex
			return 226;
		break;
		}
	break;
	case 't':
		//TODO action Case atilde --> 'ã' cod:227 latin small letter a with tilde
		return 227;
	break;
	case 'u':
		//TODO action Case auml --> 'ä' cod:228 latin small letter a with diaeresis
		return 228;
	break;
	case 'e':
		//TODO action Case aelig --> 'æ' cod:230 latin small letter ae
		return 230;
	break;
	case 'g':
		//TODO action Case agrave --> 'à' cod:224 latin small letter a with grave
		return 224;
	break;
	case 'm':
		//TODO action Case amp --> '&' cod:38 Amperstand
		return 38;
	break;
	}
break;
case 'b':
	//TODO action Case brvbar --> '¦' cod:166 broken vertical bar
	return 166;
break;
case 'c':
	switch(c1)
	{
	case 'c':
		//TODO action Case ccedil --> 'ç' cod:231 latin small letter c with cedilla
		return 231;
	break;
	case 'u':
		//TODO action Case curren --> '¤' cod:164 currency sign
		return 164;
	break;
	case 'e':
		switch(c2)
		{
		case 'd':
			//TODO action Case cedil --> '¸' cod:184 spacing cedilla
			return 184;
		break;
		case 'n':
			//TODO action Case cent --> '¢' cod:162 cent sign
			return 162;
		break;
		}
	break;
	case 'o':
		//TODO action Case copy --> '©' cod:169 copyright sign
		return 169;
	break;
	}
break;
case 'd':
	switch(c1)
	{
	case 'e':
		//TODO action Case deg --> '°' cod:176 degree sign
		return 176;
	break;
	case 'i':
		//TODO action Case divide --> '÷' cod:247 division sign
		return 247;
	break;
	}
break;
case 'e':
	switch(c1)
	{
	case 'a':
		//TODO action Case eacute --> 'é' cod:233 latin small letter e with acute
		return 233;
	break;
	case 'c':
		//TODO action Case ecirc --> 'ê' cod:234 latin small letter e with circumflex
		return 234;
	break;
	case 't':
		//TODO action Case eth --> 'ð' cod:240 latin small letter eth
		return 240;
	break;
	case 'u':
		switch(c2)
		{
		case 'r':
			//TODO action Case euro --> '€' cod:8364 euro
			return 8364;
		break;
		case 'm':
			//TODO action Case euml --> 'ë' cod:235 latin small letter e with diaeresis
			return 235;
		break;
		}
	break;
	case 'g':
		//TODO action Case egrave --> 'è' cod:232 latin small letter e with grave
		return 232;
	break;
	}
break;
case 'f':
	switch(c1)
	{
	case 'r':
		switch(c2)
		{
		case 'a':
			switch(c3)
			{
			case 'c':
				switch(c4)
				{
				case '1':
					switch(c5)
					{
					case '2':
						//TODO action Case frac12 --> '½' cod:189 fraction one half
						return 189;
					break;
					case '4':
						//TODO action Case frac14 --> '¼' cod:188 fraction one quarter
						return 188;
					break;
					}
				break;
				case '3':
					//TODO action Case frac34 --> '¾' cod:190 fraction three quarters
					return 190;
				break;
				}
			break;
			}
		break;
		}
	break;
	}
break;
case 'g':
	//TODO action Case gt --> '>' cod:62 greater than
	return 62;
break;
case 'i':
	switch(c1)
	{
	case 'q':
		//TODO action Case iquest --> '¿' cod:191 inverted question mark
		return 191;
	break;
	case 'a':
		//TODO action Case iacute --> 'í' cod:237 latin small letter i with acute
		return 237;
	break;
	case 'c':
		//TODO action Case icirc --> 'î' cod:238 latin small letter i with circumflex
		return 238;
	break;
	case 'e':
		//TODO action Case iexcl --> '¡' cod:161 inverted exclamation mark
		return 161;
	break;
	case 'u':
		//TODO action Case iuml --> 'ï' cod:239 latin small letter i with diaeresis
		return 239;
	break;
	case 'g':
		//TODO action Case igrave --> 'ì' cod:236 latin small letter i with grave
		return 236;
	break;
	}
break;
case 'l':
	switch(c1)
	{
	case 'a':
		//TODO action Case laquo --> '«' cod:171 left double angle quotes
		return 171;
	break;
	case 't':
		//TODO action Case lt --> '<' cod:60 less than
		return 60;
	break;
	}
break;
case 'm':
	switch(c1)
	{
	case 'a':
		//TODO action Case macr --> '¯' cod:175 spacing macron - overline
		return 175;
	break;
	case 'i':
		switch(c2)
		{
		case 'c':
			//TODO action Case micro --> 'µ' cod:181 micro sign
			return 181;
		break;
		case 'd':
			//TODO action Case middot --> '·' cod:183 middle dot - Georgian comma
			return 183;
		break;
		}
	break;
	}
break;
case 'n':
	switch(c1)
	{
	case 'b':
		//TODO action Case nbsp --> ' ' cod:32 non-breaking space
		return 32;
	break;
	case 't':
		//TODO action Case ntilde --> 'ñ' cod:241 latin small letter n with tilde
		return 241;
	break;
	case 'o':
		//TODO action Case not --> '¬' cod:172 not sign
		return 172;
	break;
	}
break;
case 'o':
	switch(c1)
	{
	case 'a':
		//TODO action Case oacute --> 'ó' cod:243 latin small letter o with acute
		return 243;
	break;
	case 'r':
		switch(c2)
		{
		case 'd':
			switch(c3)
			{
			case 'f':
				//TODO action Case ordf --> 'ª' cod:170 feminine ordinal indicator
				return 170;
			break;
			case 'm':
				//TODO action Case ordm --> 'º' cod:186 masculine ordinal indicator
				return 186;
			break;
			}
		break;
		}
	break;
	case 'c':
		//TODO action Case ocirc --> 'ô' cod:244 latin small letter o with circumflex
		return 244;
	break;
	case 's':
		//TODO action Case oslash --> 'ø' cod:248 latin small letter o with slash
		return 248;
	break;
	case 't':
		//TODO action Case otilde --> 'õ' cod:245 latin small letter o with tilde
		return 245;
	break;
	case 'u':
		//TODO action Case ouml --> 'ö' cod:246 latin small letter o with diaeresis
		return 246;
	break;
	case 'g':
		//TODO action Case ograve --> 'ò' cod:242 latin small letter o with grave
		return 242;
	break;
	}
break;
case 'p':
	switch(c1)
	{
	case 'a':
		//TODO action Case para --> '¶' cod:182 pilcrow sign - paragraph sign
		return 182;
	break;
	case 'l':
		//TODO action Case plusmn --> '±' cod:177 plus-or-minus sign
		return 177;
	break;
	case 'o':
		//TODO action Case pound --> '£' cod:163 pound sign
		return 163;
	break;
	}
break;
case 'q':
	//TODO action Case quot --> '"' cod:34 Aspas
	return 34;
break;
case 'r':
	switch(c1)
	{
	case 'a':
		//TODO action Case raquo --> '»' cod:187 right double angle quotes
		return 187;
	break;
	case 'e':
		//TODO action Case reg --> '®' cod:174 registered trade mark sign
		return 174;
	break;
	}
break;
case 's':
	switch(c1)
	{
	case 'e':
		//TODO action Case sect --> '§' cod:167 section sign
		return 167;
	break;
	case 'u':
		switch(c2)
		{
		case 'p':
			switch(c3)
			{
			case '1':
				//TODO action Case sup1 --> '¹' cod:185 superscript one
				return 185;
			break;
			case '2':
				//TODO action Case sup2 --> '²' cod:178 superscript two - squared
				return 178;
			break;
			case '3':
				//TODO action Case sup3 --> '³' cod:179 superscript three - cubed
				return 179;
			break;
			}
		break;
		}
	break;
	case 'h':
		//TODO action Case shy --> ' ' cod:32 soft hyphen
		return 32;
	break;
	case 'z':
		//TODO action Case szlig --> 'ß' cod:223 latin small letter sharp s - ess-zed
		return 223;
	break;
	}
break;
case 't':
	switch(c1)
	{
	case 'h':
		//TODO action Case thorn --> 'þ' cod:254 latin small letter thorn
		return 254;
	break;
	case 'i':
		//TODO action Case times --> '×' cod:215 multiplication sign
		return 215;
	break;
	}
break;
case 'u':
	switch(c1)
	{
	case 'a':
		//TODO action Case uacute --> 'ú' cod:250 latin small letter u with acute
		return 250;
	break;
	case 'c':
		//TODO action Case ucirc --> 'û' cod:251 latin small letter u with circumflex
		return 251;
	break;
	case 'u':
		//TODO action Case uuml --> 'ü' cod:252 latin small letter u with diaeresis
		return 252;
	break;
	case 'g':
		//TODO action Case ugrave --> 'ù' cod:249 latin small letter u with grave
		return 249;
	break;
	case 'm':
		//TODO action Case uml --> '¨' cod:168 spacing diaeresis - umlaut
		return 168;
	break;
	}
break;
case 'y':
	switch(c1)
	{
	case 'a':
		//TODO action Case yacute --> 'ý' cod:253 latin small letter y with acute
		return 253;
	break;
	case 'e':
		//TODO action Case yen --> '¥' cod:165 yen sign
		return 165;
	break;
	case 'u':
		//TODO action Case yuml --> 'ÿ' cod:255 latin small letter y with diaeresis
		return 255;
	break;
	}
break;
}
return '?';
}

void parseHTML(char c0,char c1,char c2,char c3,char c4,char c5,boolean fechando)
{
  switch(c0)
{
case '!':
	switch(c1)
	{
	case 'D':
		//TODO action Case !DOCTYPE
	break;
	case '-':
		//TODO action Case !--...--
	break;
	}
break;
case 'a':
	switch(c1)
	{
	case 'p':
		//TODO action Case applet
	break;
	case 'b':
		//TODO action Case abbr
	break;
	case 'r':
		switch(c2)
		{
		case 't':
			//TODO action Case article
		break;
		case 'e':
			//TODO action Case area
		break;
		}
	break;
	case 'c':
		//TODO action Case acronym
	break;
	case 's':
		//TODO action Case aside
	break;
	case 'd':
		//TODO action Case address
	break;
	case 'u':
		//TODO action Case audio
	break;
	default:
		//TODO action Case a
	break;
	}
break;
case 'b':
	switch(c1)
	{
	case 'a':
		switch(c2)
		{
		case 's':
			switch(c3)
			{
			case 'e':
				switch(c4)
				{
				case 'f':
					//TODO action Case basefont
				break;
				default:
					//TODO action Case base
				break;
				}
			break;
			}
		break;
		}
	break;
	case 'r':
		//TODO action Case br
                Println();
	break;
	case 'd':
		switch(c2)
		{
		case 'i':
			//TODO action Case bdi
		break;
		case 'o':
			//TODO action Case bdo
		break;
		}
	break;
	case 'u':
		//TODO action Case button
	break;
	case 'i':
		//TODO action Case big
	break;
	case 'l':
		//TODO action Case blockquote
	break;
	default:
		//TODO action Case b
	break;
	case 'o':
		//TODO action Case body
	break;
	}
break;
case 'c':
	switch(c1)
	{
	case 'a':
		switch(c2)
		{
		case 'p':
			//TODO action Case caption
		break;
		case 'n':
			//TODO action Case canvas
		break;
		}
	break;
	case 'e':
		//TODO action Case center
	break;
	case 'i':
		//TODO action Case cite
	break;
	case 'o':
		switch(c2)
		{
		case 'd':
			//TODO action Case code
		break;
		case 'l':
			switch(c3)
			{
			case 'g':
				//TODO action Case colgroup
			break;
			default:
				//TODO action Case col
			break;
			}
		break;
		}
	break;
	}
break;
case 'd':
	switch(c1)
	{
	case 'a':
		//TODO action Case datalist
	break;
	case 'd':
		//TODO action Case dd
	break;
	case 't':
		//TODO action Case dt
	break;
	case 'e':
		switch(c2)
		{
		case 't':
			//TODO action Case details
		break;
		case 'l':
			//TODO action Case del
		break;
		}
	break;
	case 'f':
		//TODO action Case dfn
	break;
	case 'i':
		switch(c2)
		{
		case 'a':
			//TODO action Case dialog
		break;
		case 'r':
			//TODO action Case dir
		break;
		case 'v':
			//TODO action Case div
		break;
		}
	break;
	case 'l':
		//TODO action Case dl
	break;
	}
break;
case 'e':
	switch(c1)
	{
	case 'm':
		switch(c2)
		{
		case 'b':
			//TODO action Case embed
		break;
		default:
			//TODO action Case em
		break;
		}
	break;
	}
break;
case 'f':
	switch(c1)
	{
	case 'r':
		switch(c2)
		{
		case 'a':
			switch(c3)
			{
			case 'm':
				switch(c4)
				{
				case 'e':
					switch(c5)
					{
					case 's':
						//TODO action Case frameset
					break;
					default:
						//TODO action Case frame
					break;
					}
				break;
				}
			break;
			}
		break;
		}
	break;
	case 'i':
		switch(c2)
		{
		case 'e':
			//TODO action Case fieldset
		break;
		case 'g':
			switch(c3)
			{
			case 'c':
				//TODO action Case figcaption
			break;
			case 'u':
				//TODO action Case figure
			break;
			}
		break;
		}
	break;
	case 'o':
		switch(c2)
		{
		case 'r':
			//TODO action Case form
		break;
		case 'n':
			//TODO action Case font
		break;
		case 'o':
			//TODO action Case footer
		break;
		}
	break;
	}
break;
case 'h':
	switch(c1)
	{
	case '1':
		//TODO action Case h1
	break;
	case '2':
		//TODO action Case h2
	break;
	case 'r':
		//TODO action Case hr
	break;
	case '3':
		//TODO action Case h3
	break;
	case '4':
		switch(c2)
		{
		default:
			//TODO action Case h4
		break;
		}
	break;
	case 't':
		//TODO action Case html
	break;
	case 'e':
		switch(c2)
		{
		case 'a':
			switch(c3)
			{
			case 'd':
				switch(c4)
				{
				case 'e':
					//TODO action Case header
				break;
				default:
					//TODO action Case head
				break;
				}
			break;
			}
		break;
		}
	break;
	case '6':
		//TODO action Case h6
	break;
	}
break;
case 'i':
	switch(c1)
	{
	case 'f':
		//TODO action Case iframe
	break;
	case 'm':
		//TODO action Case img
	break;
	case 'n':
		switch(c2)
		{
		case 'p':
			//TODO action Case input
		break;
		case 's':
			//TODO action Case ins
		break;
		}
	break;
	default:
		//TODO action Case i
	break;
	}
break;
case 'k':
	switch(c1)
	{
	case 'b':
		//TODO action Case kbd
	break;
	case 'e':
		//TODO action Case keygen
	break;
	}
break;
case 'l':
	switch(c1)
	{
	case 'a':
		//TODO action Case label
	break;
	case 'e':
		//TODO action Case legend
	break;
	case 'i':
		switch(c2)
		{
		case 'n':
			//TODO action Case link
		break;
		default:
			//TODO action Case li
		break;
		}
	break;
	}
break;
case 'm':
	switch(c1)
	{
	case 'a':
		switch(c2)
		{
		case 'p':
			//TODO action Case map
		break;
		case 'r':
			//TODO action Case mark
		break;
		case 'i':
			//TODO action Case main
		break;
		}
	break;
	case 'e':
		switch(c2)
		{
		case 't':
			switch(c3)
			{
			case 'a':
				//TODO action Case meta
			break;
			case 'e':
				//TODO action Case meter
			break;
			}
		break;
		case 'n':
			switch(c3)
			{
			case 'u':
				switch(c4)
				{
				case 'i':
					//TODO action Case menuitem
				break;
				default:
					//TODO action Case menu
				break;
				}
			break;
			}
		break;
		}
	break;
	}
break;
case 'n':
	switch(c1)
	{
	case 'a':
		//TODO action Case nav
	break;
	case 'o':
		switch(c2)
		{
		case 's':
			//TODO action Case noscript
		break;
		case 'f':
			//TODO action Case noframes
		break;
		}
	break;
	}
break;
case 'o':
	switch(c1)
	{
	case 'p':
		switch(c2)
		{
		case 't':
			switch(c3)
			{
			case 'g':
				//TODO action Case optgroup
			break;
			case 'i':
				//TODO action Case option
			break;
			}
		break;
		}
	break;
	case 'b':
		//TODO action Case object
	break;
	case 'u':
		//TODO action Case output
	break;
	case 'l':
		//TODO action Case ol
	break;
	}
break;
case 'p':
	switch(c1)
	{
	case 'a':
		//TODO action Case param
	break;
	case 'r':
		switch(c2)
		{
		case 'e':
			//TODO action Case pre
		break;
		case 'o':
			//TODO action Case progress
		break;
		}
	break;
	default:
		//TODO action Case p
	break;
	}
break;
case 'q':
	//TODO action Case q
break;
case 'r':
	switch(c1)
	{
	case 'p':
		//TODO action Case rp
	break;
	case 't':
		//TODO action Case rt
	break;
	case 'u':
		//TODO action Case ruby
	break;
	}
break;
case 's':
	switch(c1)
	{
	case 'p':
		//TODO action Case span
	break;
	case 'a':
		//TODO action Case samp
	break;
	case 'c':
		//TODO action Case script
	break;
	case 't':
		switch(c2)
		{
		case 'r':
			switch(c3)
			{
			case 'i':
				//TODO action Case strike
			break;
			case 'o':
				//TODO action Case strong
			break;
			}
		break;
		case 'y':
			//TODO action Case style
		break;
		}
	break;
	case 'e':
		switch(c2)
		{
		case 'c':
			//TODO action Case section
		break;
		case 'l':
			//TODO action Case select
		break;
		}
	break;
	case 'u':
		switch(c2)
		{
		case 'p':
			//TODO action Case sup
		break;
		case 'b':
			//TODO action Case sub
		break;
		case 'm':
			//TODO action Case summary
		break;
		}
	break;
	case 'm':
		//TODO action Case small
	break;
	default:
		//TODO action Case s
	break;
	case 'o':
		//TODO action Case source
	break;
	}
break;
case 't':
	switch(c1)
	{
	case 'a':
		//TODO action Case table
	break;
	case 'b':
		//TODO action Case tbody
	break;
	case 'r':
		switch(c2)
		{
		case 'a':
			//TODO action Case track
		break;
		default:
			//TODO action Case tr
		break;
		}
	break;
	case 'd':
		//TODO action Case td
	break;
	case 't':
		//TODO action Case tt
	break;
	case 'e':
		//TODO action Case textarea
	break;
	case 'f':
		//TODO action Case tfoot
	break;
	case 'h':
		switch(c2)
		{
		case 'e':
			//TODO action Case thead
		break;
		default:
			//TODO action Case th
		break;
		}
	break;
	case 'i':
		switch(c2)
		{
		case 't':
			//TODO action Case title
		break;
		case 'm':
			//TODO action Case time
		break;
		}
	break;
	}
break;
case 'u':
	switch(c1)
	{
	case 'l':
		//TODO action Case ul
	break;
	default:
		//TODO action Case u
	break;
	}
break;
case 'v':
	switch(c1)
	{
	case 'a':
		//TODO action Case var
	break;
	case 'i':
		//TODO action Case video
	break;
	}
break;
case 'w':
	//TODO action Case wbr
break;
}
}
