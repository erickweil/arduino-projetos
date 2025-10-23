// programa que retorna a linha que você escreveu na porta serial como uma string

int led = 13; //Led a servir como sinalizador de estado
#define BUFFER_LENGTH 80
char buffer[BUFFER_LENGTH];
int c;
void setup()
{
  pinMode(led,OUTPUT);//inicia o led
  Serial.begin(9600);// permite a leitura da porta serial a 9600 byte/s
  Serial.println(F("Hello Servidor!"));
}


void loop()
{
//  if (readline() > 0) {
//    Serial.print(">");
//    Serial.print(buffer);
//  }
Serial.println(analogRead(A0));
delay(100);
}

int readline()
{
return readuntil('\n');
}

int readuntil(char until)
{
  if(Serial.available() > 0)
  {
    c=0;
    while(Serial.available()==0||((buffer[c++] = Serial.read()) != until))
    {
    // do nothing
    }
    while(Serial.available()> 0)
    {
      // garbage incoming, have reached end already
      Serial.read();
    }
    int length = c;
    while(c < BUFFER_LENGTH)
    {
      // clean previous reads
      buffer[c++] = '\0';
    }
    return length;
  }
  else return -1;
}

