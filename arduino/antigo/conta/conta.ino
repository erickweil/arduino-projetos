// programa que calcula uma expressão numérica
const int led = 13; //Led a servir como sinalizador de estado
byte umbyte;
void setup()
{
pinMode(led,OUTPUT);//inicia o led
Serial.begin(9600);// permite a leitura da porta serial a 9600 bits/s
Serial.println("Escreva uma expressao numerica, por exemplo : 1+34.32+203/(10.5-4*8/10)");
Serial.println("sao aceitos os operandos : + - * / ^ e os parenteses para ordenar a conta");
Serial.println("com um limite de 63 caracteres e o limite de digitos do float");
}
void loop()
{
if(Serial.available()>0)// examina se há algo na entrada serial
{ 
delay(200);// espera até que a entrada serial receba todos os bytes de entrada
digitalWrite(led,HIGH);  //liga o led
Serial.print("analisarei ");Serial.print(Serial.available());Serial.print(" caracteres"); // faz uma linha que diz qual o tamanho da linha a ser analisada
printnum(fazerconta());Serial.println(" <--- resultado");Serial.println(); // exibe o resultado da conta
digitalWrite(led,LOW);  // apaga o led, mostra que terminou de examinar a entrada
}
}

// funcao que retorna o resultado da expressao que esta esperando na porta serial
float fazerconta()
{
float res=0;float t=0;
res = proxnum();// chama o proximo numero que esta esperando na serial
while(Serial.available() > 0) // enquanto houver mais bytes de entrada
{
  while(umbyte == ' ')
  umbyte =Serial.read();
  switch(umbyte)
  {
  case ')': 
   umbyte=Serial.read();
   return res; // retorna o resultado se encontrar o fim de parenteses 
  break;
  case '+': // caso encontrar o sinal de soma executar a soma
    // /*
    t = proxnum(); // armazena o proximo numero na variavel t
    Serial.println();printnum(res);Serial.print("+");printnum(t); // da o feedback
    res += t; // e faz a soma do proximo numero com o resultado
    Serial.print("=");printnum(res);
    // */ 
    //res += proxnum(); // outra possibilidade sem nenhum print de estado
  break;
  case '-': // caso encontrar o sinal de subtração executa a subtração
    // /*  
    t = proxnum();
    Serial.println();printnum(res);Serial.print("-");printnum(t);
    res -= t;
    Serial.print("=");printnum(res);
    // */   
    //res -= proxnum(); 
  break;
  case '*': // caso encontrar o sinal de vezes, faz a multiplicação
    // /*   
    t = proxnum();
    Serial.println();printnum(res);Serial.print("*");printnum(t);
    res *= t;
    Serial.print("=");printnum(res);
    // */
    //res *= proxnum(); 
  break;
  case '/':// caso encontrar o sinal de divisão faz a divisão 
    // /*  
    t = proxnum();
    Serial.println();printnum(res);Serial.print("/");printnum(t);
    if(t != 0)
    {
    res /= t;
    }
    else
    {
    Serial.println();Serial.println("--------------------------------");Serial.println("Ei, divisao por zero aqui!");Serial.println("--------------------------------");
    }
    Serial.print("=");printnum(res);
    // */
    //res /= proxnum(); 
  break;
  case '^': // caso encontrar esse sinal faz a potenciação  
    // /*  
    t = proxnum();
    Serial.println();printnum(res);Serial.print("^");printnum(t);
    if(!(res<0 && t<1))
    {
    res = pow(res,t);
    }
    else
    {
    Serial.println();Serial.println("--------------------------------");Serial.println("Raiz de numeros negativos nao sao permitidos! numeros imaginarios?");Serial.println("--------------------------------");
    }
    Serial.print("=");printnum(res);
    // */ 
    //res = pow(res,proxnum()); 
  break;
  default :
    Serial.println();Serial.println("--------------------------------");
   Serial.print("encontrei algo errado! isso nao deveria estar aqui:");Serial.print(" \'");Serial.write(umbyte);Serial.println("\'");
    Serial.println("--------------------------------"); 
    umbyte = Serial.read(); // continua para o proximo byte;
  break;
  }
}
Serial.println();
return res; // retorna o resultado
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
if(num == ""){
Serial.print(" ---- Faltou um numero aqui! ----- ");return(0);}
return(atof(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atof é abreviatura de "ASCII to Float" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atof) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um float
*/
}
void printnum(float num)
{
  int n = (int)num;
  float d = num-n;
  if(d != 0)
  {
  if(d > 0.1||d <= -0.1)  
  Serial.print(num,2);
  else if(d >= 0.01||d <= -0.01)  
  Serial.print(num,3); 
  else if(d >= 0.001||d <= -0.001)  
  Serial.print(num,4); 
  else if(d >= 0.0001||d <= -0.0001)  
  Serial.print(num,5); 
  else if(d >= 0.00001||d <= -0.00001)  
  Serial.print(num,6); 
  else if(d >= 0.000001||d <= -0.000001)  
  Serial.print(num,8);   
  }
  else
  {
  Serial.print(n);  
  }
}

