// programa que calcula uma expressão numérica
//const int led = 13; //Led a servir como sinalizador de estado
byte umbyte;
String comandos[] = {"pinMode","digitalWrite","digitalRead"};
void setup()
{
//pinMode(led,OUTPUT);//inicia o led
Serial.begin(9600);// permite a leitura da porta serial a 9600 bits/s
}
void loop()
{
if(Serial.available()>0)// examina se há algo na entrada serial
{ 
delay(100);// espera até que a entrada serial receba todos os bytes de entrada
//digitalWrite(led,HIGH);  //liga o led
Entender();
//Serial.print(fazerconta());Serial.print(" <--- resultado ");Serial.println(); // exibe o resultado da conta
//digitalWrite(led,LOW);  // apaga o led, mostra que terminou de examinar a entrada
}
}

void Entender()
{
//umbyte = Serial.read();  
String palavra = proxlinha('(');
for(byte i=0;i<sizeof(comandos);i++)
{
if(comandos[i] == palavra)
{
//Serial.print("Executando ");Serial.println(comandos[i]);  
Executar(i);
}
}
}

void Executar(byte qual)
{
  int pin; 
  String afr;
switch(qual)
{
 
case 0:
pin =(int)fazerconta();
if(pin >= 2)
{
if(proxlinha(')') == "OUTPUT")
{
Serial.print("Setando o pino ");Serial.print(pin);Serial.println(" como Saida de dados");   
pinMode(pin,OUTPUT);
}
else
{
Serial.print("Setando o pino ");Serial.print(pin);Serial.println(" como Entrada de dados"); 
pinMode(pin,INPUT);
}
}
else
{
Serial.println("os pinos 0 e 1 sao para entrada/saida de dados");  
}
break;
case 1:
pin =(int)fazerconta();
afr = proxlinha(')');
if(afr == "HIGH"||afr == "true"||afr == "1")
{
Serial.print("Setando o pino ");Serial.print(pin);Serial.println(" em valor logico Alto"); 
digitalWrite(pin,HIGH);
}
else
{
Serial.print("Setando o pino ");Serial.print(pin);Serial.println(" em valor logico Baixo"); 
digitalWrite(pin,LOW);
}
break;
case 2:
pin =(int)fazerconta();
Serial.print("o pino ");Serial.print(pin);Serial.print(" esta com valor ");Serial.println(digitalRead(pin));
break;
}
}
String proxlinha(char div)
{
String tex="";  // cria uma string
umbyte = Serial.read();
while(Serial.available() > 0&&(char)umbyte!=div)
{// enquanto houver mais bytes de entrada
tex +=(char) umbyte; // converte o byte em um char e então o concatena à string
umbyte = Serial.read();
}
if(!Serial.available())
tex+=(char) umbyte;
//Serial.println(tex);
return tex; // retorna a linha inteira que foi digitada  
}
// funcao que retorna o resultado da expressao que esta esperando na porta serial
float fazerconta()
{
float res=0;
res = proxnum();// chama o proximo numero que esta esperando na serial
while(Serial.available()) // enquanto houver mais bytes de entrada
{
  while(umbyte == ' ')
  umbyte =Serial.read();
  switch(umbyte)
  {
  case ')': 
   umbyte=Serial.read();
   return res; // retorna o resultado se encontrar o fim de parenteses 
  break;
    case ',': 
   //umbyte=Serial.read();
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
  default :
  Serial.print("Deu errado");
    umbyte = Serial.read(); // continua para o proximo byte;
  break;
  }
}
return res;
}

// Funçao que recebe o proximo numero disponível
float proxnum()
{  
umbyte =Serial.read();// Lê o prox byte na porta serial
while(umbyte == ' ')
umbyte =Serial.read();
String num="";
if(umbyte == '(') 
return(fazerconta()); // examina se encontrou um inicio de parenteses, e se sim, retorna o valor da conta dentro desse parenteses
if(umbyte == '-'||umbyte == '+')
{
num += (char)umbyte;
umbyte= Serial.read();
}
while(isDigit(umbyte)||umbyte=='.'||umbyte==' ') // enquanto os bytes que forem lidos ainda forem digitos ou um ponto
{
if(umbyte != ' ')  
num += (char)umbyte; // converte o byte ASCII em um char, e então concatena a string 'num'
umbyte = Serial.read(); // Lê o prox byte na porta serial
}
if(num == "")
return(0);
return(atof(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atof é abreviatura de "ASCII to Float" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atof) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um float
*/
}


