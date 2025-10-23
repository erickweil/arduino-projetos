   /** DEFINES
  */
  // Do-Re-Mi-Fa-So-La-Ti
  // frequencias em hertz
  #define c_  261
  #define d_  294
  #define e_  329
  #define f_  349
  #define g_  392
  #define a_  440
  #define b_  493
  #define C_  523
  
  // pwm time to manually manage the speaker
  #define _c  1915
  #define _d  1700
  #define _e  1519
  #define _f  1432
  #define _g  1275
  #define _a  1136
  #define _b  1014
  #define _C  956
  #define _p  0
  
  # define um_ 1.0
  # define mei_ 0.5
  # define qua_ 0.25
  # define oit_ 0.125
  # define u16_ 0.0625
  # define u32_ 0.03125
  # define u64_ 0.015625
  int notas[] = {_c,_d,_e,_f,_g,_a,_b,_C,_c,_d,_e,_f,_g,_a,_b,_C}; 
  
  /** VARIAVEIS GLOBAIS
  */
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
  
 // int sucess_melody[] = {
 // note_div*1, note_div*2, note_div*3, 
 // note_div*4, note_div*5, note_div*6,
 // note_div*7, note_div*8, note_div*9, 
 // note_div*10, note_div*11, note_div*12
 // };
 // int fail_melody[] = {
 // note_div*1, note_div*2, note_div*3, 
 // note_div*4, note_div*5, note_div*6,
 // note_div*7, note_div*8, note_div*9, 
 // note_div*10, note_div*11, note_div*12
  //};
  int T;
  int Dx =3;// dimensoes da grade no sentido x
  int Dy =4;// dimensoes da grade no sentido y
  boolean Tocar = true;
   
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

  
  /**--------------------------------------------------------AQUI O PROGRAMA È INICIADO
  #####################################################################################
  #####################################################################################
  */
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
    /**--------------------------------------------------------AQUI TUDO ACONTECE
  #####################################################################################
  #####################################################################################
  */
  int Note0;
  int Note1;
  int Note2;
  void loop()
  {
    /*for(int y =0; y< Dy;y++)
    {
      for(int x=0; x<Dx ; x++)
      {
        Led(x,y,HIGH);
        delay(100);
        Led(x,y,LOW);
      }
    }*/
    proxNote();
    delay(500);
  }
  void proxNote()
  {
  int temp0 = Note0;
  int qbit = random(0,6);
  int v = 0;
  bitSet(v,qbit);
  Note0 = v;
  int temp1 = Note1;
  Note1 = temp0;
  Note2 = temp1;
  BitLedSet(0,Note0);
  BitLedSet(1,Note1);
  BitLedSet(2,Note2);
  if(Tocar) Nota(oit_,notas[Note2]);
  }
  
  void Tone(int duration, int Tone) {
  switch(Tone)
  {
  case _c:tone(buzzer,c_,duration);break;
  case _d:tone(buzzer,d_,duration);break;
  case _e:tone(buzzer,e_,duration);break;
  case _f:tone(buzzer,f_,duration);break;
  case _g:tone(buzzer,g_,duration);break;
  case _a:tone(buzzer,a_,duration);break;
  case _b:tone(buzzer,b_,duration);break;
  case _C:tone(buzzer,C_,duration);break;
  }
  //delay(duration); 
}
  
  int Delay=125;
  void Led(int index,byte estado)
  {
      if(estado == 0)
      {
        digitalWrite(grade[index],0); 
        grade_cache[index] = 0;
      }
      else
      {
        digitalWrite(grade[index],1); 
        grade_cache[index] = 1;
      }
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
  void BitLedSet(int x,int bitmask)
  {
      for(int y=0;y<Dy;y++)
      {
      Led(x,y,bitRead(bitmask,y));
      }
  }


  float tempo = 1.75;
  void Nota(float frac,int nota)
  {
    if(nota==_p)
    {
      //delay((int)(tempo*frac*1000.0f));
    }
    else
    {
      Tone((int)(tempo*frac*1000.0f),nota);
    }
  }
