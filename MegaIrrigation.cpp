

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
  uint16_t eeprom_offset = eeprom_start + ((number - 1) * ZONE_EEPROM_SPACE)
  storebits = hwReadConfig(ZONE_STORE_BITS + eeprom_offset);
  pin = hwReadConfig(ZONE_PIN + eeprom_offset);
  hwReadConfigBlock((void*)&name, (void*)(ZONE_NAME + eeprom_offset),
                          sizeof(name));
  run_time = hwReadConfig(ZONE_RUNTIME);
  hwReadConfigBlock((void*)&blowout_time, (void*)ZONE_BLOWOUT_TIME + eeprom_offset,
                          sizeof(blowout_time));
  blow_cycles = hwReadConfig(ZONE_BLOWOUT_CYCLES + eeprom_offset);
  hwReadConfigBlock((void*)&is_dry_value, (void*)ZONE_IS_DRY_VALUE + eeprom_offset,
                          sizeof(is_dry_value));
  hwReadConfigBlock((void*)&moisture_id, (void*)ZONE_MOISTURE_ID + eeprom_offset,
                          sizeof(moisture_id));
}

void Zone::updateConfig(){
  uint16_t eeprom_offset = eeprom_start + ((number - 1) * ZONE_EEPROM_SPACE)
  hwWriteConfig(ZONE_NUM + eeprom_offset,(uint8_t)number);
  hwWriteConfig(ZONE_STORE_BITS + eeprom_offset,(uint8_t)storebits);
  hwWriteConfig(ZONE_PIN + eeprom_offset,(uint8_t)pin);
  hwWriteConfigBlock((void*)name, (void*)ZONE_NAME + eeprom_offset,
                           sizeof(name));
  hwWriteConfigBlock((void*)run_time, (void*)ZONE_RUNTIME + eeprom_offset,
                           sizeof(run_time));
  hwWriteConfigBlock((void*)blowout_time, (void*)ZONE_BLOWOUT_TIME + eeprom_offset,
                           sizeof(blowout_time));
  hwWriteConfig(ZONE_BLOWOUT_CYCLES + eeprom_offset,(uint8_t)blow_cycles);
  hwWriteConfigBlock((void*)is_dry_value, (void*)ZONE_IS_DRY_VALUE + eeprom_offset,
                           sizeof(is_dry_value));
  hwWriteConfigBlock((void*)moisture_id, (void*)ZONE_MOISTURE_ID + eeprom_offset,
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


Irrigation::Irrigation(uint8_t num_zones, uint16_t eeprom_st){
  eeprom_start = eeprom_st;
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