//#include <MemoryFree.h>
//#include <avr/pgmspace.h>
#include <TFTLCD7781.h>
#include <TouchScreen.h>
#include <Vector3.h>
// *********************************************************** Declarações iniciais de valores fixos ****************************************************************************************

#define YP A2  
#define XM A1 
#define YM 6 
#define XP 7   

#define TS_MINX 120
#define TS_MINY 120
#define TS_MAXX 930
#define TS_MAXY 950

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 400);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0 
#define LCD_RESET A4

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF
#define GREY            0xBDF7
#define GRAY            0x7BEF
#define MARROM          0x79E0
#define AZULADO         0xB71C
TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define MINPRESSURE 10
#define MAXPRESSURE 5000
#define W 320
#define H 240
#define WIDTH 320
#define HEIGHT 240
#define WIDTHf 320.0f
#define HEIGHTf 240.0f

#define MAXSTEPS 32
//________________________________________________________AQUI TEMOS A INICIALIZAÇAO,                           ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

int cR = 0;
int cG = 0;
int cB = 255;

Vector3 normal;
Vector3 p;
Vector3 cam;
#define HIT_FARAWAY 0
#define HIT_INSIDE 1
#define HIT_SURFACE 2	
#define HIT_NOTHING 3
void setup(void) 
{
  tft.reset();
  tft.initDisplay(); 
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  tft.fillScreen(WHITE);
  
  cam.set(0.0f,0.0f,-3.0f);
  RenderAll(32);
  RenderAll(16);
  RenderAll(8);
  RenderAll(4);
  RenderAll(1);
}

void RenderAll(int MAG)
{
   int MX = W/MAG;
   int MY = H/MAG;
   for(int x =0;x<MX;x++)
   {
	   for(int y =0;y<MY;y++)
	   {
	       Render(x*MAG,y*MAG,MAG);
	   }
   }
}
//________________________________________________________AQUI TEMOS O LOOP PRINCIPAL, ONDE TUDO ACONTECE       ___________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
void loop()
{
  Point p = ts.getPoint();                                        
  pinMode(XM, OUTPUT);                                             
  pinMode(YP, OUTPUT);                                               
  if (p.z > MINPRESSURE ) // verifica se o toque está dentro da margem de erro 
  {
   // turn from 0->1023 to tft.width                                
   int y = H-map(p.y, TS_MINX, TS_MAXX, H, 0);                        
   int x = map(p.x, TS_MINY, TS_MAXY, W, 0);
   
   const int MX = W;
   const int MY = H;
   for(int x =0;x<MX;x++)
   {
	   for(int y =0;y<MY;y++)
	   {
	       Render(x,y,1);
	   }
   }
   
   //tft.drawPixel(x,y,BLACK); 
  }
}

float Magnitude(float x,float y,float z)
{
	return sqrt(x*x+y*y+z*z);
}

float floatMod(float a, float b)
{
    return (a - b * floor(a / b));
}
/** ################################### Distance Estimator #############################
                                     Coloque sua formula aqui
*/
float DE(float x,float y,float z)
{
//return x*x+y*y+z*z + 10.0f;
x = floatMod(x,1.0f)-0.5f;
y = floatMod(y,1.0f)-0.5f;
z = floatMod(z,1.0f)-0.5f;
//esfera
float squared = x*x+y*y+z*z;
if(squared>0.1f)
return sqrt(squared-0.1f);
else
return 0.0f;



//return abs(z)+abs(x)+abs(y)-0.5f;
/*
float b = 0.2f;

x = abs(x);
y = abs(y);
z = abs(z);

x = max(x-b,0.0);
y = max(y-b,0.0);
z = max(z-b,0.0);

return sqrt(x*x+y*y+z*z);
*/
}
Vector3 from;
Vector3 dir;
float Near = 0.0f;
float Far  = 2.0f;
float Dist = (Far-Near);
int Total = 40;
float totalDistance;
float MinimumDistance;
float MaxDistance = 10.0f;
void Render(int Pixelx,int Pixely,int MAG) {
	from.set((((float)Pixelx)/WIDTHf) - 0.5f,(((float)Pixely)/WIDTHf) - 0.37f,-2.0f);

	dir.set(from.x - cam.x,from.y - cam.y,from.z - cam.z);
	dir.normalize();
	
	p.set(&from);
	
	
	totalDistance = 0.0f;
    MinimumDistance = 0.01f*MAG;
	int hited = trace();
	switch(hited)
	{
	case HIT_FARAWAY:
		cR = 255;
		cG = 0;
		cB = 0;
	break;
	case HIT_INSIDE:
		cR = 255;
		cG = 0;
		cB = 255;
	break;
	case HIT_SURFACE:
	{
		CalcNormal(MinimumDistance/2.0f); // 'small' é o tanto que vai ao redor para calcular o gradiente
		cR = normal.x;
		cG = normal.y;
		cB = normal.z;
	}
	break;
	case HIT_NOTHING:
		cR = 0;
		cG = 0;
		cB = 255;
	break;
	}
	
	if(MAG == 1)
	tft.drawPixel(Pixelx,Pixely,tft.Color565(cR,cG,cB));
	else
	tft.fillRect(Pixelx,Pixely,MAG,MAG,tft.Color565(cR,cG,cB));
}


