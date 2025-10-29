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

#endif