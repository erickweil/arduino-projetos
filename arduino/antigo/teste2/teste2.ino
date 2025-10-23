
char expdois(char op,char um,char outro,boolean &ovf);
String numero = "23";
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
somar(numero,"50",'-');
//numeroa = temp;
i++;
//if(i<=1001)
//{
//Serial.print("2^");Serial.print(i);Serial.print(" : ");
//Serial.println(numero);

//}
//else
//{
//while(true)
//{
//}  
//}
delay(1000);
}
void A()
{
if(digitalRead(13) == LOW)  
digitalWrite(13,HIGH);
else
digitalWrite(13,LOW);
}
void somar(String num1,String num2,char op)
{
if(num1.charAt(0) == '-')
{
if(op=='-')  
op = '+';
else
op = '-';
num1.setCharAt(0,0);
}
String anum1 = num1;  
boolean ovf;
Serial.print("Vamos calcular ");Serial.print(num1);Serial.print(op);Serial.println(num2);
for(int a=num2.length()-1;a>=0;a--)
{
char b;  
int i =a+(num1.length()-num2.length());

b = num2.charAt(a);

do
{
Serial.println(num1);  
ovf = false;
num1.setCharAt(i,expdois(op,num1.charAt(i),b,ovf));
if(i>=0)
i--;
else
{
if(op == '-')
{
Serial.println("Numero negativo!!");
somar(num2,anum1,'-');
numero = "-"+numero;
return;
}
else
{
num1 = "0"+num1;
i++;
}
}
b = '1';
}
while(ovf);
}
numero = num1;
Serial.print("e o resultado: ");Serial.println(numero);
}

String maior(String num1,String num2)
{
  
}
char expdois(char op,char um,char outro,boolean &ovf)
{
  Serial.write(um);  Serial.write(op);  Serial.write(outro);Serial.print("=");  
switch(op)
{
  case '+': // caso encontrar o sinal de soma executar a soma
  um += outro-'0';// outra possibilidade sem nenhum print de estado
  break;
  case '-': // caso encontrar o sinal de subtração executa a subtração
  um -= outro-'0'; 
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
Serial.print(um);
if(ovf)
Serial.print(" e vai um");
Serial.println();
delay(500);
return um;
}
