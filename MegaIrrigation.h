  /*
   To calculate ETA for watering:
  // (on a fixed interval of time)
  if ( current_moisture <= is_dry_moisture){
     water();
  } else {
     periods_till_water =  (current_moisture - is_dry_value)/(previous_moisture - current_reading)
     if (periods_till_water <= 1 && periods_till_water >= 0){
        water();
     }
  }
  */


#ifndef MegaIrrigation_h
#define MegaIrrigation_h

#define IRRIGATION_VERSION 1

#include "Arduino.h"
#include <EEPROMex.h>

//#include <avr/eeprom.h>
//#include <avr/pgmspace.h>

#define ZONE_NAME_SIZE 15 // Zone name size 14+NUL

// Start of eeprom is stored in EEPROM_LOCAL_CONFIG_ADDRESS
//#include "../libraries/MySensors/core/MyEepromAddresses.h"
// eeprom arangement will be offset by above.
#define IRR_VERSION 0                          // uint8_t
#define IRR_STORE_BITS (IRR_VERSION + 1)       // uint8_t
#define IRR_NUM_ZONES (IRR_STORE_BITS + 1)     // uint8_t
#define IRR_MASTER_VALVE_PIN (IRR_NUM_ZONES + 1)   // uint8_t
#define IRR_RAIN_SESNOR_PIN (IRR_MASTER_VALVE_PIN + 1) // uint8_t
#define IRR_WIND_ID (IRR_RAIN_SESNOR_PIN + 1)   // uing16_t
// With spare (5 bytes).
#define IRR_EEPROM_BYTES  12 // EEPROM Bytes needed for IRR_*

#define ZONE_NUM 0                     // uint8_t
#define ZONE_STORE_BITS (ZONE_NUM + 1) // uint8_t
#define ZONE_PIN (ZONE_STORE_BITS + 1) // uint8_t
#define ZONE_NAME (ZONE_PIN + 1)       // char[15] ZONE_NAME_SIZE
#define ZONE_RUNTIME (ZONE_NAME + ZONE_NAME_SIZE)   // uint8_t (store in mintues)
#define ZONE_BLOWOUT_TIME (ZONE_RUNTIME + 2)        // uint16_t (store in seconds)
#define ZONE_BLOWOUT_CYCLES (ZONE_BLOWOUT_TIME + 2) // uint8_t
#define ZONE_IS_DRY_VALUE (ZONE_BLOWOUT_CYCLES + 1) // uint16_t
#define ZONE_MOISTURE_ID (ZONE_IS_DRY_VALUE + 2)    // uint16_t
// With spare (4 bytes)
#define ZONE_EEPROM_BYTES 30 // EEPROM Bytes needed for each zone;

//Store bits for Irrigation class
#define SBI_IN_EEPROM 0      // Set to 1 once saved.
#define SBI_ON_OFF 1         // Set to 1 when system is on.
#define SBI_MASTER_VALVE_NORMALLY_OPEN 2 // Set to 1 when normally open.
#define SBI_ZONE_NORMALLY_OPEN 3 // Set to 1 when zones are normally open.



// Store bits for Zone class
#define SBZ_IN_EEPROM 0      // Set to 1 once saved.
#define SBZ_USE_MOISTURE 1   // Use Soil Moisture MySensor
#define SBZ_USE_WEATHER 2    // Use Weather forecast.
#define SBZ_USE_WIND 3       // Use Wind sensor.

// Ram bits Zone
#define RBZ_IS_ON 0


uint8_t  _version = IRRIGATION_VERSION;
uint8_t  _storebits = 0;         // store bits
uint8_t  _is_enabled = 0;        // Master ON OFF/rain
uint8_t  _num_zones = 0;         // Number of zones.
uint8_t  _master_valve_pin = 0;  // Pin for master valve
uint8_t  _rain_sensor_pin = 0;   // Pin for rain sensor.
uint16_t _wind_id = 0;           // MySensor ID for wind.
int      _eeprom_start_addr = 0; // EEPROM start address.

uint8_t   _zone_number = 0;       //  1 Zone Number
uint8_t   _zone_storebits = 0;    //  1 used to store on/off bits
uint8_t   _zone_pin = 0;          //  1 Arduino Pin number
char      _zone_name[ZONE_NAME_SIZE];     // 15 Zone Name 14char
uint16_t  _zone_run_time = 0;     //  4 Zone Run Time (in minutes)
uint16_t  _zone_blowout_time = 0; //  4 Zone Run Time for compressed air blow out. (in seconds)
uint8_t   _zone_blow_cycles = 0;  //  4 Number of cycles for compressor air blow out. 
uint16_t  _zone_is_dry_value = 0; //  2 Value at which the zone is considered dry.
uint16_t  _zone_moisture_id = 0;  //  2 MySensors ID for moisture sensor.

// Store in RAM
uint16_t  _zone_previous_moisture = 0;  // Previous 
uint16_t  _zone_current_moisture = 0;   // Current Mosture sensor value
int       _zone_eeprom_offset = 0;      // EEPROM address offset for this zone.
uint8_t   _zone_rambits = 0;            // used store


#endif