

#include <Arduino.h>
#include "MegaIrrigation.h"
//#include "../libraries/MySensors/core/MyEepromAddresses.h"
//#include <avr/eeprom.h>
//#include <avr/pgmspace.h>

// constructor
Zone::Zone(){
  number = 0;
  storebits = 0;
  pin = 0;
  //name = "             ";
  run_time = 0;
  is_dry_value = 0;
  moisture_id = 0;
  previous_moisture = 0;
  current_moisture = 0;
  rambits = 0;
  for (uint8_t i; i<=14; i++){
    name[i] = ' ';
  }
}

void Zone::readConfig(){
  storebits = hwReadConfig(ZONE_STORE_BITS);
  pin = hwReadConfig(ZONE_PIN);
  hwReadConfigBlock((void*)&name, (void*)ZONE_NAME,
                          sizeof(name));
  run_time = hwReadConfig(ZONE_RUNTIME);
  hwReadConfigBlock((void*)&blowout_time, (void*)ZONE_BLOWOUT_TIME,
                          sizeof(blowout_time));
  blow_cycles = hwReadConfig(ZONE_BLOWOUT_CYCLES);
  hwReadConfigBlock((void*)&is_dry_value, (void*)ZONE_IS_DRY_VALUE,
                          sizeof(is_dry_value));
  hwReadConfigBlock((void*)&moisture_id, (void*)ZONE_MOISTURE_ID,
                          sizeof(moisture_id));
}

void Zone::updateConfig(){
  hwWriteConfig(ZONE_NUM,(uint8_t)number);
  hwWriteConfig(ZONE_STORE_BITS,(uint8_t)storebits);
  hwWriteConfig(ZONE_PIN,(uint8_t)pin);
  hwWriteConfigBlock((void*)name, (void*)ZONE_NAME,
                           sizeof(name));
  hwWriteConfigBlock((void*)run_time, (void*)ZONE_RUNTIME,
                           sizeof(run_time));
  hwWriteConfigBlock((void*)blowout_time, (void*)ZONE_BLOWOUT_TIME,
                           sizeof(blowout_time));
  hwWriteConfig(ZONE_BLOWOUT_CYCLES,(uint8_t)blow_cycles);
  hwWriteConfigBlock((void*)is_dry_value, (void*)ZONE_IS_DRY_VALUE,
                           sizeof(is_dry_value));
  hwWriteConfigBlock((void*)moisture_id, (void*)ZONE_MOISTURE_ID,
                           sizeof(moisture_id));
}

void Zone::water_on(){
  bitSet(rambits,RBZ_IS_ON);
}

void Zone::water_off(){
  bitClear(rambits,RBZ_IS_ON);
}

void Zone::wind_sensor_on(){
  bitSet(storebits,SBZ_USE_WIND);
}

void Zone::wind_sensor_off(void){
  bitClear(storebits,SBZ_USE_WIND);
}


Irrigation::Irrigation(uint8_t num_zones){
  Zone zone[num_zones - 1];
  for (uint8_t i; i<num_zones; i++){
    zone[i].number = i+1;
  }

}

void Irrigation::run_all_zones(void){

}

void Irrigation::run_one_zone(uint8_t zn){

}

void Irrigation::stop(void){

}