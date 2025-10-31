#ifndef LoRaWan_APP_H
#define LoRaWan_APP_H

#include "UnitTest.h"

#define LORAWAN_APP_DATA_MAX_SIZE 255

/*!
 * LoRaWAN devices classes definition
 *
 * LoRaWAN Specification V1.0.2, chapter 2.1
 */
typedef enum eDeviceClass {
    /*!
     * LoRaWAN device class A
     *
     * LoRaWAN Specification V1.0.2, chapter 3
     */
    CLASS_A,
    /*!
     * LoRaWAN device class B
     *
     * LoRaWAN Specification V1.0.2, chapter 8
     */
    CLASS_B,
    /*!
     * LoRaWAN device class C
     *
     * LoRaWAN Specification V1.0.2, chapter 17
     */
    CLASS_C,
} DeviceClass_t;


/*!
 * LoRaMAC region enumeration
 */
typedef enum eLoRaMacRegion_t {
    /*!
     * AS band on 923MHz
     */
    LORAMAC_REGION_AS923,
    /*!
     * Australian band on 915MHz
     */
    LORAMAC_REGION_AU915,
    /*!
     * Chinese band on 470MHz
     */
    LORAMAC_REGION_CN470,
    /*!
     * Chinese band on 779MHz
     */
    LORAMAC_REGION_CN779,
    /*!
     * European band on 433MHz
     */
    LORAMAC_REGION_EU433,
    /*!
     * European band on 868MHz
     */
    LORAMAC_REGION_EU868,
    /*!
     * South korean band on 920MHz
     */
    LORAMAC_REGION_KR920,
    /*!
     * India band on 865MHz
     */
    LORAMAC_REGION_IN865,
    /*!
     * North american band on 915MHz
     */
    LORAMAC_REGION_US915,
    /*!
     * North american band on 915MHz with a maximum of 16 channels
     */
    LORAMAC_REGION_US915_HYBRID,
    /*!
     * Australian band on 915MHz Subband 2
     */
    LORAMAC_REGION_AU915_SB2,
    /*!
     * AS band on 922.0-923.4MHz
     */
    LORAMAC_REGION_AS923_AS1,
    /*!
     * AS band on 923.2-924.6MHz
     */
    LORAMAC_REGION_AS923_AS2,
    /*!
     * Russia band on 864MHz
     */
    LORAMAC_REGION_RU864,
} LoRaMacRegion_t;

enum eDeviceState_LoraWan
{
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_SEND,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
};

enum eDeviceState_Lora
{
    LORA_INIT,
    LORA_SEND,
    LORA_RECEIVE,
    LORA_CAD,
    MCU_SLEEP,
};

extern uint8_t devEui[];
extern uint8_t appEui[];
extern uint8_t appKey[];
extern uint8_t nwkSKey[];
extern uint8_t appSKey[];
extern uint32_t devAddr;
extern uint8_t appData[LORAWAN_APP_DATA_MAX_SIZE];
extern uint8_t appDataSize;
extern uint8_t appPort;
extern uint32_t txDutyCycleTime;
extern bool overTheAirActivation;
extern LoRaMacRegion_t loraWanRegion;
extern bool loraWanAdr;
extern bool isTxConfirmed;
extern uint32_t appTxDutyCycle;
extern DeviceClass_t loraWanClass;
extern bool passthroughMode;
extern uint8_t confirmedNbTrials;
extern bool modeLoraWan;
extern bool keepNet;
extern uint16_t userChannelsMask[6];


/*!
 * Defines a random delay for application data transmission duty cycle. 1s,
 * value in [ms].
 */
#define APP_TX_DUTYCYCLE_RND                        1000

class LoRaWanClass{
public:
  void init(DeviceClass_t lorawanClass,LoRaMacRegion_t region);
  void join();
  void send();
  void cycle(uint32_t dutyCycle);
  void sleep(DeviceClass_t classMode);
  void setDefaultDR(int8_t dataRate);
  void generateDeveuiByChipID();
};

extern enum eDeviceState_LoraWan deviceState;

extern LoRaWanClass LoRaWAN;


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

#endif