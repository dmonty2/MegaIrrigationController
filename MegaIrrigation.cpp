

#include <Arduino.h>
#include "MegaIrrigation.h"

// constructor
Zone::Zone(void){
  number = 0;
  pin = 0;
  //name = "             ";
  run_time = 0;
  is_dry_value = 0;
  storebits = 0;
  moisture_id = 0;
  previous_moisture = 0;
  current_moisture = 0;
  rambits = 0;
}

void Zone::water_on(void){
  bitSet(rambits,IS_ON);
}

void Zone::loadConfig(){
  //EEPROM_LOCAL_CONFIG_ADDRESS
}

void Zone::water_off(void){
  bitClear(rambits,IS_ON);
}

void Zone::wind_sensor_on(void){
  bitSet(storebits,USE_WIND);
}

void Zone::wind_sensor_off(void){
  bitClear(storebits,USE_WIND);
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