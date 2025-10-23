#include <SimpleModbusSlave.h>
	
enum 
{     
  
  VALOR_ALTURA_ATUAL, 
  VALOR_ALTURA_REQUERIDA,  
  HOLDING_REGS_SIZE 
};
 
unsigned int holdingRegs[HOLDING_REGS_SIZE]; 

int altura_atual;
int altura_requerida;
void setup()
{
     modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 2, HOLDING_REGS_SIZE, holdingRegs);
 
     modbus_update_comms(9600, SERIAL_8N1, 1);
     
}

 

void loop()
{
     modbus_update();
  
     holdingRegs[VALOR_ALTURA_ATUAL] = altura_atual; 
     altura_requerida = holdingRegs[VALOR_ALTURA_REQUERIDA];  
     
     
     if(altura_atual < altura_requerida)
     {
       altura_atual++;
       subir();
     } 
     if(altura_atual > altura_requerida)
     {
       altura_atual--;
       descer();
     }   
    
}


void subir()
{
  delay(50);
}

void descer()
{
  delay(50);
}
