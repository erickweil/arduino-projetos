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
  tft.setCursor(0,5);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  
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
char c='?';
char c0;
char c1;
char c2;
char c3;
char c4;
char c5;
boolean fechando=false;
char lastc='?';
void lerlinha()
{
  //String tex="";  // cria uma string
  while(c!='\0') // enquanto houver mais bytes de entrada
  {
    lastc=c;
    c = readASCII(); 
    //tex +=c; // converte o byte em um char e então o concatena à string
    charsRead++;
    switch(c)
    {
      // CODE HTML
      case '&':
        c = lastc; // para nao acumular espaços seguidos
        c0 = readASCII();
        if(c0 != ';')
        {
          c1 = readASCII();
          if(c1 != ';')
          {
            c2 = readASCII();
            if(c2 != ';')
            {
              c3 = readASCII();
              if(c3 != ';')
              {
                c4 = readASCII();
                if(c4 != ';')
                {
                  c5 = readASCII();
                  if(c5!=';')
                  {
                     skipASCIIUntil(';'); // pula os caracteres ate o fim
                  }
                }
              }
            }
          }
        }
        parseCODE(c0,c1,c2,c3,c4,c5);
      break;
      //TAG HTML
      case '<':
        c = lastc; // para nao acumular espaços seguidos
        c0 = readASCII();
        if(c0 == '/')
        {
          fechando = true;
          c0 = readASCII();
        }
        else
        {
          fechando = false;
        }
        if(c0 != '>')
        {
          c1 = readASCII();
          if(c1 != '>')
          {
            c2 = readASCII();
            if(c2 != '>')
            {
              c3 = readASCII();
              if(c3 != '>')
              {
                c4 = readASCII();
                if(c4 != '>')
                {
                  c5 = readASCII();
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
        if(!isGraph(c))
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

uint16_t font_posX=0;
uint16_t font_posY=5;
uint16_t font_color = BLACK;
uint8_t font_size = 2;
void Println()
{
  font_posX = 0;
  font_posY += font_size*8;
}
void PrintChar(char c)
{
  tft.drawChar( font_posX, font_posY, c, font_color, font_size );
  font_posX += font_size*6;
  if(font_posX + font_size*6 > W)
  {
    font_posX = 0;
    font_posY += font_size*8;
  }
}
void DrawChar(char c)
{
  tft.drawChar( font_posX, font_posY, c, font_color, font_size );
}

void DrawAcento(char Acento)
{
  switch(Acento)
  {
    case '´':tft.drawLine(font_posX+font_size, font_posY+font_size,  font_posX+font_size*3, font_posY-font_size, font_color);break;
    case '`':tft.drawLine(font_posX+font_size*3, font_posY+font_size,  font_posX+font_size, font_posY-font_size, font_color);break;
    case '^':tft.drawChar( font_posX, font_posY-font_size*2, Acento, font_color, font_size );break;
    case '~':tft.drawChar( font_posX, font_posY-font_size*2, Acento, font_color, font_size );break;
    case '°':tft.drawChar( font_posX+font_size, font_posY-font_size*2, 'o', font_color, 1 ); break;
    case '¨':
    tft.drawPixel( font_posX+font_size, font_posY+font_size, font_color);
    tft.drawPixel( font_posX+font_size*3, font_posY+font_size, font_color);
    break;
  }
  //tft.drawChar( font_posX, font_posY-font_size*2, c, font_color, font_size);
}
byte BlockReadChar()
{
  byte ret;
  while (Serial.available() == 0)
  {
    delayMicroseconds(100);
  }
  return ret = Serial.read();
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

const char readASCII()
{
 return GETASCII(BlockReadChar());
} 
const char peekASCII()
{
 return GETASCII(BlockPeekChar());
} 
const char GETASCII(char especial)
{
  
  if( especial < 127)
  return (char)especial;
  else
  {
    switch(especial)
    {
      case 225: case 224: case 227: case 226://a
      return 'a';
      case 233:case 232:case 234:case 235:
      return 'e';
      case 237: case 236: case 238: case 239:
      return 'i';
      case 243: case 242: case 245: case 244: case 246:
      return 'o';
      case 250: case 249: case 251: case 252:
      return 'u';
      
      case 193: case 192: case 194: case 195: case 196:
      return 'A';
      case 201: case 200: case 202: case 203:
      return 'E';
      case 205: case 204: case 206: case 207:
      return 'I';
      case 211: case 210: case 212: case 213: case 214:
      return 'O';
      case 218: case 217: case 219: case 220:
      return 'U';

      case 199:return 'C'; 
      case 231:return 'c';
      
      case 185: return '1';
      case 178: return '2';
      case 179: return '3';
      
      case 170: return 'a';
      case 176:case 186: return 'o';

      default:
      return '?';
    }
  }
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

void parseCODE(char c0,char c1,char c2,char c3,char c4,char c5)
{
switch(c0)
{
case 'A':
	switch(c1)
	{
	case 'a':
		//TODO action Case Aacute --> 'Á' latin capital letter A with acute
                DrawAcento('´');PrintChar('A');
	break;
	case 'r':
		//TODO action Case Aring --> 'Å' latin capital letter A with ring above
                DrawAcento('°');PrintChar('A');
	break;
	case 'c':
		//TODO action Case Acirc --> 'Â' latin capital letter A with circumflex
                DrawAcento('^');PrintChar('A');
	break;
	case 't':
		//TODO action Case Atilde --> 'Ã' latin capital letter A with tilde
                DrawAcento('~');PrintChar('A');
	break;
	case 'u':
		//TODO action Case Auml --> 'Ä' latin capital letter A with diaeresis
                DrawAcento('¨');PrintChar('A');
	break;
	case 'E':
		//TODO action Case AElig --> 'Æ' latin capital letter AE
                PrintChar('A');PrintChar('E');
	break;
	case 'g':
		//TODO action Case Agrave --> 'À' latin capital letter A with grave
                DrawAcento('`');PrintChar('A');
	break;
	}
break;
case 'C':
	//TODO action Case Ccedil --> 'Ç' latin capital letter C with cedilla
        DrawChar(',');PrintChar('C');
break;
case 'E':
	switch(c1)
	{
	case 'a':
		//TODO action Case Eacute --> 'É' latin capital letter E with acute
                DrawAcento('´');PrintChar('E');
	break;
	case 'c':
		//TODO action Case Ecirc --> 'Ê' latin capital letter E with circumflex
                DrawAcento('^');PrintChar('E');
	break;
	case 'T':
		//TODO action Case ETH --> 'Ð' latin capital letter ETH
                DrawChar('-');PrintChar('D');
	break;
	case 'u':
		//TODO action Case Euml --> 'Ë' latin capital letter E with diaeresis
                DrawAcento('¨');PrintChar('E');
	break;
	case 'g':
		//TODO action Case Egrave --> 'È' latin capital letter E with grave
                DrawAcento('`');PrintChar('E');
	break;
	}
break;
case 'I':
	switch(c1)
	{
	case 'a':
		//TODO action Case Iacute --> 'Í' latin capital letter I with acute
                DrawAcento('´');PrintChar('I');
	break;
	case 'c':
		//TODO action Case Icirc --> 'Î' latin capital letter I with circumflex
                DrawAcento('^');PrintChar('I');
	break;
	case 'u':
		//TODO action Case Iuml --> 'Ï' latin capital letter I with diaeresis
                DrawAcento('¨');PrintChar('I');
	break;
	case 'g':
		//TODO action Case Igrave --> 'Ì' latin capital letter I with grave
                DrawAcento('`');PrintChar('I');
	break;
	}
break;
case 'N':
	//TODO action Case Ntilde --> 'Ñ' latin capital letter N with tilde
        DrawAcento('~');PrintChar('N');
break;
case 'O':
	switch(c1)
	{
	case 'a':
		//TODO action Case Oacute --> 'Ó' latin capital letter O with acute
                DrawAcento('´');PrintChar('O');
	break;
	case 'c':
		//TODO action Case Ocirc --> 'Ô' latin capital letter O with circumflex
                DrawAcento('^');PrintChar('O');
	break;
	case 's':
		//TODO action Case Oslash --> 'Ø' latin capital letter O with slash
                DrawChar('/');PrintChar('O');
	break;
	case 't':
		//TODO action Case Otilde --> 'Õ' latin capital letter O with tilde
                DrawAcento('~');PrintChar('O');
	break;
	case 'u':
		//TODO action Case Ouml --> 'Ö' latin capital letter O with diaeresis
                DrawAcento('¨');PrintChar('O');
	break;
	case 'g':
		//TODO action Case Ograve --> 'Ò' latin capital letter O with grave
                DrawAcento('`');PrintChar('O');
	break;
	}
break;
case 'T':
	//TODO action Case THORN --> 'Þ' latin capital letter THORN
        DrawChar('|');PrintChar('b');
break;
case 'U':
	switch(c1)
	{
	case 'a':
		//TODO action Case Uacute --> 'Ú' latin capital letter U with acute
                DrawAcento('´');PrintChar('U');
	break;
	case 'c':
		//TODO action Case Ucirc --> 'Û' latin capital letter U with circumflex
                DrawAcento('^');PrintChar('U');
	break;
	case 'u':
		//TODO action Case Uuml --> 'Ü' latin capital letter U with diaeresis
                DrawAcento('¨');PrintChar('U');
	break;
	case 'g':
		//TODO action Case Ugrave --> 'Ù' latin capital letter U with grave
                DrawAcento('`');PrintChar('U');
	break;
	}
break;
case 'Y':
	//TODO action Case Yacute --> 'Ý' latin capital letter Y with acute
        DrawAcento('´');PrintChar('Y');
break;
case 'a':
	switch(c1)
	{
	case 'a':
		//TODO action Case aacute --> 'á' latin small letter a with acute
                DrawAcento('´');PrintChar('a');
	break;
	case 'r':
		//TODO action Case aring --> 'å' latin small letter a with ring above
                DrawAcento('°');PrintChar('a');
	break;
	case 'c':
		switch(c2)
		{
		case 'u':
			//TODO action Case acute --> '´' acute accent - spacing acute
                        DrawAcento('´');PrintChar(' ');
		break;
		case 'i':
			//TODO action Case acirc --> 'â' latin small letter a with circumflex
                        DrawAcento('^');PrintChar('a');
		break;
		}
	break;
	case 't':
		//TODO action Case atilde --> 'ã' latin small letter a with tilde
                DrawAcento('~');PrintChar('a');
	break;
	case 'u':
		//TODO action Case auml --> 'ä' latin small letter a with diaeresis
                DrawAcento('¨');PrintChar('a');
	break;
	case 'e':
		//TODO action Case aelig --> 'æ' latin small letter ae
                PrintChar('a');PrintChar('e');
	break;
	case 'g':
		//TODO action Case agrave --> 'à' latin small letter a with grave
                DrawAcento('`');PrintChar('a');
	break;
	case 'm':
		//TODO action Case amp --> '&' Amperstand
                PrintChar('&');
	break;
	}
break;
case 'b':
	//TODO action Case brvbar --> '¦' broken vertical bar
        PrintChar('|');
break;
case 'c':
	switch(c1)
	{
	case 'c':
		//TODO action Case ccedil --> 'ç' latin small letter c with cedilla
                DrawChar(',');PrintChar('c');
	break;
	case 'u':
		//TODO action Case curren --> '¤' currency sign
                DrawChar('X');PrintChar('o');
	break;
	case 'e':
		switch(c2)
		{
		case 'd':
			//TODO action Case cedil --> '¸' spacing cedilla
                        PrintChar(',');
		break;
		case 'n':
			//TODO action Case cent --> '¢' cent sign
                        DrawChar('|');PrintChar('c');
		break;
		}
	break;
	case 'o':
		//TODO action Case copy --> '©' copyright sign
                PrintChar('(');PrintChar('c');PrintChar(')');
	break;
	}
break;
case 'd':
	switch(c1)
	{
	case 'e':
		//TODO action Case deg --> '°' degree sign
                DrawAcento('°');PrintChar(' ');
	break;
	case 'i':
		//TODO action Case divide --> '÷' division sign
                DrawChar('-');PrintChar(':');
	break;
	}
break;
case 'e':
	switch(c1)
	{
	case 'a':
		//TODO action Case eacute --> 'é' latin small letter e with acute
                DrawAcento('´');PrintChar('e');
	break;
	case 'c':
		//TODO action Case ecirc --> 'ê' latin small letter e with circumflex
                DrawAcento('^');PrintChar('e');
	break;
	case 't':
		//TODO action Case eth --> 'ð' latin small letter eth
                PrintChar('e');PrintChar('t');PrintChar('h');
	break;
	case 'u':
		switch(c2)
		{
		case 'r':
			//TODO action Case euro --> '€' euro
                        DrawChar('=');PrintChar('C');
		break;
		case 'm':
			//TODO action Case euml --> 'ë' latin small letter e with diaeresis
                        DrawAcento('¨');PrintChar('e');
		break;
		}
	break;
	case 'g':
		//TODO action Case egrave --> 'è' latin small letter e with grave
                DrawAcento('`');PrintChar('e');
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
						//TODO action Case frac12 --> '½' fraction one half
                                                PrintChar('1');PrintChar('/');PrintChar('2');
					break;
					case '4':
						//TODO action Case frac14 --> '¼' fraction one quarter
                                                PrintChar('1');PrintChar('/');PrintChar('4');
					break;
					}
				break;
				case '3':
					//TODO action Case frac34 --> '¾' fraction three quarters
                                        PrintChar('3');PrintChar('/');PrintChar('4');
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
	//TODO action Case gt --> '>' greater than
          PrintChar('>');
break;
case 'i':
	switch(c1)
	{
	case 'q':
		//TODO action Case iquest --> '¿' inverted question mark
                PrintChar('?');
	break;
	case 'a':
		//TODO action Case iacute --> 'í' latin small letter i with acute
                DrawAcento('´');PrintChar('i');
	break;
	case 'c':
		//TODO action Case icirc --> 'î' latin small letter i with circumflex
                DrawAcento('^');PrintChar('i');
	break;
	case 'e':
		//TODO action Case iexcl --> '¡' inverted exclamation mark
                PrintChar('!');
	break;
	case 'u':
		//TODO action Case iuml --> 'ï' latin small letter i with diaeresis
                DrawAcento('¨');PrintChar('i');
	break;
	case 'g':
		//TODO action Case igrave --> 'ì' latin small letter i with grave
                DrawAcento('`');PrintChar('i');
	break;
	}
break;
case 'l':
	switch(c1)
	{
	case 'a':
		//TODO action Case laquo --> '«' left double angle quotes
                PrintChar('"');
	break;
	case 't':
		//TODO action Case lt --> '<' less than
                PrintChar('<');
	break;
	}
break;
case 'm':
	switch(c1)
	{
	case 'a':
		//TODO action Case macr --> '¯' spacing macron - overline
                PrintChar('-');
	break;
	case 'i':
		switch(c2)
		{
		case 'c':
			//TODO action Case micro --> 'µ' micro sign
                        DrawChar('|');PrintChar('u');  
		break;
		case 'd':
			//TODO action Case middot --> '·' middle dot - Georgian comma
                        DrawAcento('°');PrintChar(' ');
		break;
		}
	break;
	}
break;
case 'n':
	switch(c1)
	{
	case 'b':
		//TODO action Case nbsp --> ' ' non-breaking space
                PrintChar(' ');
	break;
	case 't':
		//TODO action Case ntilde --> 'ñ' latin small letter n with tilde
                DrawAcento('~');PrintChar('n');
	break;
	case 'o':
		//TODO action Case not --> '¬' not sign
                PrintChar('-');
	break;
	}
break;
case 'o':
	switch(c1)
	{
	case 'a':
		//TODO action Case oacute --> 'ó' latin small letter o with acute
                DrawAcento('´');PrintChar('o');
	break;
	case 'r':
		switch(c2)
		{
		case 'd':
			switch(c3)
			{
			case 'f':
				//TODO action Case ordf --> 'ª' feminine ordinal indicator
			break;
			case 'm':
				//TODO action Case ordm --> 'º' masculine ordinal indicator
			break;
			}
		break;
		}
	break;
	case 'c':
		//TODO action Case ocirc --> 'ô' latin small letter o with circumflex
                DrawAcento('^');PrintChar('o');
	break;
	case 's':
		//TODO action Case oslash --> 'ø' latin small letter o with slash
                DrawChar('/');PrintChar('o');
	break;
	case 't':
		//TODO action Case otilde --> 'õ' latin small letter o with tilde
                DrawAcento('~');PrintChar('o');
	break;
	case 'u':
		//TODO action Case ouml --> 'ö' latin small letter o with diaeresis
                DrawAcento('¨');PrintChar('o');
	break;
	case 'g':
		//TODO action Case ograve --> 'ò' latin small letter o with grave
                DrawAcento('`');PrintChar('o');
	break;
	}
break;
case 'p':
	switch(c1)
	{
	case 'a':
		//TODO action Case para --> '¶' pilcrow sign - paragraph sign
                DrawChar('P');PrintChar('|');
	break;
	case 'l':
		//TODO action Case plusmn --> '±' plus-or-minus sign
                DrawChar('+');PrintChar('_');
	break;
	case 'o':
		//TODO action Case pound --> '£' pound sign
                DrawChar('-');PrintChar('L');
	break;
	}
break;
case 'q':
	//TODO action Case quot --> '"' Aspas
        PrintChar('"');
break;
case 'r':
	switch(c1)
	{
	case 'a':
		//TODO action Case raquo --> '»' right double angle quotes
                PrintChar('"');
	break;
	case 'e':
		//TODO action Case reg --> '®' registered trade mark sign
                PrintChar('(');PrintChar('r');PrintChar(')');
	break;
	}
break;
case 's':
	switch(c1)
	{
	case 'e':
		//TODO action Case sect --> '§' section sign
                DrawChar('\\');PrintChar('S');
	break;
	case 'u':
		switch(c2)
		{
		case 'p':
			switch(c3)
			{
			case '1':
				//TODO action Case sup1 --> '¹' superscript one
                                PrintChar('1');
			break;
			case '2':
				//TODO action Case sup2 --> '²' superscript two - squared
                                PrintChar('2');  
			break;
			case '3':
				//TODO action Case sup3 --> '³' superscript three - cubed
                                PrintChar('3');
			break;
			}
		break;
		}
	break;
	case 'h':
		//TODO action Case shy --> '' soft hyphen
                PrintChar('-');
	break;
	case 'z':
		//TODO action Case szlig --> 'ß' latin small letter sharp s - ess-zed
                DrawChar('f');PrintChar('3');
	break;
	}
break;
case 't':
	switch(c1)
	{
	case 'h':
		//TODO action Case thorn --> 'þ' latin small letter thorn
                DrawChar('|');PrintChar('o');
	break;
	case 'i':
		//TODO action Case times --> '×' multiplication sign
                PrintChar('*');
	break;
	}
break;
case 'u':
	switch(c1)
	{
	case 'a':
		//TODO action Case uacute --> 'ú' latin small letter u with acute
                DrawAcento('´');PrintChar('u');
	break;
	case 'c':
		//TODO action Case ucirc --> 'û' latin small letter u with circumflex
                DrawAcento('^');PrintChar('u');
	break;
	case 'u':
		//TODO action Case uuml --> 'ü' latin small letter u with diaeresis
                DrawAcento('¨');PrintChar('u');
	break;
	case 'g':
		//TODO action Case ugrave --> 'ù' latin small letter u with grave
                DrawAcento('`');PrintChar('u');
	break;
	case 'm':
		//TODO action Case uml --> '¨' spacing diaeresis - umlaut
                DrawAcento('¨');PrintChar(' ');
	break;
	}
break;
case 'y':
	switch(c1)
	{
	case 'a':
		//TODO action Case yacute --> 'ý' latin small letter y with acute
                DrawAcento('´');PrintChar('y');
	break;
	case 'e':
		//TODO action Case yen --> '¥' yen sign
                DrawChar('=');PrintChar('Y');
	break;
	case 'u':
		//TODO action Case yuml --> 'ÿ' latin small letter y with diaeresis
                DrawAcento('¨');PrintChar('y');
	break;
	}
break;
}
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
