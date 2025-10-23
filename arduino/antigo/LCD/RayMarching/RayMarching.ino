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

#define MAXSTEPS 64
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
Vector3 LightPos;
#define HIT_FARAWAY 0
#define HIT_INSIDE 1
#define HIT_SURFACE 2	
#define HIT_NOTHING 3
void setup(void) 
{
  Serial.begin(9600);
  
  tft.reset();
  tft.initDisplay(); 
  //********************************************** consertando problema onde o primeiro pixel é desenhado com a cor anterior ***********************************************************
  for(byte i=0;i<20;i++)
  {
  tft.drawPixel(10,12,GREY);
   tft.writeRegister(TFTLCD7781_GRAM_HOR_AD, 10); // GRAM Address Set (Horizontal Address) (R20h)
 tft.writeRegister(TFTLCD7781_GRAM_VER_AD, 12); // GRAM Address Set (Vertical Address) (R21h)
 tft.writeCommand(TFTLCD7781_RW_GRAM);  // Write Data to GRAM (R22h)
int data = tft.readData();

  tft.drawPixel(10,12,GREY);
 tft.writeRegister(TFTLCD7781_GRAM_HOR_AD, 10); // GRAM Address Set (Horizontal Address) (R20h)
 tft.writeRegister(TFTLCD7781_GRAM_VER_AD, 12); // GRAM Address Set (Vertical Address) (R21h)
 tft.writeCommand(TFTLCD7781_RW_GRAM);  // Write Data to GRAM (R22h)
 data = tft.readData();
 
 delay(20);
  Serial.print(data,HEX);
  Serial.print(F(" Tinha de ser "));
  Serial.println(0x52AA,HEX);  
  if(data==0x52AA)
  break;
  else
  tft.initDisplay(); 
  }
  
  
  
  
  
  
  tft.setRotation(1); 
  //pinMode(A0,INPUT);
  tft.fillScreen(BLACK);
  /*
  Serial.print("\nBRANCO ");Serial.println(WHITE,HEX);
  GetPixel(100,100);
  GetPixel(100,100);
  tft.fillScreen(BLACK);
  Serial.print("\nPRETO ");Serial.println(BLACK,HEX);
  GetPixel(100,100);
  GetPixel(100,100);
  tft.fillScreen(BLUE);
  Serial.print("\nAZUL ");Serial.println(BLUE,HEX);
  GetPixel(100,100);
  GetPixel(100,100);
  tft.fillScreen(RED);
  Serial.print("\nVERMELHO ");Serial.println(RED,HEX);
  GetPixel(100,100);
  GetPixel(100,100);
  */
  LightPos.set(1.0f,2.0f,2.0f);
  cam.set(0.0f,0.0f,-3.0f);
  //RenderAll(64,true,true);
  //RenderAll(16,true,false);
  //RenderAll(8,true,true);
  RenderAll(1,false,false);
}

