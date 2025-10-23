import processing.serial.*;
import java.io.FileWriter;
import java.io.BufferedWriter;
Serial myPort;  // Create object from Serial class
char val;
String data;      // Data received from the serial port
float temp;
String lido;
Grafico graf;
PrintWriter output;
void setup() 
{
  size(720, 350);
  graf = new Grafico(60,0,50);
  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  println(Serial.list());
  
 // output = createWriter("leituras.txt");
 try {
  String path = dataPath("leituras.txt");
  output = new PrintWriter(new BufferedWriter(new FileWriter(path, true)));
  String portName = Serial.list()[1];
  myPort = new Serial(this, portName, 9600);
  lido = " ";
  temp = 0.0f;
  data = "";
  output.println("<leitura --> "+day()+"/"+month()+"/"+year()+" "+hour()+":"+minute()+":"+second()+">");
  output.flush(); 
    }
  catch (IOException e) { 
    println(e);
  }
}
int segs=0;
boolean primeiraleitura=true;
void draw()
{
  if(myPort.available() >0)
  {
  lido = ""; 
  while( myPort.available() > 0) {  // If data is available,
    val = (char)myPort.read();         // read it and store it in val
    lido += val;
    delay(5);
  }
  String[] m = match(lido, "^\\d+\\.\\d+");
  if(m != null && m.length>0)  
  {
  println("matched! :'"+m[0]+"'"); 
  temp = Float.parseFloat(m[0]);
  if(primeiraleitura)
  {
    primeiraleitura = false;
    graf.SetarTudo(temp);
  }
  else
  graf.addValue(temp);
  segs++;
  if(segs>=60)
  {
   println("Escreveu a media");   
  segs=0;
  Write(graf.Media());  
  }
  }
  }
  background(255);             // Set background to white
 
  fill(1);                 // set fill to light gray
  text("Temperatura : "+temp+" Graus Celcius",0,10);
  text("serial input:"+lido,0,40);
  graf.DrawGraph(0,60,width,height-60);

  
}
void Write(float n)
{
  output.println(n);
  output.flush(); 
}
