void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if((Serial.available() > 0) )
  {
  delay(200);
printarsilabas(lerlinha());  
  }
}


String lerlinha()
{
String tex="";  // cria uma string
while(Serial.available() > 0) // enquanto houver mais bytes de entrada
{  
tex +=(char) Serial.read(); // converte o byte em um char e então o concatena à string
}
return tex; // retorna a linha inteira que foi digitada
}

void printarsilabas(String palavra)
{
String silaba="";
boolean inicvogal;
int contsil=0;
for(int i=0;i<palavra.length();i++)
{
char letra = palavra.charAt(i);
silaba += letra;
boolean vogal = evogal(letra);
if(silaba.length()==1)
{
//Serial.print("opa");  
inicvogal = vogal;

}
if(inicvogal&&!evogal(palavra.charAt(i+1)))
{ 
Serial.print("-");Serial.print(silaba);
silaba = "";
contsil++;
}
if(silaba.length()==2)
{
if(!inicvogal&&vogal&&enasal(palavra.charAt(i+1)))
{
silaba += palavra.charAt(i+1); i++;  
Serial.print("-");Serial.print(silaba);
silaba = "";
contsil++;
}
}
if(vogal == !inicvogal)
{
 

if(silaba.length()>=2)
{
Serial.print("-");Serial.print(silaba);
silaba = "";
contsil++;
}

}


}
if(silaba !="")
{
Serial.print("-");Serial.print(silaba);
silaba = "";
contsil++;
}
Serial.println();
}

boolean evogal(char letra)
{
if(letra == 'a'||letra == 'e'||letra == 'i'||letra == 'o'||letra == 'u')
{
return true;
}
else
return false;
}
boolean enasal(char letra)
{
if(letra == 'n'||letra == 'm')
{
return true;
}
else
return false;
}

