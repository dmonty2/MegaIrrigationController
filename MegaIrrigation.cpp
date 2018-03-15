

#include <Arduino.h>
#include "MegaIrrigation.h"

// constructor
Zone::Zone(void){
  z_number = 0;
  z_pin = 0;
  z_name = "             ";
  z_run_time = 0;
  z_is_dry_value = 0;
  z_storebits = 0;
  z_moisture_id = 0;
  z_previous_moisture = 0;
  z_current_moisture = 0;
  z_rambits = 0;
}

void Zone::water_on(void){
  bitSet(z_rambits,IS_ON);
}

void Zone::water_off(void){
  bitClear(z_rambits,IS_ON);
}

void Zone::wind_sensor_on(void){
  bitSet(z_storebits,USE_WIND);
}

void Zone::wind_sensor_off(void){
  bitClear(z_storebits,USE_WIND);
}


Irrigation::Irrigation(void){

}

void Irrigation::run_all_zones(void){

}

void Irrigation::run_one_zone(uint8_t zn){

}

void Irrigation::stop(void){

}