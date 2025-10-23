public class Grafico
{
public float[] leituras;
public int Resolucao;
public float MIN= 10;
public float MAX= 40;
public Grafico(int res,float min,float max)
{
this.Resolucao = res;
this.MIN = min;
this.MAX = max;
leituras = new float[res+60];
}
void DrawGraph(int posx, int posy, int largura, int altura)
{
  AdjustGraphScale();
  fill(200,255,200);
  rect(posx,posy,largura,altura);
  fill(0);
  stroke(255,255,255);
  DrawGraphInfos(posx,posy,largura,altura);
 // DrawLines(posx,posy,largura,altura,false);
  DrawLines(posx,posy,largura,altura,true);
}
void DrawLines(int posx, int posy, int largura, int altura,boolean smooth)
{
  noFill();
  if(smooth)
  stroke(255,0,0);
  else
  stroke(0,0,0);
  beginShape();
  for (int i = 0; i < leituras.length; i++) {
    float maped = map(leituras[i], MIN, MAX, posy+altura, posy);
    float offset = map(i, 0,leituras.length,posx,posx+largura);
    if(smooth)
    curveVertex(offset,maped);
    else
    vertex(offset,maped);
  }
  endShape(); 
}

void DrawGraphInfos(int posx, int posy, int largura, int altura)
{
  for(int i=0;i<10;i++)
  {
  float offset = map(i, 0,10,posy,posy+altura);  
  line(posx,offset,posx+largura,offset);  
  }
    for(int i=0;i<10;i++)
  {
  float offset = map(i, 0,10,posx,posx+largura);  
  line(offset,posy,offset,posy+altura);  
  }
}

void AdjustGraphScale()
{
  float min=MAX;
  float max=MIN;
  for(int i=0;i<leituras.length;i++)
  {
    if(leituras[i] > max)
    {
     max = leituras[i]; 
    }
    if(leituras[i] < min)
    {
     min = leituras[i];
    }
  }
  MAX = max+3;
  MIN = min-3;
}
float Media()
{
float tudo=0;  
for(int i=0;i<60;i++)
{
tudo += leituras[i];  
}
return tudo/60.0f;
}
void addValue(float val)
{
float[] antigo = leituras.clone();  
leituras = new float[leituras.length];
for(int i=0;i<leituras.length-1;i++)
{
leituras[i] = antigo[i+1];  
}
leituras[leituras.length-1] = val;
}
void SetarTudo(float val)
{
 for(int i=0;i<leituras.length;i++)
{
leituras[i] = val;  
} 
}
}
