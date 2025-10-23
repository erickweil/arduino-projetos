static int index=0;
static Objeto[] TObj=new Objeto[5000];
class Objeto {
  String formato="rect"; 
  V2 escala = V2(10, 10);
  V2 pos= V2(0, 0);
  V2 dim= V2(10, 10);
  float rot=0.0;
  color cor=color(0, 0, 0);
  boolean clicado=false;

  void asignar()
  {
    TObj[index] = this;
    println(" no index "+index+" ");
    index++;
  }
  Objeto(String _f, V2 _pos, V2 _dim)
  {  
    formato = _f;
    pos = _pos;
    dim = _dim;
    print("novo "+formato+" criado em ("+pos.x+","+pos.y+") de dimensões ("+dim.x+","+dim.y+")"); 
    asignar();
  }
  Objeto(String _f, V2 _pos, V2 _dim, color _cor)
  {
    formato = _f;
    pos = _pos;
    dim = _dim;
    cor = _cor;
    print("novo "+formato+" criado em ("+pos.x+","+pos.y+") de dimensões ("+dim.x+","+dim.y+") e RGB="+cor); 
    asignar();
  }
  Objeto(String _f, V2 _pos, float _rot, V2 _dim, color _cor)
  {
    formato = _f;
    pos = _pos;
    dim = _dim;
    cor = _cor;
    rot = _rot;
    print("novo "+formato+" criado em ("+pos.x+","+pos.y+") de dimensões ("+dim.x+","+dim.y+") e RGB="+cor+" rotacionado "+rot+" graus em sentido horário"); 
    asignar();
  }

  void draw()
  {
        fill(cor);
    stroke(cor);
        V2 m = V2(mouseX, mouseY);
    if (clicado)
    {
      pos.x = m.x;
      pos.y = m.y;
    }



    pushMatrix();
    translate(pos.x, pos.y);
    rotate(radians(rot));
    scale(escala.x, escala.y);
    float ax = pos.x/escala.x;
    float ay = pos.y/escala.y;
    float x = (dim.x)/escala.x;
    float y = (dim.y)/escala.y;
    translate(-ax, -ay);

    if (formato=="rect")
    {
      if (retangulo(pos,dim,m)) 
      {
        if (mousePressed && mouseButton==LEFT)
        {
          stroke(255, 175, 0);
          fill(255, 255, 255);
          clicado = true;
        } else
        {
          stroke(145, 250, 0);
          fill(162, 255, 254);  
          clicado = false;
        }
      } else
      {   
        clicado = false;
      }
      rectMode(CENTER);  
      rect(ax, ay, x, y);
    }
    if (formato=="circu")
    {
       if(elipse(pos,dim.Mag()/4,m)) 
      {
        if (mousePressed&& mouseButton==LEFT)
        {
          stroke(255, 175, 0);
          fill(255, 255, 255);
          clicado = true;
        } else
        {
          stroke(145, 250, 0);
          fill(162, 255, 254);  
          clicado = false;
        }
      } else
      {   
        clicado = false;
      }
      ellipse(ax, ay, x, y);
    }
    popMatrix();
  }
}

void update()
{
  fill(0);  
  text(index+" Objetos na cena", 0, 10); 
  for (int i=0; i<index; i++)
  {
    Objeto esse = TObj[i];  
    esse.draw();
  }
}
void destruir(int i)
{
  if (index>1)
    TObj[i] = TObj[index-1];
  if (index>0)
    index--;
}

