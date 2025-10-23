float[] x=new float[1000];
float[] y=new float[1000];
int v=0;
boolean clicou=false;
int mover=0;
float pmovx =0;
float pmovy =0;
void setup()
{
size(500,500);  
  
}


void draw() {
  if(mousePressed)
  {
    if(!clicou&&mouseButton==RIGHT)
    {
    pmovx = mouseX;
    pmovy = mouseY;
    for(int i=1;i<=v;i++)
    {
    if((pmovx-x[i])*(pmovx-x[i])+(pmovy-y[i])*(pmovy-y[i])<40)
    mover = i;  
    }
    }
    if(!clicou)
    {
      
    if(mouseButton==LEFT)
  {  
    if(v<1000)  
    v++;
    x[v] = mouseX;
    y[v] = mouseY;
    mover=-1;
  }
    clicou = true; 
   }
    if(mouseButton == CENTER)
    v=0;
  }
  else
  {
  mover = -1;  
  clicou = false;
  }


  background(51);
  fill(255,0,0);
   if(mover != -1)
 {
   ellipse(x[mover],y[mover],10,10);
    x[mover] = mouseX;
    y[mover] = mouseY;  
 }
 // translate(mouseX, mouseY);
  noFill();
  
  stroke(255);
  strokeWeight(1);
 
  for(int i=1;i<=v;i++)
  {
    strokeWeight(1);
    fill(0,255,0);
    ellipse(x[i],y[i],5,5);
    
    stroke(40);
    fill(5,5,5,10);
    ellipse(x[i],y[i],120,120);
    noFill();
    stroke(255);
      for(int k=1;k<=i;k++)
      {
      float d =  (x[k]-x[i])*(x[k]-x[i])+(y[k]-y[i])*(y[k]-y[i]);

      if(i!=k)
      {
        
    if(d>3900&&d<8000)
    {
      x[i] -= (x[i]-x[k])/(d/100);
      x[k] -= (x[k]-x[i])/(d/100);
      y[i] -= (y[i]-y[k])/(d/100);
      y[k] -= (y[k]-y[i])/(d/100);
      beginShape();
      strokeWeight(2000/d);
      if(d<100)
      strokeWeight(30);
      stroke(0,100,0);
      vertex(x[i],y[i]);
      vertex(x[k],y[k]);
      endShape();
      stroke(255);
    }
          if(d<3900)
    {  
      x[i] += (x[i]-x[k])/(d/10);
      x[k] += (x[k]-x[i])/(d/10);
      y[i] += (y[i]-y[k])/(d/10);
      y[k] += (y[k]-y[i])/(d/10);
    

      beginShape();
      strokeWeight(2000/d);
      if(d<100)
      strokeWeight(30);
      vertex(x[i],y[i]);
      vertex(x[k],y[k]);
      endShape();
    }
    }
      }
      }
  }
  

