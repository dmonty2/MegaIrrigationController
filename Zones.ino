/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

void initZoneConfig(){
  
}



// Caculate each zone's EEPROM area.
void set_zone_eeprom_offset(){
  int _schedule_eeprom_end = (int)(_eeprom_start_addr + IRR_EEPROM_BYTES + (NUMBER_OF_SCHEDULES * SCHEDULE_EEPROM_BYTES));
  _zone_eeprom_offset = (int)(_schedule_eeprom_end + (((int)_zone_number - 1) * ZONE_EEPROM_BYTES));
}

// Called from menu.
void loadZoneConfig(uint8_t num){
  _zone_number = num;
  set_zone_eeprom_offset();
  if ( zone_initialized() == false ){
    resetZone(num, true);
  }
  _zone_storebits = EEPROM.readByte(ZONE_STORE_BITS + _zone_eeprom_offset);
  _zone_pin = EEPROM.readByte(ZONE_PIN + _zone_eeprom_offset);
  EEPROM.readBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
  _zone_run_time = EEPROM.readByte(ZONE_RUNTIME + _zone_eeprom_offset);
  _zone_blowout_time = EEPROM.readInt(ZONE_BLOWOUT_TIME + _zone_eeprom_offset);
  _zone_blow_cycles = EEPROM.readByte(ZONE_BLOWOUT_CYCLES + _zone_eeprom_offset);
  _zone_is_dry_value = EEPROM.readInt(ZONE_IS_DRY_VALUE + _zone_eeprom_offset);
  _zone_moisture_id = EEPROM.readInt(ZONE_MOISTURE_ID + _zone_eeprom_offset);
}

// Force reset all zones.
void resetZoneConfig(){
  for ( int i = 1; i <= _num_zones; i++){
    resetZone(i, true);
  }  
}

void resetZone(int zone_num, bool force){
  _zone_number = zone_num;
  set_zone_eeprom_offset();
  _zone_storebits = 0;
  set_zone_is_enabled(0); // Saves _zone_storebits.
  set_zone_run_time(10);
  set_zone_pin(0);
  set_zone_moisture_id(0);
  // Fill in default "Zone 1 " "Zone 2 " including trailing whitespace
  strcpy(_zone_name, "Zone ");
  char num[3];
  itoa(zone_num, num, 10);
  strcat(_zone_name, num);
  if (zone_num <= 9){
    strcat(_zone_name, " ");
  }
  //Fill in rest of bits with " " whitespace.
  for (int n=8; n < ZONE_NAME_SIZE; n++){
    strcat(_zone_name, " ");
  }
  set_zone_name();  // Write to eeprom
  set_zone_dry_level(0);
  set_zone_blowout_time(40);
  set_zone_blowout_cycles(4);
  set_zone_initialized();
}

void updateZoneConfig(){
  set_zone_eeprom_offset();
  EEPROM.updateByte(ZONE_STORE_BITS + _zone_eeprom_offset,(uint8_t)_zone_storebits);
  EEPROM.updateByte(ZONE_PIN + _zone_eeprom_offset,(uint8_t)_zone_pin);
  EEPROM.updateBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
  EEPROM.updateByte(ZONE_RUNTIME + _zone_eeprom_offset,(uint8_t)_zone_run_time);
  EEPROM.updateInt(ZONE_BLOWOUT_TIME + _zone_eeprom_offset,(uint8_t)_zone_blowout_time);
  EEPROM.updateByte(ZONE_BLOWOUT_CYCLES + _zone_eeprom_offset,(uint8_t)_zone_blow_cycles);
  EEPROM.updateInt(ZONE_IS_DRY_VALUE + _zone_eeprom_offset,(uint8_t)_zone_is_dry_value);
  EEPROM.updateInt(ZONE_MOISTURE_ID + _zone_eeprom_offset,(uint8_t)_zone_moisture_id);
  EEPROM.updateByte(ZONE_INITIALIZED + _zone_eeprom_offset, (uint8_t)170);
}

void water_on(){
  _current_running_zone = _zone_number;
  _zone_timer_start = _currentMillis;
  if ( _manual_zones_running >= 1 ){
    _zone_timer_end = _manual_zones_time[_zone_number - 1] * 60 * 1000; // Manual Run
  } else {
    _zone_timer_end = _zone_run_time * 60 * 1000; // Schedule Run
  }
  // TODO Master Valve
  if(_zone_pin >= 1){
    if (zone_normally_open()){
      digitalWrite(_zone_pin, HIGH);
    } else {
      digitalWrite(_zone_pin, LOW);
    }
  }
}

void water_off(){
  _current_running_zone = 0;
  _manual_zones_time[_zone_number - 1] = 0;
  if(_zone_pin >= 1){
    if (zone_normally_open()){
      digitalWrite(_zone_pin, LOW);
    } else {
      digitalWrite(_zone_pin, HIGH);
    }
  }
}

