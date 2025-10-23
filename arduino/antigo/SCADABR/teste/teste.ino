#include <SimpleModbusSlave.h>
	
#define  LED 9

enum 
{     
  
  VALOR_POT,     
  VALOR_PWM,        
  HOLDING_REGS_SIZE 
};
 
unsigned int holdingRegs[HOLDING_REGS_SIZE]; 

void setup()
{
     modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 2, HOLDING_REGS_SIZE, holdingRegs);
 
     modbus_update_comms(9600, SERIAL_8N1, 1);
  
     pinMode(LED, OUTPUT);
}

 

void loop()
{
     modbus_update();
  
     holdingRegs[VALOR_POT] = analogRead(A0); 
     
     analogWrite(LED, holdingRegs[VALOR_PWM]);   

}