void RenderAll(int MAG,boolean Pre,boolean first)
{
   int MX = W/MAG;
   int MY = H/MAG;
   for(int x =0;x<MX;x++)
   {
	   for(int y =0;y<MY;y++)
	   {
		   if(Pre)
	           PreRender(x*MAG,y*MAG,MAG,first);
		   else
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
Vector3 pos;
float DE(float x,float y,float z)
{


//x = floatMod(x,1.0f)-0.5f;
//y = floatMod(y,1.0f)-0.5f;
//z = floatMod(z,1.0f)-0.5f;

//esfera
pos.set(x-0.5,y,z);
float esfera = pos.length()-0.36f;

//cubo
pos.set(x+0.5f,y,z);

pos.absolute();
pos.x -= 0.36f;
pos.y -= 0.36f;
pos.z -= 0.36f;
//vec3 d = abs(p) - b;

float dx = pos.x;
float dy = pos.y;
float dz = pos.z;

pos.x = pos.x > 0.0f? pos.x : 0.0f;
pos.y = pos.y > 0.0f? pos.y : 0.0f;
pos.z = pos.z > 0.0f? pos.z : 0.0f;

float cubo = min(max(dx,max(dy,dz)),0.0f) + pos.length(); 
//return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));

float chao = 0.3f-y;

return min(chao,min(esfera,cubo));
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
float distance;
void PreRender(int Pixelx,int Pixely,int MAG,boolean first) {
	unsigned long prev_pixel;
	float prev_Dist;
	if(!first)
	{
		prev_pixel = GetPixel(Pixelx,Pixely);
		prev_Dist = prev_pixel/100.0f;
	}
	//Serial.println(prev_Dist);
	from.set((((float)Pixelx)/WIDTHf) - 0.5f,(((float)Pixely)/WIDTHf) - 0.37f,-2.0f);
	dir.set(from.x - cam.x,from.y - cam.y,from.z - cam.z);
	dir.normalize();
	p.set(&from);
	if(!first)
	{
		p.x += dir.x*prev_Dist;
		p.y += dir.y*prev_Dist;
		p.z += dir.z*prev_Dist; 
	}

	if(first)
	totalDistance = 0.0f;
	else
	totalDistance = prev_Dist;
        MinimumDistance = 0.01f*MAG*2.0f;
	
	int hited = trace();
        if(hited == HIT_INSIDE) totalDistance = 0.0f;
        if(hited == HIT_SURFACE) totalDistance -= distance;
	prev_pixel = (unsigned long)(totalDistance*100.0f);
	if(MAG == 1)
	tft.drawPixel(Pixelx,Pixely,prev_pixel);
	else
	tft.fillRect(Pixelx,Pixely,MAG,MAG,prev_pixel);
}

void Render(int Pixelx,int Pixely,int MAG) {
  
        //unsigned long prev_pixel = GetPixel(Pixelx,Pixely);
	//float prev_Dist = prev_pixel/100.0f;
        
	from.set((((float)Pixelx)/WIDTHf) - 0.5f,(((float)Pixely)/WIDTHf) - 0.37f,-2.0f);

	dir.set(from.x - cam.x,from.y - cam.y,from.z - cam.z);
	dir.normalize();
	
	p.set(&from);
	//p.x += dir.x*prev_Dist;
	//p.y += dir.y*prev_Dist;
	//p.z += dir.z*prev_Dist; 
	
	//totalDistance = prev_Dist;
        totalDistance=0.0f;
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
		CalcNormal(MinimumDistance); // 'small' é o tanto que vai ao redor para calcular o gradiente
		CalcCor();
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
		distance = DE(p.x,p.y,p.z);
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

const float dir00[] = {-1.0f,-1.0f,-1.0f};
const float dir01[] = {+1.0f,-1.0f,-1.0f};
const float dir02[] = {-1.0f,+1.0f,-1.0f};
const float dir03[] = {+1.0f,+1.0f,-1.0f};
const float dir04[] = {-1.0f,-1.0f,+1.0f};
const float dir05[] = {+1.0f,-1.0f,+1.0f};
const float dir06[] = {-1.0f,+1.0f,+1.0f};
const float dir07[] = {+1.0f,+1.0f,+1.0f};		
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
	+dir07[0]*resultado07
	,
	// Y
	 dir00[1]*resultado00
	+dir01[1]*resultado01
	+dir02[1]*resultado02
	+dir03[1]*resultado03
	+dir04[1]*resultado04
	+dir05[1]*resultado05
	+dir06[1]*resultado06
	+dir07[1]*resultado07
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

float diffuseR=1.0f;
float diffuseG=1.0f;
float diffuseB=1.0f;

float specularR=1.0f;
float specularG=1.0f;
float specularB=1.0f;
const float shininess = 65.0f;

Vector3 EyeSpacePos;
Vector3 LightDir;
Vector3 spec_L;
Vector3 spec_E;
Vector3 spec_R;
void CalcCor()
{
		
		//vec3 LightDir = normalize(v_position-u_CamPos);
		LightDir.set(&LightPos);
		LightDir.subtract(&p);
		LightDir.normalize();
		float dotnormal = normal.dot(&LightDir);
		if(dotnormal>0.0)
		{
			float IdiffR = diffuseR *dotnormal;  
			float IdiffG = diffuseG *dotnormal;  
			float IdiffB = diffuseB *dotnormal; 
				
			// calculate Specular Term:	
			EyeSpacePos.set(&p);
			EyeSpacePos.subtract(&cam); // posicao do pixel em eye-space
			
			spec_L.set(&EyeSpacePos);
			spec_L.subtract(&LightDir);	
			spec_L.normalize();
			
			spec_E.set(&EyeSpacePos);
			spec_E.normalize();
			
			spec_R.set(&spec_L);
			spec_R.reflect(&normal);
			spec_R.scale(-1.0f);
			spec_R.normalize();
			 
			float dotspec = spec_R.dot(&spec_E);
			float IspecR = 0;  
			float IspecG = 0;  
			float IspecB = 0; 
			if(dotspec>0.0f)
			{
				float powshininess = pow(dotspec,0.3f*shininess);
				
				if(powshininess>0.0f)
				{
					IspecR = specularR * powshininess;
					IspecG = specularG * powshininess;
					IspecB = specularB * powshininess;
				}
			}
			
			cR = (int)((IdiffR + IspecR) * 255.0f);  
			cG = (int)((IdiffG + IspecG) * 255.0f);    
			cB = (int)((IdiffB + IspecB) * 255.0f);  
		}
		else
		{
			cR = 0;  
			cG = 0;  
			cB = 0; 
		}
		
		if(cR > 255) cR = 255;
		if(cG > 255) cG = 255;
		if(cB > 255) cB = 255;
}

//________________________________________________________AQUI TEMOS ACESSOS A MEMORIA FLASH                                _______________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________________________________________

byte Get(const byte* ref,int k) {
 return pgm_read_byte(&ref[k]);
}
int Get(const int* ref,int k) {
 return pgm_read_word(&ref[k]);
}
int Get(int* ref,int k) {
 return ref[k];
}
float Get(const float* ref,int k) {
 return pgm_read_dword(&ref[k]);
}
long Get(const long* ref,int k) {
 return pgm_read_dword(&ref[k]);
}

// acessos ao valor dos pixel
#define swap(a, b) { int16_t t = a; a = b; b = t; }
const uint16_t TFTWIDTH = 240;
const uint16_t TFTHEIGHT = 320;
unsigned long GetPixel(int x,int y)
{
	// check rotation, move pixel around if necessary
	int rotation = tft.getRotation();
	switch (rotation) {
	case 1:
	swap(x, y);
	x = TFTWIDTH - x - 1;
	break;
	case 2:
	x = TFTWIDTH - x - 1;
	y = TFTHEIGHT - y - 1;
	break;
	case 3:
	swap(x, y);
	y = TFTHEIGHT - y - 1;
	break;
	}

	if ((x >= TFTWIDTH) || (y >= TFTHEIGHT)) return 0;

	
	tft.goTo(x,y);

        // existe um bug onde deve se ler duas vezes para resgatar o valor correto
	//tft.readRegister(TFTLCD7781_RW_GRAM);
	unsigned long data = tft.readRegister(TFTLCD7781_RW_GRAM);
	
	//Serial.println(data,HEX);
    return data;
}
