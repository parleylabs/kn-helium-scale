#include "LoRaWan_APP.h"
#include "Arduino.h"

#include <Wire.h>
#include <EEPROM.h> //Needed to record user settings
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h" 

/* OTAA para*/
uint8_t devEui[] = { 0x60, 0x81, 0xF9, 0x93, 0x54, 0x85, 0xFA, 0xEA };
uint8_t appEui[] = { 0x60, 0x81, 0xF9, 0x6C, 0xEB, 0xB5, 0x39, 0xA9 };
uint8_t appKey[] = { 0x4B, 0xED, 0x63, 0xA1, 0xF0, 0xD1, 0x5D, 0x2B, 0xE7, 0xFD, 0xF1, 0xF5, 0x91, 0xE6, 0xD8, 0x8A };
/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask, default channels 0-7*/ 
uint16_t userChannelsMask[6]={ 0xFF00,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 300000; //set to every 5 min


/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;


NAU7802 myScale; //Create instance of the NAU7802 class


/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port )
{
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
  *appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
  *if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
  *if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
  *for example, if use REGION_CN470, 
  *the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
  */

    union dataUnion {
      float dataFloat;
      unsigned char weightBytes[4];
    };

    dataUnion weight; //declaring union
    weight.dataFloat = myScale.getWeight();

    appDataSize = 4; //this is for floats
    appData[0] = weight.weightBytes[0];
    appData[1] = weight.weightBytes[1];
    appData[2] = weight.weightBytes[2];
    appData[3] = weight.weightBytes[3];

}

//Create an array to take average of weights. This helps smooth out jitter.
#define AVG_SIZE 4
float avgWeights[AVG_SIZE];
byte avgWeightSpot = 0;

void setup() {
 boardInitMcu();
 Serial.begin(115200); 
#if(AT_SUPPORT)
  enableAt();
#endif
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();

  //scale code

  Wire.begin();
  Wire.setClock(400000); 

  if (myScale.begin() == false)
  {
    Serial.println("Scale not detected. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Scale detected!");

  myScale.setCalibrationFactor(5367.73); // hard coding the calibration factor to avoid calibration
  myScale.setZeroOffset(-82703); // hard coding zero offset
  
  Serial.print("Zero offset: ");
  Serial.println(myScale.getZeroOffset());
  Serial.print("Calibration factor: ");
  Serial.println(myScale.getCalibrationFactor());
}

void loop() {
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
#if(AT_SUPPORT)
      getDevParam();
#endif
      printDevParam();
      LoRaWAN.init(loraWanClass,loraWanRegion);
      deviceState = DEVICE_STATE_JOIN;
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      prepareTxFrame( appPort );
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;
      break;
    }
    case DEVICE_STATE_CYCLE:
    {
      // Schedule next packet transmission
      txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      LoRaWAN.sleep();
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
   if (myScale.available() == true)
  {
    //this code organizes the data into columns in serial monitor for easier
    //readability and checking of values
    long currentReading = myScale.getReading();
    float currentWeight = myScale.getWeight();

    Serial.print("Reading: ");
    Serial.print(currentReading);
    Serial.print("\tWeight: ");
    Serial.print(currentWeight, 2); //Print 2 decimal places

    avgWeights[avgWeightSpot++] = currentWeight;
    if(avgWeightSpot == AVG_SIZE) avgWeightSpot = 0;

    float avgWeight = 0;
    for (int x = 0 ; x < AVG_SIZE ; x++)
      avgWeight += avgWeights[x];
    avgWeight /= AVG_SIZE;

    Serial.print("\tAvgWeight: ");
    Serial.print(avgWeight, 2); //Print 2 decimal places
    
    Serial.println();
  }
}



  
