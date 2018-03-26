

#include <Arduino.h>
#include "MegaIrrigation.h"


void init_irrigation(uint8_t num_zones, int eeprom_start){
  EEPROM.setMemPool(eeprom_start, EEPROMSizeMega);
  _num_zones = num_zones;
  _eeprom_start_addr = eeprom_start;
}

void run_all_zones(void){

}

void run_one_zone(uint8_t _zone_){

}

void stop(void){

}

uint8_t irReadConfg(int pos){

}

uint8_t irWriteConfg(int pos){

}


void set_zone_eeprom_offset(){
  _zone_eeprom_offset = (int)(_eeprom_start_addr + (((int)_zone_number - 1) * ZONE_EEPROM_BYTES));
}

void loadZoneConfig(uint8_t num){
  _zone_number = num;
  set_zone_eeprom_offset();
  _zone_storebits = EEPROM.readByte(ZONE_STORE_BITS + _zone_eeprom_offset);
  _zone_pin = EEPROM.readByte(ZONE_PIN + _zone_eeprom_offset);
  EEPROM.readBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
  _zone_run_time = EEPROM.readByte(ZONE_RUNTIME + _zone_eeprom_offset);
  _zone_blowout_time = EEPROM.readInt(ZONE_BLOWOUT_TIME + _zone_eeprom_offset);
  _zone_blow_cycles = EEPROM.readByte(ZONE_BLOWOUT_CYCLES + _zone_eeprom_offset);
  _zone_is_dry_value = EEPROM.readInt(ZONE_IS_DRY_VALUE + _zone_eeprom_offset);
  _zone_moisture_id = EEPROM.readInt(ZONE_MOISTURE_ID + _zone_eeprom_offset);
}


void updateZoneConfig(){
  set_zone_eeprom_offset();
  EEPROM.updateByte(ZONE_NUM + _zone_eeprom_offset,(uint8_t)_zone_number);
  EEPROM.updateByte(ZONE_STORE_BITS + _zone_eeprom_offset,(uint8_t)_zone_storebits);
  EEPROM.updateByte(ZONE_PIN + _zone_eeprom_offset,(uint8_t)_zone_pin);
  EEPROM.updateBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
  EEPROM.updateByte(ZONE_RUNTIME + _zone_eeprom_offset,(uint8_t)_zone_run_time);
  EEPROM.updateInt(ZONE_BLOWOUT_TIME + _zone_eeprom_offset,(uint8_t)_zone_blowout_time);
  EEPROM.updateByte(ZONE_BLOWOUT_CYCLES + _zone_eeprom_offset,(uint8_t)_zone_blow_cycles);
  EEPROM.updateInt(ZONE_IS_DRY_VALUE + _zone_eeprom_offset,(uint8_t)_zone_is_dry_value);
  EEPROM.updateInt(ZONE_MOISTURE_ID + _zone_eeprom_offset,(uint8_t)_zone_moisture_id);
}

bool zone_normally_open(){
  return bitRead(_storebits,SBI_ZONE_NORMALLY_OPEN);
}

void water_on(){
  bitSet(_zone_rambits,RBZ_IS_ON);
  if(_zone_pin >= 1){
    if (zone_normally_open()){
      digitalWrite(_zone_pin, HIGH);
    } else {
      digitalWrite(_zone_pin, LOW);
    }
  }
}

void water_off(){
  bitClear(_zone_rambits,RBZ_IS_ON);
  if(_zone_pin >= 1){
    if (zone_normally_open()){
      digitalWrite(_zone_pin, LOW);
    } else {
      digitalWrite(_zone_pin, HIGH);
    }
  }
}

void wind_sensor_on(){
  bitSet(_zone_storebits,SBZ_USE_WIND);
}

void wind_sensor_off(void){
  bitClear(_zone_storebits,SBZ_USE_WIND);
}

void set_zone_name(char new_name[ZONE_NAME_SIZE]){
  strcpy(_zone_name,new_name);
  EEPROM.updateBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
}


bool zone_is_on() {
  return bitRead(_zone_rambits,RBZ_IS_ON);
}

bool zone_use_moisture(){
  return bitRead(_zone_storebits,SBZ_USE_MOISTURE);
}

bool zone_use_weather(){
  return bitRead(_zone_storebits,SBZ_USE_WEATHER);
}

bool master_valve_normally_open(){
  return bitRead(_storebits,SBI_MASTER_VALVE_NORMALLY_OPEN);
}


