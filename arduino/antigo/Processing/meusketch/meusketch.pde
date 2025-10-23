import processing.serial.*;

Serial myPort;  // Create object from Serial class

void setup() 
{
  size(200,100); //make our canvas 200 x 200 pixels big
  String portName = Serial.list()[1]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
}
void draw() {                         //if we clicked in the window
   myPort.write((width-mouseX)/10);         //send a 1
if(mousePressed)
myPort.write(100);//otherwise  
}
