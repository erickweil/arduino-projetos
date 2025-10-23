#include <Comunicar.h>
const String usuarios[] = {"Erick","Miriam","Roberto"};
const String senhas[] = {"1a2b3c4d","123456","tijolo21"};
void setup()
{
Serial.begin(9600);// inicia a entrada serial a uma taxa de 9600 baud
}

void loop()
{
String nome="";
String senha="";
int i=0;
int tentativas=0;
Serial.println("Digite o seu Nome");
nome = Ler.proxlinha('!');
Serial.print(">> ");Serial.println(nome);
for(i=0;i<sizeof(usuarios);i++)
{
  if(usuarios[i] == nome)
  {
    if(tentativas == 0)
    {
    Serial.print("bom dia ");Serial.print(nome);Serial.println(", digite sua senha:");}
    senha = Ler.proxlinha('!');  
    Serial.print(">> ");Serial.println(senha);
    if(senhas[i] == senha)
    {
    Serial.println(">>--- Entrada Autorizada ---<<");
    delay(3600000);
    }
    else
    {
    Serial.println("sua senha nao bate com nossos registros, tente novamente");
    tentativas++;
    i--;
    }
    if(tentativas > 3)
    {
          Serial.println(">>--- Voce errou quatro vezes a senha aguarde um minuto ---<<");
          delay(60000);
          tentativas = 0;
    }
  }
}
Serial.println("Seu nome nao se encontra em nossos registros");

}

