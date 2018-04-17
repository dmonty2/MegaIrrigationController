/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

 
void loadScheduleConfig(uint8_t num){
  _schedule_number = num;
  set_schedule_eeprom_offset();
  _schedule_storebits = EEPROM.readByte(SCHEDULE_STORE_BITS + _zone_eeprom_offset);
  /*_zone_pin = EEPROM.readByte(ZONE_PIN + _zone_eeprom_offset);
  EEPROM.readBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
  _zone_run_time = EEPROM.readByte(ZONE_RUNTIME + _zone_eeprom_offset);
  _zone_blowout_time = EEPROM.readInt(ZONE_BLOWOUT_TIME + _zone_eeprom_offset);
  _zone_blow_cycles = EEPROM.readByte(ZONE_BLOWOUT_CYCLES + _zone_eeprom_offset);
  _zone_is_dry_value = EEPROM.readInt(ZONE_IS_DRY_VALUE + _zone_eeprom_offset);
  _zone_moisture_id = EEPROM.readInt(ZONE_MOISTURE_ID + _zone_eeprom_offset);
  */
}

// Caculate each zone's EEPROM area.
void set_schedule_eeprom_offset(){
  _schedule_eeprom_offset = (int)(_eeprom_start_addr + (((int)_schedule_number - 1) * SCHEDULE_EEPROM_BYTES));
}

bool schedule_is_enabled(){
  return bitRead(_schedule_storebits,SCHEDULE_BIT_ENABLED);
}

void set_schedule_is_enabled(bool val){
  saveBit(_schedule_storebits, SCHEDULE_BIT_ENABLED, SCHEDULE_STORE_BITS + _schedule_eeprom_offset, val);
}


bool schedule_water_any_day(){
  return bitRead(_schedule_storebits,SCHEDULE_BIT_ANY_DAY);
}

void set_schedule_water_any_day(bool val){
  saveBit(_schedule_storebits, SCHEDULE_BIT_ANY_DAY, SCHEDULE_STORE_BITS + _schedule_eeprom_offset, val);
}

bool schedule_water_day(){
  return bitRead(_schedule_storebits,SCHEDULE_BIT_SUN);
}

bool is_water_day(){
  int day_of_week = weekday();
  int day_of_month = day();
  if (bitRead(_schedule_storebits,SCHEDULE_BIT_ANY_DAY)){
    return true;
  }
  if ( bitRead(_schedule_storebits, day_of_week) ){
    return true;
  }
  if ( bitRead(_schedule_storebits, SCHEDULE_BIT_EVEN) && (day_of_month%2 == 0) ){
    return true;
  }
  if ( bitRead(_schedule_storebits, SCHEDULE_BIT_ODD) && (day_of_month%2 == 1) ){
    return true;
  }
  if ( bitRead(_schedule_storebits, SCHEDULE_BIT_NTH_DAY) &&
              ( _schedule_every_nth_day > 0 ) &&
              ( day_of_month % _schedule_every_nth_day == 0 )){
    return true;
  }
  return false;
}

void set_schedule_water_sun(bool val){
  saveBit(_schedule_storebits, SCHEDULE_BIT_SUN, SCHEDULE_STORE_BITS + _schedule_eeprom_offset, val);
}
