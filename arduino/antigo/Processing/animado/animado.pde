boolean clicoudireito;
void setup()
{
  size(600,400);


}


void draw()
{
  background(255); 
 update();
if(mousePressed && (mouseButton == RIGHT))
clicoudireito = true;

 if(clicoudireito)
 {
 new Objeto(randomforma(),V2(mouseX+random(50)-random(50),mouseY+random(50)-random(50)),V2(10+random(100),10+random(100)),color(random(255),random(255),random(255),random(255)));
 clicoudireito = false;   
 }
}
  void mouseReleased()
  {

  }
  
  String randomforma()
  {
    if(random(100)-random(100)>=0){return"circu";}else{return"rect";}
  }
