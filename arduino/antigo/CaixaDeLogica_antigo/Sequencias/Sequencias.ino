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
   int buzzer = A4;
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

  void setup()
  {
  //Serial.begin(9600);  
    // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  T = sizeof(grade)/sizeof(*grade); // mesma coisa que grade.length em java  
  /*for(int x=0;x<Dx;x++)
  {
  for(int y=0;y<Dy;y++)
  {
  pinMode(P(x,y), OUTPUT);
  Serial.print("O led ");Serial.print(P(x,y));Serial.print(" foi setado na pos x:");Serial.print(x);Serial.print(" y:");Serial.println(y);
  }
  }
  */
  for(int i=0;i<12;i++)
  pinMode(grade[i], OUTPUT);
  
  for(int i=0;i<4;i++)
  { 
  pinMode(botoes[i], INPUT);
  //Serial.print("o botao ");Serial.print(botoes[i]);Serial.print(" foi setado no index i:");Serial.print(i);
  }
  pinMode(buzzer, OUTPUT);
  }
   boolean Tocar=true;
  long Delay = 0;
  int UltimaSeq=0;
  int QueSeq=0;
  #define NSeqs 4
  void loop()
  {
    Delay = 250;
    if(digitalRead(botoes[0]))
    {
      QueSeq = 1;
    }
    if(digitalRead(botoes[1]))
    {
      QueSeq = 2;
    }
    if(digitalRead(botoes[2]))
    {
      QueSeq = 3;
    }
    if(digitalRead(botoes[3]))
    {
      QueSeq = 4;
    }
    if(digitalRead(botoes[0])||digitalRead(botoes[1])||digitalRead(botoes[2])||digitalRead(botoes[3]))
    {
       Tocar=true;
       SetLeds(1);
        
        delay(250);
        SetLeds(0);
        delay(250);
    }
    if(UltimaSeq<1)
    UltimaSeq=NSeqs;
    if(UltimaSeq>NSeqs)
    UltimaSeq=1;
    
   
    switch(QueSeq)
    {
     case 0:
     Tocar=false;
     //SetLeds(grade_cache[0]==0);
     delay(10);
     break;
     case 1:seq_colunas(); UltimaSeq=QueSeq;
     SetLeds(1);
     break;
     case 2:seq_iteracoes(); UltimaSeq=QueSeq;
     SetLeds(1);
     break;
     case 3:seq_legal();
     SetLeds(1);
     break;
     case 4:seq_louco(); UltimaSeq=QueSeq;
     SetLeds(1);
     break;
    }
    QueSeq=0;
    
  }
  void seq_iteracoes()
  {
    /* LIGANDO e APAGANDO HORIZONTALMENTE */
    for(int i=0;i<12;i++)
    {
    Led(i,true);
    delay(100);
    }
    delay(250);
    for(int i=11;i>=0;i--)
    {
    Led(i,false);
    delay(100);
    }
    
    /* LIGANDO e APAGANDO VERTICALMENTE */
    for(int x=0;x<Dx;x++)
    {
      for(int y=0;y<Dy;y++)
      {
        Led(P(x,y),1);
        delay(100);
      }
    }
    delay(250);
    for(int x=0;x<Dx;x++)
    {
      for(int y=0;y<Dy;y++)
      {
        Led(P(x,y),0);
        delay(100);
      }
    }
  }
  void seq_colunas()
  {
    /* SETANDO TODAS AS LINHAS HORIZONTAIS */
    SetLeds(0);
    xLeds(0,1);
    delay(Delay);
    SetLeds(0);
    xLeds(1,1);
    delay(Delay);
    SetLeds(0);
    xLeds(2,1);
    delay(Delay);
    
    /* APAGADO POR UM POUCO */
    SetLeds(0);
    delay(Delay);
    
    /* SETANDO TODAS AS LINHAS VERTICAIS */
    SetLeds(0);
    yLeds(0,1);
    delay(Delay);
    SetLeds(0);
    yLeds(1,1);
    delay(Delay);
    SetLeds(0);
    yLeds(2,1);
    delay(Delay);
    SetLeds(0);
    yLeds(3,1);
    delay(Delay);
    
    /* APAGADO POR UM POUCO */
    SetLeds(0);   
    delay(Delay);
  }
  void seq_louco()
  {
    int NVezes = random(20,100);
    for(int i=0;i<NVezes;i++)
    {
    Leds_semdelay(random(0,2),random(0,2),random(0,2),random(0,2),random(0,2),random(0,2),random(0,2),random(0,2),random(0,2),random(0,2),random(0,2),random(0,2)); 
    delay(random(0,100)); 
    }
  }
  void seq_legal()
  {
    Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,1,0, 0,0,0, 0,0,0, 0,0,0);
    Leds(1,0,1, 0,1,0, 0,0,0, 0,0,0); Leds(0,0,0, 1,0,1, 0,1,0, 0,0,0); Leds(0,0,0, 0,0,0, 1,0,1, 0,1,0); Leds(0,0,0, 0,0,0, 0,0,0, 1,0,1);
    
    Leds(1,1,1, 0,0,0, 0,0,0, 0,0,0); Leds(1,0,1, 1,1,1, 0,0,0, 0,0,0); Leds(1,0,1, 1,0,1, 1,1,1, 0,0,0); Leds(1,0,1, 1,0,1, 1,0,1, 1,1,1);
    Leds(1,0,1, 1,0,1, 1,0,1, 1,0,1); Leds(0,0,0, 1,0,1, 1,0,1, 1,0,1); Leds(0,0,0, 0,0,0, 1,0,1, 1,0,1); Leds(0,0,0, 0,0,0, 0,0,0, 1,0,1);
    
    Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); 
    
    Leds(0,0,0, 0,1,0, 0,1,0, 0,0,0); Leds(0,0,0, 1,0,0, 0,0,1, 0,0,0); Leds(1,0,0, 0,0,0, 0,0,0, 0,0,1); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0);
    
    Leds(0,1,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,1,0, 0,1,0, 0,0,0, 0,0,0); Leds(0,1,0, 0,1,1, 0,0,0, 0,0,0); Leds(0,1,0, 0,1,1, 0,0,1, 0,0,0);
    Leds(0,1,0, 0,1,1, 0,0,1, 0,0,1); Leds(0,1,0, 0,1,1, 0,0,1, 0,1,1); Leds(0,1,0, 0,1,1, 0,1,1, 0,1,1); Leds(0,1,0, 0,1,1, 1,1,1, 1,1,1);
    Leds(0,1,0, 1,1,1, 1,1,1, 1,1,1); Leds(1,1,0, 1,1,1, 1,1,1, 1,1,1); Leds(1,1,1, 1,1,1, 1,1,1, 1,1,1); Leds(0,0,0, 1,1,1, 1,1,1, 1,1,1);
    Leds(0,0,0, 0,0,0, 1,1,1, 1,1,1); Leds(0,0,0, 0,0,0, 0,0,0, 1,1,1); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0);
    
    Leds(0,0,0, 1,0,0, 1,0,0, 0,0,0); Leds(0,0,0, 0,1,0, 0,1,0, 0,0,0); Leds(0,0,0, 0,0,1, 0,0,1, 0,0,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0);
    Leds(1,0,0, 0,0,0, 0,0,0, 1,0,0); Leds(0,1,0, 0,0,0, 0,0,0, 0,1,0); Leds(0,0,1, 0,0,0, 0,0,0, 0,0,1); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0);
    Leds(0,0,0, 1,0,0, 0,0,0, 1,0,0); Leds(0,0,0, 0,1,0, 0,0,0, 0,1,0); Leds(0,0,0, 0,0,1, 0,0,0, 0,0,1); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0);
    
    Leds(1,1,1, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 1,1,1, 0,0,0, 0,0,0); Leds(0,0,0, 0,0,0, 1,1,1, 0,0,0); Leds(0,0,0, 1,1,1, 0,0,0, 0,0,0);
    Leds(1,1,1, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 1,1,1, 0,0,0, 0,0,0); Leds(0,1,0, 1,0,0, 0,0,1, 0,0,0); Leds(0,0,0, 1,1,1, 0,0,0, 0,0,0);
    Leds(1,0,0, 0,1,0, 0,0,1, 0,0,0); Leds(1,1,0, 1,1,1, 0,1,1, 0,0,0); Leds(1,0,1, 0,1,0, 0,1,0, 0,1,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0);
    
    Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0); Leds(0,0,0, 0,0,0, 0,0,0, 0,0,0);
  }
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
