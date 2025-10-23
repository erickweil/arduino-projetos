int grade[] =
{13,12,11,10,
 9 , 8, 7, 6,
 5 , 4, 3, 2}; // pinos dos leds dispostos em sequencia
/*
para uma grade de dimensoes igual a x=4 e y =3
os indices contam do canto esqerdo inferior para o canto direito superior:
 _______________
 |8 , 9 , 10, 11|
 |4 , 5 , 6 , 7 |
 |0 , 1 , 2 , 3 |
 e as posicoes seriam assim: i.e : {x,y}
 ________________________________
 |{0,2} , {1,2} , {2,2} , {3,2} |
 |{0,1} , {1,1} , {2,1} , {3,1} |
 |{0,0} , {1,0} , {2,0} , {3,0} | 
  
então coloque os valores de cada pino de led no array grade de acordo 
com os indices desse esquema.
  
*/
int T;
int Dx =4;// dimensoes da grade no sentido x
int Dy =3;// dimensoes da grade no sentido y
// usar long se tiver mais de 16 leds e menos de 32, mais que 32 reescrever codigo para aceitar strings;
String Rotina[] = {
  "111000000000","000111000000","000000111000","000000000111",
  "000000000000","001001001001","010010010010","100100100100",
  "000000000000","100000000000","010100000000","001010100000",
  "000001010100","000000001010","000000000001","000000000000",
  "100100000000","010010000000","001001000000","000000100100",
  "000000010010","000000001001","000000000000","000000000000",
};
/*String Rotina[] = {
  "111111111111","111111111110","111111111100","111111111000",
  "111111110000","111111100000","111111000000","111110000000",
  "111100000000","111000000000","110000000000","100000000000",
  "000000000000","100000000000","110000000000","111000000000",
  "111100000000","111110000000","111111000000","111111100000",
  "111111110000","111111111000","111111111100","111111111110",
    
};*/
long Rotinadelay[] = {
  100,100,100,100,
  100,100,100,100,
  100,100,100,100,
  100,100,100,100,
  100,100,100,100,
  100,100,100,100
  };
int S = sizeof(Rotina)/sizeof(*Rotina);
int rotina[sizeof(Rotina)/sizeof(*Rotina)]; 
int frame = 0;
void setup()
{
Serial.begin(9600);  
for(int i=0;i<S;i++)
{
Serial.print("o frame ");Serial.print(i);Serial.print(" tera a sequencia : {");  
rotina[i] = SB(Rotina[i]);
Serial.println("}");
}

T = sizeof(grade)/sizeof(*grade); // mesma coisa que grade.length em java  
for(int x=0;x<Dx;x++)
{
for(int y=0;y<Dy;y++)
{
pinMode(P(x,y), OUTPUT);
Serial.print("O led ");Serial.print(P(x,y));Serial.print(" foi setado na pos x:");Serial.print(x);Serial.print(" y:");Serial.println(y);
}
}
  

}


void loop()
{
sequencia();
delay(Rotinadelay[frame]);
}

void sequencia()
{  
if( frame < S-1)
frame++;
else
frame=0;
//Serial.print("frame ");Serial.print(frame);Serial.print(" :");
for(int i=0;i<T;i++)
{
//Serial.print(  bitRead(rotina[frame],i));
digitalWrite(grade[i],bitRead(rotina[frame],i));  
}
//Serial.println();
}

int P(int x,int y)
{
return grade[x + Dx*y];  
}
void Led(int x,int y,boolean estado)
{
digitalWrite(P(x,y), estado);  
}

int SB(String bin) // SB = Setar em Binario
{
unsigned int num=0;
if(bin.length()> 16)
{
Serial.print("sequencia muito grande, substitua int para long no codigo");
return 0;
}
for(unsigned int i=0;i<bin.length();i++)
{
unsigned char bit1;
bit1 = bin.charAt(i);
if(bit1 == '1')
bitSet(num, i);
else
bitClear(num, i);
Serial.print((char)bit1);
Serial.print(",");
}
return num;
}


