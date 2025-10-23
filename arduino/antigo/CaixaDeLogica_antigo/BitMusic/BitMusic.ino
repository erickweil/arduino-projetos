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
  long note_div = 4098/256+4098/256;
  int On_toOFF_melody[] = {
  note_div*1, note_div*2, note_div*3, 
  note_div*4, note_div*5, note_div*6,
  note_div*7, note_div*8, note_div*9, 
  note_div*10, note_div*11, note_div*12
  };
  int Off_toON_melody[] = {
 note_div*1, note_div*2, note_div*3, 
  note_div*4, note_div*5, note_div*6,
  note_div*7, note_div*8, note_div*9, 
  note_div*10, note_div*11, note_div*12
  };
  int T;
  int Dx =3;// dimensoes da grade no sentido x
  int Dy =4;// dimensoes da grade no sentido y
  boolean Tocar = true;
  
  /**--------------------------------------------------------AQUI O PROGRAMA È INICIADO
  #####################################################################################
  #####################################################################################
  */
  void setup()
  {
 // Serial.begin(9600);  
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
  //Serial.print("o botao ");Serial.print(botoes[i]);Serial.print(" foi setado no index i:");Serial.println(i);
  }
  pinMode(buzzer, OUTPUT);
  }
    /**--------------------------------------------------------AQUI TUDO ACONTECE
  #####################################################################################
  #####################################################################################
  */
  void loop()
  {
    //Potenciade2(12);delay(2000);
    Incremento(12);delay(2000);
    //long valor =random(2,4096);
    //long valor2 =random(2,120);
    //Serial.print("Progressao geometrica de ");Serial.println(valor);Serial.println(" prog ");Serial.println(valor2);
    //ProgressaoGeometrica(12,valor,valor2);delay(200);
    //Fibonacci(12);
  }
  int Delay=125;
  void Led_seAcendeu(int index,byte estado)
  {
      if(estado == 0)
      {
        digitalWrite(grade[index],0); 
        grade_cache[index] = 0;
      }
      else
      {
        digitalWrite(grade[index],1); 
        if(grade_cache[index]==0)
        {
          if(Tocar)tone(buzzer,Off_toON_melody[index], Delay);
          grade_cache[index] = 1;
        }
      }
  }
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
        if(Tocar)tone(buzzer,Off_toON_melody[index], Delay);
        grade_cache[index] = 1;
      }
  }
  void BitMusic(long bitmask)
  {
      for(int i=0;i<12;i++)
      {
      Led(i,bitRead(bitmask,i));
      }
  }
  void BitAlternativeMusic(long bitmask)
  {
      for(int i=0;i<12;i++)
      {
      Led_seAcendeu(i,bitRead(bitmask,i));
      }
  }
  void Potenciade2(int i)
  {
    for(int simple_count=1;bitRead(simple_count,i)==0;simple_count=simple_count*2)
    {
        BitMusic(simple_count);
        delay(200);
    }
  }
  void Incremento(int i)
  {
    for(int simple_count=i;bitRead(simple_count,i)==0;simple_count=simple_count++)
    {
      BitAlternativeMusic(simple_count);
      delay(250);
    }
  }
  void ProgressaoGeometrica(int i,int prog1,int prog2)
  {
    int prog;
    int div;
    if(prog1>prog2)
    {
    prog = prog1;
    div = prog2;
    }
    else
    {
    prog = prog2;
    div= prog1;
    }
    for(int simple_count=prog;bitRead(simple_count,i)==0;simple_count=simple_count+simple_count/div)
    {
       Serial.print(simple_count); Serial.print(",");
       BitMusic(simple_count);
       delay(120);
    }
    Serial.println(".");
  }
  
  void Fibonacci(int i)
  {
    long Contagem=0;
    long tempo = 1;
    long tempoa = 1;
    for(int fibonacci=0;bitRead(Contagem,31)==0;fibonacci++)
    {
      BitMusic(Contagem);
      long temp = tempo;
      tempo += tempoa;
      tempoa = temp;
      Contagem=tempo;
      Delay=250;
      delay(Delay);
    }
     delay(2000);

  }
