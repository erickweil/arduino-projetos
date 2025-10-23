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
   int buzzer = 1;
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
  long Delay = 250;
  void setup()
  {
    Delay = 250;
    //Serial.begin(9600);  
      // if analog input pin 0 is unconnected, random analog
    // noise will cause the call to randomSeed() to generate
    // different seed numbers each time the sketch runs.
    // randomSeed() will then shuffle the random function.
    randomSeed(analogRead(0));
    T = sizeof(grade)/sizeof(*grade); // mesma coisa que grade.length em java  
    // 12 leds
    for(int i=0;i<12;i++)
    {
      pinMode(grade[i], OUTPUT);
    }
    // 4 botoes
    for(int i=0;i<4;i++)
    { 
      pinMode(botoes[i], INPUT);
      //Serial.print("o botao ");Serial.print(botoes[i]);Serial.print(" foi setado no index i:");Serial.print(i);
    }
    pinMode(buzzer, OUTPUT);
  }
  boolean Tocar=false;  
  // Joguinho de coletar o LED
  
  int Jogador_x = 0;
  int Jogador_y = 0;
  int piscar = 1;
  
  int Moeda_x = 0;
  int Moeda_y = 0;
  
  int Jogador_vel = 250;
  
  int Score = 0;
  
  int esta_apertando = 0;
  
  void loop()
  {
    // so mexe o jogador 1 vez ao pressionar, deve soltar o botão e apertar denovo para mexer mais uma vez
    if(esta_apertando == 0)
    {
      if(digitalRead(botoes[0]))
      {
        Jogador_x++;
        esta_apertando = 1;
      }
      if(digitalRead(botoes[1]))
      {
        Jogador_x--;
        esta_apertando = 1;
      }
      if(digitalRead(botoes[2]))
      {
        Jogador_y--;
         esta_apertando = 1;
      }
      if(digitalRead(botoes[3]))
      {
        Jogador_y++;
        esta_apertando = 1;
      }
    }
    else
    {
       // checa se ainda está pressioando o botão ou se soltou já.
       esta_apertando = digitalRead(botoes[0]) || digitalRead(botoes[1]) || digitalRead(botoes[2]) || digitalRead(botoes[3]);
    }
    
    // não permite que o jogador saia da grade
    Jogador_x = max(0,Jogador_x);
    Jogador_x = min(2,Jogador_x);
    Jogador_y = max(0,Jogador_y);
    Jogador_y = min(3,Jogador_y);
    
    Moeda_x = max(0,Moeda_x);
    Moeda_x = min(2,Moeda_x);
    Moeda_y = max(0,Moeda_y);
    Moeda_y = min(3,Moeda_y);
    
    // se o jogador for em cima da moeda, troca o lugar dela
    if(Moeda_x == Jogador_x && Moeda_y == Jogador_y)
    {
      // um valor randomico que pode ser 0,1 ou 2
      Moeda_x = random(0,3);
      // pode ser 0,1,2 ou 3
      Moeda_y = random(0,4);
      
      tone(buzzer,Off_toON_melody[1], Delay);
      Tocar = true; 
      // preenche a tela tocando musica
      SetLeds(1);
      SetLeds(0);
      Tocar = false;
      
      Score++;
    }
    
    // apaga todos os leds
    SetLeds(0);
    //acende o led do jogador
    Led(Jogador_x,Jogador_y,1);
    //acende o led da moeda, de acordo com o piscar dela
    piscar = piscar == 0 ? 1 : 0;
    Led(Moeda_x,Moeda_y,piscar);
    delay(100);
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
     Leds(e,e,e,e,e,e,e,e,e,e,e,e);
  }
