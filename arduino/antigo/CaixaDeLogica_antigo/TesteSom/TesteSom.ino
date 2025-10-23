  /*************************************************
 * Public Constants
 *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
  
  int grade[] =
  {13,12,11,
   10, 9, 8,
   7,  6, 5,
   4,  3, 2}; // pinos dos leds dispostos em sequencia
  byte grade_cache[] =
  {0,0,0,
   0,0,0,
   0,0,0,
   0,0,0}; // pinos que estao ou nao ligados
  int P(int x,int y)
  {
    switch(x)
    {
      case 0:
      switch(y)
      {
        case 0:return 0;
        case 1:return 3;
        case 2:return 6;
        case 3:return 9;
      }
      case 1:
      switch(y)
      {
        case 0:return 1;
        case 1:return 4;
        case 2:return 7;
        case 3:return 10;
      }
      case 2:
      switch(y)
      {
        case 0:return 2;
        case 1:return 5;
        case 2:return 8;
        case 3:return 11;
      }
    }
    return 0;
  }
   int buzzer = 19;
   int botoes[] = {14,15,16,17};
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
  long note_div = 4978/24;
  int On_toOFF_melody[] = {
  note_div*0, note_div*1, note_div*2, 
  note_div*3, note_div*4, note_div*5,
  note_div*6, note_div*7, note_div*8, 
  note_div*9, note_div*10, note_div*11
  };
  int Off_toON_melody[] = {
  note_div*0, note_div*1, note_div*2, 
  note_div*3, note_div*4, note_div*5,
  note_div*6, note_div*7, note_div*8, 
  note_div*9, note_div*10, note_div*11
  };
  int T;
  int Dx =3;// dimensoes da grade no sentido x
  int Dy =4;// dimensoes da grade no sentido y
  boolean Tocar = true;
  void setup()
  {
  Serial.begin(9600);  
    // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  T = sizeof(grade)/sizeof(*grade); // mesma coisa que grade.length em java  
  for(int i=0;i<12;i++)
  pinMode(grade[i], OUTPUT);
  for(int i=0;i<4;i++)
  { 
  pinMode(botoes[i], INPUT);
  Serial.print("o botao ");Serial.print(botoes[i]);Serial.print(" foi setado no index i:");Serial.println(i);
  }
  pinMode(buzzer, OUTPUT);
  }
  long Delay = 100;
    void Leds(
  byte led0,
  byte led1,
  byte led2,
  byte led3,
  byte led4,
  byte led5,
  byte led6,
  byte led7,
  byte led8,
  byte led9,
  byte led10,
  byte led11)
  {
    Led(0,led0);
    Led(1,led1);
    Led(2,led2);
    Led(3,led3);
    Led(4,led4);
    Led(5,led5);
    Led(6,led6);
    Led(7,led7);
    Led(8,led8);
    Led(9,led9);
    Led(10,led10);
    Led(11,led11);
    delay(Delay);
  }
   
  void Leds_semdelay(
  byte led0,
  byte led1,
  byte led2,
  byte led3,
  byte led4,
  byte led5,
  byte led6,
  byte led7,
  byte led8,
  byte led9,
  byte led10,
  byte led11)
  {
    Led(0,led0);
    Led(1,led1);
    Led(2,led2);
    Led(3,led3);
    Led(4,led4);
    Led(5,led5);
    Led(6,led6);
    Led(7,led7);
    Led(8,led8);
    Led(9,led9);
    Led(10,led10);
    Led(11,led11);
  }

  void Led(int index,byte estado)
  {
      //if(bitRead(bitmask,i))
      //tone(buzzer, melody[i], Delay*2);
      if(estado == 0)
      {
        digitalWrite(grade[index],0); 
        if(grade_cache[index]==1)
        {
        //tone(buzzer,On_toOFF_melody[index], Delay);
        grade_cache[index] = 0;
        }
      }
      else
      {
       // tone(buzzer, melody[index], Delay);
        digitalWrite(grade[index],1); 
        if(grade_cache[index]==0)
        {
        if(Tocar)
        {
        tone(buzzer,Off_toON_melody[index], Delay/2);
        delay(10);
        }
        grade_cache[index] = 1;
        }
      }
      //delay(Delay/10);

  }
  void Led(int x,int y,byte estado)
  {
    Led(P(x,y), estado);  
  }
  void yLeds(int y,byte estado)
  {
   for(int x=0;x<Dx;x++)
    {
    Led(P(x,y),estado);
    }
  }
  void xLeds(int x,byte estado)
  {
    for(int y=0;y<Dy;y++)
    {
    Led(P(x,y),estado);
    }
  }
  void SetLeds(byte e)
  {
     Leds_semdelay(e,e,e,e,e,e,e,e,e,e,e,e);
  }
  
  
  
/* Play Melody
 * -----------
 *
 * Program to play melodies stored in an array, it requires to know
 * about timing issues and about how to play tones.
 *
 * The calculation of the tones is made following the mathematical
 * operation:
 *
 *       timeHigh = 1/(2 * toneFrequency) = period / 2
 *
 * where the different tones are described as in the table:
 *
 * note         frequency       period  PW (timeHigh)   
 * c            261 Hz          3830    1915    
 * d            294 Hz          3400    1700    
 * e            329 Hz          3038    1519    
 * f            349 Hz          2864    1432    
 * g            392 Hz          2550    1275    
 * a            440 Hz          2272    1136    
 * b            493 Hz          2028    1014    
 * C            523 Hz          1912    956
 *
 * (cleft) 2005 D. Cuartielles for K3
 */

int ledPin = 13;
int speakerPin = 19;


