// programa que retorna a linha que você escreveu na porta serial como uma string

int led = 13; //Led a servir como sinalizador de estado
String texto=""; //variável a receber o texto de entrada
void setup()
{
pinMode(led,OUTPUT);//inicia o led
Serial.begin(9600);// permite a leitura da porta serial a 9600 byte/s
}
void loop()
{
if(Serial.available()>0)// examina se há algo na entrada serial
{ 
delay(200);//esperarfluxo(); // espera até que a entrada serial receba todos os bytes de entrada
digitalWrite(led,HIGH);  //liga o led
texto = ""; // limpa a variavel texto 
Serial.print("analisarei ");  
Serial.print(Serial.available());
Serial.print(" caracteres"); // faz uma linha que diz qual o tamanho da linha a ser analisada
texto = lerlinha(); // chama a função que converte cada byte de entra em um char e o concatena a string 'texto'
Serial.println();
Serial.print("foi escrito : \'");
Serial.print(texto); // mostra o texto de entrada
Serial.println("\'");
digitalWrite(led,LOW);  // apaga o led, mostra que terminou de examinar a entrada
}
}
//void esperarfluxo()
//{
//  int anterior = 0;  
//while(Serial.available()!=anterior)
//{
//Serial.print(",");
//anterior = Serial.available();
//delay((long)Serial.available()+10);  
//
//}
//}
String lerlinha()
{
String tex="";  // cria uma string
while(Serial.available() > 0) // enquanto houver mais bytes de entrada
{ 
Serial.print(".");  // dá um feedback de quantos caracteres foram analisados
tex +=(char) Serial.read(); // converte o byte em um char e então o concatena à string
}
return tex; // retorna a linha inteira que foi digitada
}



