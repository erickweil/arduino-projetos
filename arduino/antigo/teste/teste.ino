
char expdois(char op,char um,char outro,boolean &ovf);
String numero = "1";
//String numeroa = "1";
int i;
void setup()
{
pinMode(13,OUTPUT);  
Serial.begin(115200);  
}
void loop()
{
//String temp = numero;
A();
somar(numero,numero);
//numeroa = temp;
i++;
if(i<=1001)
{
Serial.print("2^");Serial.print(i);Serial.print(" : ");
Serial.println(numero);

}
else
{
while(true)
{
}  
}
//delay(100);
}
void A()
{
if(digitalRead(13) == LOW)  
digitalWrite(13,HIGH);
else
digitalWrite(13,LOW);
}
void somar(String num1,String num2)
{
boolean ovf;
for(int a=num2.length()-1;a>=0;a--)
{
char b;  
int i =a+(num1.length()-num2.length());

b = num2.charAt(a);

do
{
ovf = false;
num1.setCharAt(i,expdois('+',num1.charAt(i),b,ovf));
if(i>=0)
i--;
else
{
num1 = "0"+num1;
i++;
}
b = '1';
}
while(ovf);
}
numero = num1;
}

char expdois(char op,char um,char outro,boolean &ovf)
{
 // Serial.write(um);  Serial.write(op);  Serial.write(outro);  
switch(op)
{
  case '+': // caso encontrar o sinal de soma executar a soma
  um += outro-'0';// outra possibilidade sem nenhum print de estado
  break;
  case '-': // caso encontrar o sinal de subtração executa a subtração
  um -= outro; 
  break;
  case '*': // caso encontrar o sinal de vezes, faz a multiplicação
  
  break;
  case '/':// caso encontrar o sinal de divisão faz a divisão 
  
  break;
  case '^': // caso encontrar esse sinal faz a potenciação  
  
  break;
  default :
  Serial.print("encontrei algo errado! isso nao deveria estar aqui:");Serial.write(op);
  break; 
}
while(um < '0' || um > '9')
{
if(um < '0')  
um += 10;
else
um -= 10;
ovf = true;
}
return um;
}