// Do-Re-Mi-Fa-So-La-Ti
#define c_  261
#define d_  294
#define e_  329
#define f_  349
#define g_  392
#define a_  440
#define b_  493
#define C_  523


#define _c  1915
#define _d  1700
#define _e  1519
#define _f  1432
#define _g  1275
#define _a  1136
#define _b  1014
#define _C  956
#define _p  0
byte names[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};  
int tones[] = {1915, 1700, 1519, 1432, 1275, 1136, 1014, 956};
byte melody[] = "2d2a1f2c2d2a2d2c2f2d2a2c2d2a1f2c2d2a2a2g2p8p8p8p";
// count length: 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
//                                10                  20                  30
int count = 0;
int count2 = 0;
int count3 = 0;
int MAX_COUNT = 24;
int statePin = LOW;


void Tone(int duration, int Tone) {
  switch(Tone)
  {
  case _c:tone(speakerPin,c_,duration);
  case _d:tone(speakerPin,d_,duration);
  case _e:tone(speakerPin,e_,duration);
  case _f:tone(speakerPin,f_,duration);
  case _g:tone(speakerPin,g_,duration);
  case _a:tone(speakerPin,a_,duration);
  case _b:tone(speakerPin,b_,duration);
  case _C:tone(speakerPin,C_,duration);
  }
  delay(duration); 
}
void playTone(int duration, int Tone) {
  long total =duration * 1000L;
  for (long i = 0; i < total; i += Tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(Tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(Tone);
  }
}

/*semibreve (um tempo),
mínima (meio tempo),
semínima (um quarto de tempo),
colcheia (um oitavo de tempo), 
semicolcheia (1/16 de tempo), 
fusa (1/32 de tempo) ,
semifusa (1/64 de tempo).
*/
# define ju_ 0.0
# define um_ 1.0
# define mei_ 0.5
# define qua_ 0.25
# define oit_ 0.125
# define u16_ 0.0625
# define u32_ 0.03125
# define u64_ 0.015625

float tempo = 1.75;

int musica_notas0[] = 
{
 _p,_p,_p,_c,
 _f,_g,_a,_f,
 _C   ,_p,_f,
 _b,_a,_g,_f,
 _e   ,_p,_c,
 // voce pertence a quem que deus vai obedecer? Di
 _e,_e,_f,_e,
 _f,_g,_a,_a,
 _g,_f,_c,_d,
 _a,_g,_e,_c,
 //
 _f,_g,_a,_f,
 _C   ,_p,_f,
 _C,_b,_a,_f,
 _d   ,_d,_d,
 //
 _g,_a,_b,_g,
 _a,_a,_f,_a,
 _g,_f,_f,_g,
 _g
 };
float musica_tempos0[]=
{
qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
mei_     ,qua_,qua_,
qua_,qua_,qua_,qua_,
mei_     ,qua_,qua_,

qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,

qua_,qua_,qua_,qua_,
mei_     ,qua_,qua_,
qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
mei_     ,qua_,qua_,

qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
qua_,qua_,qua_,qua_,
mei_
};
float musica_notas[]=
{
  _f,_e,_f,_g,_b,_d,_d,_b

  
};
float musica_tempos[]=
{
  oit_,oit_,oit_,oit_,qua_,mei_,qua_
};
void loop() {

  int rest=10;
  for(int i=0;i<56;i++)
  {
    Nota(musica_tempos0[i],musica_notas0[i]);//delay(rest);
  }
  /*
  Nota(um_,_c);delay(rest);
  Nota(um_,_d);delay(rest);
  Nota(um_,_e);delay(rest);
  Nota(um_,_f);delay(rest);
  Nota(um_,_g);delay(rest);
  Nota(um_,_a);delay(rest);
  Nota(um_,_b);delay(rest);
  Nota(um_,_C);delay(rest);
  Nota(um_,_b);delay(rest);
  Nota(um_,_a);delay(rest);
  Nota(um_,_g);delay(rest);
  Nota(um_,_f);delay(rest);
  Nota(um_,_e);delay(rest);
  Nota(um_,_d);delay(rest);
  Nota(um_,_c);delay(rest);*/

 delay(60000000L); 
}

void Nota(float frac,int nota)
{
  if(nota==_p)
  {
  delay((int)(tempo*frac*1000.0f));
  }
  else
  {
  Tone((int)(tempo*frac*1000.0f),nota);
  }
}
/*
 * NOTES
 * The program purports to hold a tone for 'duration' microseconds.
 *  Lies lies lies! It holds for at least 'duration' microseconds, _plus_
 *  any overhead created by incremeting elapsed_time (could be in excess of 
 *  3K microseconds) _plus_ overhead of looping and two digitalWrites()
 *  
 * As a result, a tone of 'duration' plays much more slowly than a rest
 *  of 'duration.' rest_count creates a loop variable to bring 'rest' beats 
 *  in line with 'tone' beats of the same length. 
 * 
 * rest_count will be affected by chip architecture and speed, as well as 
 *  overhead from any program mods. Past behavior is no guarantee of future 
 *  performance. Your mileage may vary. Light fuse and get away.
 *  
 * This could use a number of enhancements:
 * ADD code to let the programmer specify how many times the melody should
 *     loop before stopping
 * ADD another octave
 * MOVE tempo, pause, and rest_count to #define statements
 * RE-WRITE to include volume, using analogWrite, as with the second program at
 *          http://www.arduino.cc/en/Tutorial/PlayMelody
 * ADD code to make the tempo settable by pot or other input device
 * ADD code to take tempo or volume settable by serial communication 
 *          (Requires 0005 or higher.)
 * ADD code to create a tone offset (higer or lower) through pot etc
 * REPLACE random melody with opening bars to 'Smoke on the Water'
 */
