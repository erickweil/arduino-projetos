#include "LoRaWan_APP.h"
#include <ArduinoFake.h>

RTC_DATA_ATTR int8_t defaultDrForNoAdr = 5;
/*AT mode, auto into low power mode*/
RTC_DATA_ATTR bool autoLPM = true;

/*loraWan current Dr when adr disabled*/
RTC_DATA_ATTR int8_t currentDrForNoAdr;

/*!
 * User application data size
 */
RTC_DATA_ATTR uint8_t appDataSize = 0;

/*!
 * User application data
 */
RTC_DATA_ATTR uint8_t appData[LORAWAN_APP_DATA_MAX_SIZE];


/*!
 * Defines the application data transmission duty cycle
 */
RTC_DATA_ATTR uint32_t txDutyCycleTime ;

/*!
 * PassthroughMode mode enable/disable. don't modify it here. 
 * when use PassthroughMode, set it true in app.ino , Reference the example PassthroughMode.ino 
 */
bool passthroughMode = false;

/*!
 * when use PassthroughMode, Mode_LoraWan to set use lora or lorawan mode . don't modify it here. 
 * it is used to set mode lora/lorawan in PassthroughMode.
 */
bool modeLoraWan = true;


RTC_DATA_ATTR enum eDeviceState_LoraWan deviceState=DEVICE_STATE_INIT;

/*void __attribute__((weak)) downLinkAckHandle()
{
	//printf("ack received\r\n");
}*/


void LoRaWanClass::generateDeveuiByChipID()
{
	uint32_t uniqueId[2] = {0xCAFE,0xBABE};
	for(int i=0;i<8;i++)
	{
		if(i<4)
			devEui[i] = (uniqueId[1]>>(8*(3-i)))&0xFF;
		else
			devEui[i] = (uniqueId[0]>>(8*(7-i)))&0xFF;
	}
}

void LoRaWanClass::init(DeviceClass_t lorawanClass,LoRaMacRegion_t region)
{
    deviceState = DEVICE_STATE_JOIN;
}

void LoRaWanClass::join()
{
    deviceState = DEVICE_STATE_CYCLE;
}

void LoRaWanClass::send()
{
    printf("[LoRaWAN] Sending bytes: ");
    for(int i=0;i<appDataSize;i++)
    {
        // Simula o envio dos dados
        printf("%02X ", appData[i]);
    }
    printf("\n");
}

void LoRaWanClass::cycle(uint32_t dutyCycle)
{

}

void LoRaWanClass::sleep(DeviceClass_t classMode)
{

}

void LoRaWanClass::setDefaultDR(int8_t dataRate)
{
	defaultDrForNoAdr = dataRate;
}

LoRaWanClass LoRaWAN;