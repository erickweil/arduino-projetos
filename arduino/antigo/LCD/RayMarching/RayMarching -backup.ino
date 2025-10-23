//#include <MemoryFree.h>
//#include <avr/pgmspace.h>
#include <TFTLCD7781.h>
#include <TouchScreen.h>
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

float normalx;
float normaly;
float normalz;

float px;
float py;
float pz;

float camx = 0.0f;
float camy = 0.0f;
float camz = -3.0f;
	
	
void setup(void) 
{
  tft.reset();
  tft.initDisplay(); 
  tft.setRotation(1); 
  pinMode(A0,INPUT);
  tft.fillScreen(WHITE);
  
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

void Render(int Pixelx,int Pixely,int MAG) {
	float fromx = (((float)Pixelx)/WIDTHf) - 0.5f;
	float fromy = (((float)Pixely)/WIDTHf) - 0.37f;
	float fromz = -2.0f;

	float dirx = (fromx - camx);
	float diry = (fromy - camy);
	float dirz = (fromz - camz);

//	float dirx = 0.0f;
//	float diry = 0.0f;
//	float dirz = 1.0f;

	// normalizing dir
	float mag = sqrt(dirx*dirx + diry*diry + dirz*dirz);
	dirx = dirx / mag;
	diry = diry / mag;
	dirz = dirz / mag;
        //*/
	
	float Near = 0.0f;
	float Far  = 2.0f;
	float Dist = (Far-Near);
	int Total = 40;

	float totalDistance = 0.0f;
	//float MinimumDistance = (Dist/MAXSTEPS)/2.0f;
        float MinimumDistance = 0.01f*MAG;
	float MaxDistance = 10.0f;
	int steps;
	
	px = fromx;
	py = fromy;
	pz = fromz;

	// cor azul
	cR = 0;
	cG = 0;
	cB = 255;
	for (int steps=0;steps < MAXSTEPS; steps++) 
	{
		float distance = DE(px,py,pz);
		if(distance < MinimumDistance)
		{
                        if(steps==0)
                        {
                        // dentro do objeto 
                        cR = 255;
			cG = 0;
			cB = 255;
                        }
                        else
                        {
			//superficie do objeto
			cR = 0;
			cG = (int)(255.0f * (1.0f - totalDistance/MaxDistance));
			cB = 0;
                        }
			break;
		}
                totalDistance += distance;
		if(totalDistance+Near > MaxDistance)
		{
			// caiu fora do campo de visão
			cR = 255;
			cG = 0;
			cB = 0;
			break;
		}
                px += dirx*distance;
		py += diry*distance;
		pz += dirz*distance;  
	}
        if(MAG == 1)
        tft.drawPixel(Pixelx,Pixely,tft.Color565(cR,cG,cB));
        else
        tft.fillRect(Pixelx,Pixely,MAG,MAG,tft.Color565(cR,cG,cB));
}


const float[] dir00 PROGMEM = {-1.0f,-1.0f,-1.0f};
const float[] dir01 PROGMEM = {+1.0f,-1.0f,-1.0f};
const float[] dir02 PROGMEM = {-1.0f,+1.0f,-1.0f};
const float[] dir03 PROGMEM = {+1.0f,+1.0f,-1.0f};
const float[] dir04 PROGMEM = {-1.0f,-1.0f,+1.0f};
const float[] dir05 PROGMEM = {+1.0f,-1.0f,+1.0f};
const float[] dir06 PROGMEM = {-1.0f,+1.0f,+1.0f};
const float[] dir07 PROGMEM = {+1.0f,+1.0f,+1.0f};		
void CalcNormal(float small,float px,float py,float pz)
{
	/*
	calcular 8 subcalculos ao redor
	para encontrar o "gradiente"
	quer dizer normal?
	*/		
	float resultado00 = DE(px+dir00[0]*small,py+dir00[1]*small,pz+dir00[2]*small);
	float resultado01 = DE(px+dir01[0]*small,py+dir01[1]*small,pz+dir01[2]*small);
	float resultado02 = DE(px+dir02[0]*small,py+dir02[1]*small,pz+dir02[2]*small);
	float resultado03 = DE(px+dir03[0]*small,py+dir03[1]*small,pz+dir03[2]*small);
	float resultado04 = DE(px+dir04[0]*small,py+dir04[1]*small,pz+dir04[2]*small);
	float resultado05 = DE(px+dir05[0]*small,py+dir05[1]*small,pz+dir05[2]*small);
	float resultado06 = DE(px+dir06[0]*small,py+dir06[1]*small,pz+dir06[2]*small);
	float resultado07 = DE(px+dir07[0]*small,py+dir07[1]*small,pz+dir07[2]*small);
	
	normalx = 
	 dir00[0]*resultado00
	+dir01[0]*resultado01
	+dir02[0]*resultado02
	+dir03[0]*resultado03
	+dir04[0]*resultado04
	+dir05[0]*resultado05
	+dir06[0]*resultado06
	+dir07[0]*resultado07;
	normaly = 
	 dir00[1]*resultado00
	+dir01[1]*resultado01
	+dir02[1]*resultado02
	+dir03[1]*resultado03
	+dir04[1]*resultado04
	+dir05[1]*resultado05
	+dir06[1]*resultado06
	+dir07[1]*resultado07;
	normalz = 
	 dir00[2]*resultado00
	+dir01[2]*resultado01
	+dir02[2]*resultado02
	+dir03[2]*resultado03
	+dir04[2]*resultado04
	+dir05[2]*resultado05
	+dir06[2]*resultado06
	+dir07[2]*resultado07;
	
	float normalMAG = Magnitude(normalx,normaly,normalz);
    normalx /= normalMAG;
	normaly /= normalMAG;
	normalz /= normalMAG;
}