void run_all_zones(void){
  // TODO - me think this is now part of schedule as run schedule. Not needed?
  _systemState = stateRunningAllZones;
  for (uint8_t zone_num = 0; zone_num <= _num_zones; zone_num++ ){
    loadZoneConfig(zone_num);
  }

}

void run_some_zones(void){
  _systemState = stateRunningSomeZones;

}

// TODO
void run_one_zone(uint8_t zone){
  
}

// TODO
void blowout_zones(void){

}

// Safety - call this function to ensure zones are off.
void all_zones_off(){
  for ( uint8_t zone=1; zone <= _num_zones; zone++ ){
    loadZoneConfig(zone);
    water_off();
  }
  _manual_zones_running = 0;
  _current_running_zone = 0;
  _current_running_scedule = 0;
  _systemState = stateOff;
  // TODO master valve off.
}

void checkZoneTimer(){
  if ( _current_running_zone >= 1 && (_currentMillis - _zone_timer_start) >= _zone_timer_end){
    water_off();
  }
}

void wind_sensor_on(){
  bitSet(_zone_storebits,ZONE_BIT_USE_WIND);
}

void wind_sensor_off(void){
  bitClear(_zone_storebits,ZONE_BIT_USE_WIND);
}

void set_zone_name(){
  EEPROM.updateBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
}

bool zone_in_eeprom(){
  return bitRead(_zone_storebits,ZONE_BIT_IN_EEPROM);
}

bool zone_is_enabled(){
  return bitRead(_zone_storebits,ZONE_BIT_ENABLED);
}

void set_zone_is_enabled(bool val){
  saveBit(_zone_storebits, ZONE_BIT_ENABLED, ZONE_STORE_BITS + _zone_eeprom_offset, val);
}

bool zone_use_moisture(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_MOISTURE);
}

void set_zone_use_moisture(bool val){
  saveBit(_zone_storebits, ZONE_BIT_USE_MOISTURE, ZONE_STORE_BITS + _zone_eeprom_offset, val);
}

bool zone_use_rain(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_RAIN);
}

void set_zone_use_rain(bool val){
  saveBit(_zone_storebits, ZONE_BIT_USE_RAIN, ZONE_STORE_BITS + _zone_eeprom_offset, val);
}

bool zone_use_wind(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_WIND);
}

void set_zone_use_wind(bool val){
  saveBit(_zone_storebits, ZONE_BIT_USE_WIND, ZONE_STORE_BITS + _zone_eeprom_offset, val);
}

bool zone_use_temp(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_TEMP);
}

void set_zone_use_temp(bool val){
  saveBit(_zone_storebits, ZONE_BIT_USE_TEMP, ZONE_STORE_BITS + _zone_eeprom_offset, val);
}

bool zone_use_mini_cycle(){
  return bitRead(_zone_storebits,ZONE_BIT_MINI_CYCLE);
}

//TODO
void set_zone_use_mini_cycle(bool val){
  saveBit(_zone_storebits, ZONE_BIT_MINI_CYCLE, ZONE_STORE_BITS + _zone_eeprom_offset, val);
  
}

void set_zone_run_time(uint16_t val){
  // TODO if menuScheduleRepeatDelay > 0 && sum of zones > menuScheduleRepeatDelay then update menuScheduleRepeatDelay to menuScheduleRepeatDelay + 1;
  if (val >= 255){
    val = 255;
  }
  _zone_run_time = val;
  EEPROM.updateByte(ZONE_RUNTIME + _zone_eeprom_offset, val);
}
void set_zone_pin(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _zone_pin = val;
  EEPROM.updateByte(ZONE_PIN + _zone_eeprom_offset, val);
}
void set_zone_moisture_id(uint16_t val){
  _zone_moisture_id = val;
  EEPROM.updateInt(ZONE_MOISTURE_ID + _zone_eeprom_offset, val);
}
void set_zone_dry_level(uint16_t val){
  _zone_is_dry_value = val;
  EEPROM.updateInt(ZONE_IS_DRY_VALUE + _zone_eeprom_offset, val);
}
void set_zone_blowout_time(uint16_t val){
  _zone_blowout_time = val;
  EEPROM.updateInt(ZONE_BLOWOUT_TIME + _zone_eeprom_offset, val);
}
void set_zone_blowout_cycles(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _zone_blow_cycles = val;
  EEPROM.updateByte(ZONE_BLOWOUT_CYCLES + _zone_eeprom_offset, val);
}

// Called by schedule to calculate total runtime for repeat schedules
int get_zone_runtime(int zone_num){
  int offset = (int)(_eeprom_start_addr + (zone_num - 1) * ZONE_EEPROM_BYTES);
  return EEPROM.readByte(ZONE_RUNTIME + offset);
}

bool zone_initialized(){
  if (EEPROM.readByte(ZONE_INITIALIZED + _zone_eeprom_offset) == 170){
    return true;
  } else {
    return false;
  }
}

void set_zone_initialized(){
  EEPROM.updateByte(ZONE_INITIALIZED + _zone_eeprom_offset, (uint8_t)170);
}

