class V2
{
  float x=0;
  float y=0;

  V2()
  {
  }
  V2(float _x, float _y)
  {
    x=_x;
    y=_y;
  }
  float Mag()
  {
    return x*x+y*y;
  }
  V2 Menos(float _x, float _y)
  {
    return new V2(x-_x, y-_y);
  }
  V2 Menos(V2 outro)
  {
    return new V2(x-outro.x, y-outro.y);
  }
}
V2 V2(float _x, float _y)
{
  return new V2(_x, _y);
}
boolean retangulo(V2 a,V2 b,V2 m)
{
 return (m.x >= a.x - b.x/2 &&m.x <= a.x + b.x/2 &&m.y >= a.y - b.y/2 &&m.y <= a.y + b.y/2 ) ; 
}
boolean elipse(V2 c,float r,V2 m)
{
 return (c.Menos(m).Mag() <= r);
}