int trace()
{
	int steps;
	for (steps=0;steps < MAXSTEPS; steps++) 
	{
		float distance = DE(p.x,p.y,p.z);
		if(distance < MinimumDistance)
		{
			if(steps==0)
			{
				// dentro do objeto 
				return HIT_INSIDE;
			}
			else
			{
				//superficie do objeto
				//cR = 0;
				//cG = (int)(255.0f * (1.0f - totalDistance/MaxDistance));
				//cB = 0;
				return HIT_SURFACE;
			}
			break;
		}
		totalDistance += distance;
		if(totalDistance+Near > MaxDistance)
		{
			return HIT_FARAWAY;
		}
		p.x += dir.x*distance;
		p.y += dir.y*distance;
		p.z += dir.z*distance;  
	}
	// foi ate o máximo de iterações e não chegou ao MaxDistance
	return HIT_NOTHING;
}

const float[] dir00 PROGMEM = {-1.0f,-1.0f,-1.0f};
const float[] dir01 PROGMEM = {+1.0f,-1.0f,-1.0f};
const float[] dir02 PROGMEM = {-1.0f,+1.0f,-1.0f};
const float[] dir03 PROGMEM = {+1.0f,+1.0f,-1.0f};
const float[] dir04 PROGMEM = {-1.0f,-1.0f,+1.0f};
const float[] dir05 PROGMEM = {+1.0f,-1.0f,+1.0f};
const float[] dir06 PROGMEM = {-1.0f,+1.0f,+1.0f};
const float[] dir07 PROGMEM = {+1.0f,+1.0f,+1.0f};		
void CalcNormal(float small)
{
	
	//calcular 8 subcalculos ao redor
	//para encontrar o "gradiente"
	//a direção para fora?
			
	float resultado00 = DE(p.x+dir00[0]*small,p.y+dir00[1]*small,p.z+dir00[2]*small);
	float resultado01 = DE(p.x+dir01[0]*small,p.y+dir01[1]*small,p.z+dir01[2]*small);
	float resultado02 = DE(p.x+dir02[0]*small,p.y+dir02[1]*small,p.z+dir02[2]*small);
	float resultado03 = DE(p.x+dir03[0]*small,p.y+dir03[1]*small,p.z+dir03[2]*small);
	float resultado04 = DE(p.x+dir04[0]*small,p.y+dir04[1]*small,p.z+dir04[2]*small);
	float resultado05 = DE(p.x+dir05[0]*small,p.y+dir05[1]*small,p.z+dir05[2]*small);
	float resultado06 = DE(p.x+dir06[0]*small,p.y+dir06[1]*small,p.z+dir06[2]*small);
	float resultado07 = DE(p.x+dir07[0]*small,p.y+dir07[1]*small,p.z+dir07[2]*small);
	
	normal.set( 
	// X
	 dir00[0]*resultado00
	+dir01[0]*resultado01
	+dir02[0]*resultado02
	+dir03[0]*resultado03
	+dir04[0]*resultado04
	+dir05[0]*resultado05
	+dir06[0]*resultado06
	+dir07[0]*resultado07;
	,
	// Y
	 dir00[1]*resultado00
	+dir01[1]*resultado01
	+dir02[1]*resultado02
	+dir03[1]*resultado03
	+dir04[1]*resultado04
	+dir05[1]*resultado05
	+dir06[1]*resultado06
	+dir07[1]*resultado07;
	,
	// Z
	 dir00[2]*resultado00
	+dir01[2]*resultado01
	+dir02[2]*resultado02
	+dir03[2]*resultado03
	+dir04[2]*resultado04
	+dir05[2]*resultado05
	+dir06[2]*resultado06
	+dir07[2]*resultado07);
	
	normal.normalize(); // faz o valor ficar com tamanho 1.0
	normal.scale(-1.0f); // a normal é o contrário do gradiente
}




