

#include <Arduino.h>
#include "MegaIrrigation.h"
//#include "../libraries/MySensors/core/MyEepromAddresses.h"
//#include <avr/eeprom.h>
//#include <avr/pgmspace.h>

// ==== Irrigation Class ====
Irrigation::Irrigation(uint8_t num_zones, uint16_t eeprom_st){
  EEPROM.setMemPool(eeprom_st, EEPROMSizeMega);
  eeprom_start = eeprom_st;
  Zone zone[num_zones - 1];
  for (uint8_t i; i<num_zones; i++){
    zone[i].readConfig(i+1);
  }

}

void Irrigation::run_all_zones(void){

}

void Irrigation::run_one_zone(uint8_t zn){

}

void Irrigation::stop(void){

}

uint8_t irReadConfg(int pos){

}

uint8_t irWriteConfg(int pos){

}




// ==== Zone Class ==== 
Zone::Zone(){
  number = 0;
  storebits = 0;
  pin = 0;
  strcpy (name, "             ");
  run_time = 0;
  is_dry_value = 0;
  moisture_id = 0;
  previous_moisture = 0;
  current_moisture = 0;
  rambits = 0;
}

void Zone::readConfig(uint8_t num){
  number = num;
  readConfig();
}

void Zone::readConfig(){
  uint16_t offset = eepromOffset();
  storebits = EEPROM.readByte(ZONE_STORE_BITS + offset);
  pin = EEPROM.readByte(ZONE_PIN + offset);
  EEPROM.readBlock<char>(ZONE_NAME + offset, name, sizeof(name));
  run_time = EEPROM.readByte(ZONE_RUNTIME + offset);
  blowout_time = EEPROM.readInt(ZONE_BLOWOUT_TIME + offset);
  blow_cycles = EEPROM.readByte(ZONE_BLOWOUT_CYCLES + offset);
  is_dry_value = EEPROM.readInt(ZONE_IS_DRY_VALUE + offset);
  moisture_id = EEPROM.readInt(ZONE_MOISTURE_ID + offset);
}

void Zone::updateConfig(){
  uint16_t eeprom_offset = eepromOffset();
  EEPROM.updateByte(ZONE_NUM + eeprom_offset,(uint8_t)number);
  EEPROM.updateByte(ZONE_STORE_BITS + eeprom_offset,(uint8_t)storebits);
  EEPROM.updateByte(ZONE_PIN + eeprom_offset,(uint8_t)pin);
  EEPROM.updateBlock<char>(ZONE_NAME + eeprom_offset, name, sizeof(name));
  EEPROM.updateByte(ZONE_RUNTIME + eeprom_offset,(uint8_t)run_time);
  EEPROM.updateInt(ZONE_BLOWOUT_TIME + eeprom_offset,(uint8_t)blowout_time);
  EEPROM.updateByte(ZONE_BLOWOUT_CYCLES + eeprom_offset,(uint8_t)blow_cycles);
  EEPROM.updateInt(ZONE_IS_DRY_VALUE + eeprom_offset,(uint8_t)is_dry_value);
  EEPROM.updateInt(ZONE_MOISTURE_ID + eeprom_offset,(uint8_t)moisture_id);
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

void Zone::set_name(char new_name[ZONE_NAME_SIZE]){
  uint16_t eeprom_offset = eepromOffset();
  strcpy(name,new_name);
  EEPROM.updateBlock<char>(ZONE_NAME + eeprom_offset, name, sizeof(name));
}

uint16_t Zone::eepromOffset(){
  return (uint16_t)(eeprom_start + ((number - 1) * ZONE_EEPROM_BYTES));
}

