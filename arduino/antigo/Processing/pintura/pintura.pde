float escala=1;
// a função setup executa uma única vez
void setup() {
// define o tamanho em pixels do canvas  
size(500,400, P2D);
}
// a função draw executa continuamente, ao fim retorna ao começo
// até que o programa seja parado ou ocorra um erro
void draw() {
// stroke é a linha que contorna a forma
// fill é o preenchimento da forma

// as cores são um byte
// 0 é preto, e 255 é branco, tudo entre isso será escala de cinza
// colocando um número, será cinza, mas com 3 números
// será R G B onde cada valor será a quantidade de cada cor

// podemos pensar que cada um dos valores RGB são como lanternas,
// e os valores é a quantidade que cada uma será ligada
//background(255, 255, 255); // aqui toda a tela é preenchida com branco
stroke(0, 0, 0);
fill(0, 255, 0);



//  Questão
if(mousePressed)
{// se a resposta for verdadeira, SIM
//    x   y   w   h
  pushMatrix();
  translate(pmouseX,pmouseY);
  scale(escala);
  float ax = (pmouseX*1.0)/escala;
  float ay = (pmouseY*1.0)/escala;
  float x = (mouseX*1.0)/escala;
  float y = (mouseY*1.0)/escala;
  translate(-ax,-ay);
  line(ax,ay,x,y);
  popMatrix();
   if(escala < 20)
   escala+=(x-ax)*(x-ax)+(y-ay)*(y-ay);
}
else
{
escala = 1;  
}

  //noStroke();

  // draw the original position in gray
  //fill(192);
  //rect(20, 20, 40, 40);
  
  // draw a translucent red rectangle by changing the coordinates
//  fill(255, 0, 0, 128);
//  rect(20 + 60, 20 + 80, 40, 40);
//  
//  // draw a translucent blue rectangle by translating the grid
//  fill(0, 0, 255, 128);
//  pushMatrix();
//  translate(mouseX,mouseY);
//  rect(20, 20, 40, 40);
//  popMatrix();

  
}
