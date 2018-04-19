/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */


void initSettingsConfig(){
  //Serial.println("Loading settings");
  _eeprom_version = EEPROM.readByte(EEPROM_VERSION + _eeprom_start_addr);
  _storebits = EEPROM.readByte(IRR_STORE_BITS + _eeprom_start_addr);
  _is_enabled = is_enabled();
  _num_zones = EEPROM.readByte(IRR_NUM_ZONES + _eeprom_start_addr);
  if (_num_zones == 0 || _num_zones == 255){
    _num_zones = 5;
  }
  _master_valve_pin = EEPROM.readByte(IRR_MASTER_VALVE_PIN + _eeprom_start_addr);
  _rain_sensor_pin = EEPROM.readByte(IRR_RAIN_SESNOR_PIN + _eeprom_start_addr);
  _wind_id = EEPROM.readInt(IRR_WIND_ID + _eeprom_start_addr);
  _rain_id = EEPROM.readInt(IRR_RAIN_ID + _eeprom_start_addr);
  _temperature_id = EEPROM.readInt(IRR_TEMPERATURE_ID + _eeprom_start_addr);
  _blowout_recharge_wait = EEPROM.readInt(IRR_BLOWOUT_CHARGE_WAIT + _eeprom_start_addr);
}

// Reset all settings to some sane defaults for 1st run or for a reset.
void defaultReset(){
  int i = 0;
  // Settings
  _storebits = 0;
  set_eeprom_version(EEPROM_VERSION);
  set_is_enabled(1);
  set_master_valve_normally_open(1);
  set_zone_normally_open(1);
  set_rain_normally_open(1);
  set_number_of_zones(5);
  set_master_pin(0);
  set_rain_pin(0);
  set_rain_id(0);
  set_temp_id(0);
  set_wind_id(0);
  //set_weather_id(0); //TODO
  set_blowout_wait(90); // 90 seconds to re-charge

  // Schedule
  resetScheduleConfig();
  resetZoneConfig();

  // Indicate reset by going back to menuRoot.
  menuSelected = menuRoot;
  menuLevel = menuRoot;
  for ( i = 0; i < 4; i++ ){
    menuParentTree[i] = 0;
  }
}

void set_eeprom_version(uint8_t ver){
  _eeprom_version = ver;
  EEPROM.writeByte(IRR_EEPROM_VERSION + _eeprom_start_addr, ver);
}

bool in_eeprom(){
  return bitRead(_storebits,SETTING_BIT_IN_EEPROM);
}

//save in Ram and eeprom ( ram destination, position in ram, eeprom destinaion, value )
void saveBit(uint8_t &dest, uint16_t bit_position, int eeprom_dest, uint8_t val){
  if (val == 1){
    bitSet(dest,bit_position);
  } else {
    bitClear(dest,bit_position);
  }
  EEPROM.updateByte(eeprom_dest, dest);
}

//save in Ram and eeprom ( ram destination, position in ram, eeprom destinaion, value )
void save16Bit(uint16_t &dest, uint16_t bit_position, int eeprom_dest, uint8_t val){
  if (val == 1){
    bitSet(dest,bit_position);
  } else {
    bitClear(dest,bit_position);
  }
  EEPROM.updateInt(eeprom_dest, dest);
}
 
void save32Bit(uint32_t &dest, uint16_t bit_position, int eeprom_dest, uint8_t val){
  if (val == 1){
    bitSet(dest,bit_position);
  } else {
    bitClear(dest,bit_position);
  }
  EEPROM.updateLong(eeprom_dest, dest);
} 

bool is_enabled(){
  return bitRead(_storebits,SETTING_BIT_ENABLE);
}

void set_is_enabled(bool val){
  saveBit(_storebits, SETTING_BIT_ENABLE, IRR_STORE_BITS + _eeprom_start_addr, val);
}

bool master_valve_normally_open(){
  return bitRead(_storebits,SETTING_BIT_MASTER_VALVE_NORMALLY_OPEN);
}

void set_master_valve_normally_open(bool val){
  saveBit(_storebits, SETTING_BIT_MASTER_VALVE_NORMALLY_OPEN, IRR_STORE_BITS + _eeprom_start_addr, val);
}

bool zone_normally_open(){
  return bitRead(_storebits,SETTING_BIT_ZONE_NORMALLY_OPEN);
}

void set_zone_normally_open(bool val){
  saveBit(_storebits, SETTING_BIT_ZONE_NORMALLY_OPEN, IRR_STORE_BITS + _eeprom_start_addr, val);
}

bool rain_normally_open(){
  return bitRead(_storebits,SETTING_BIT_RAIN_NORMALLY_OPEN);
}

void set_rain_normally_open(bool val){
  saveBit(_storebits, SETTING_BIT_RAIN_NORMALLY_OPEN, IRR_STORE_BITS + _eeprom_start_addr, val);
}

void set_number_of_zones(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _num_zones = val;
  EEPROM.writeByte(IRR_NUM_ZONES + _eeprom_start_addr, val);
}

void set_master_pin(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _master_valve_pin = val;
  EEPROM.writeByte(IRR_MASTER_VALVE_PIN + _eeprom_start_addr, val);
}

void set_rain_pin(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _rain_sensor_pin = val;
  EEPROM.writeByte(IRR_RAIN_SESNOR_PIN + _eeprom_start_addr, val);
}

void set_rain_id(uint16_t val){
  _rain_id = val;
  EEPROM.writeInt(IRR_RAIN_ID + _eeprom_start_addr, val);
  
}

void set_temp_id(uint16_t val){
  _temperature_id = val;
  EEPROM.writeInt(IRR_TEMPERATURE_ID + _eeprom_start_addr, val);
  
}
void set_wind_id(uint16_t val){
  _wind_id = val;
  EEPROM.writeInt(IRR_WIND_ID + _eeprom_start_addr, val);
  
}

// TODO work out weather forcast
void set_weather_id(uint16_t val){
  // = val;
  //EEPROM.writeByte( + _eeprom_start_addr, val);
}

// Time it takes to recharge the air compressor.
void set_blowout_wait(uint16_t val){
  _blowout_recharge_wait = val;
  EEPROM.writeInt(IRR_BLOWOUT_CHARGE_WAIT + _eeprom_start_addr, val);
}


