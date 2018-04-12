
void loadSettingsConfig(){
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
  _storebits = 0;
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
  /*
  for ( i = 1; i <= _num_zones; i++){
    loadZoneConfig(i);
    _zone_storebits = 0;
    set_zone_run_time(10);
    set_zone_pin(0);
    set_moisture_id(0);
    //set_zone_name(Zone#); // TODO
    set_dry_level(0);
    set_blowout_time(40);
    set_blowout_cycles(4);
  }
  for ( i = 1; i <= _num_schedules; i++ ){
    loadScheduleConfig(i);
    _schedule_storebits = 0;
    //TODO...
  }
  */
  // Indicate reset by going back to menuRoot.
  menuSelected = menuRoot;
  menuLevel = menuRoot;
  for ( i = 0; i < 4; i++ ){
    menuParentTree[i] = 0;
  }
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
  Serial.print("dest: ");
  Serial.print(dest);
  Serial.print("; eeprom_dest: ");
  Serial.print(eeprom_dest);
  Serial.print("; IRR_STORE_BITS + _eeprom_start_addr: ");
  Serial.print(IRR_STORE_BITS + _eeprom_start_addr);
  Serial.print("; eeprom read: ");
  uint8_t x = EEPROM.readByte(IRR_STORE_BITS + _eeprom_start_addr);
  Serial.print(x);
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
  EEPROM.writeByte(IRR_RAIN_ID + _eeprom_start_addr, val);
  
}
void set_temp_id(uint16_t val){
  _temperature_id = val;
  EEPROM.writeByte(IRR_TEMPERATURE_ID + _eeprom_start_addr, val);
  
}
void set_wind_id(uint16_t val){
  _wind_id = val;
  EEPROM.writeByte(IRR_WIND_ID + _eeprom_start_addr, val);
  
}

// TODO work out weather forcast
void set_weather_id(uint16_t val){
  // = val;
  //EEPROM.writeByte( + _eeprom_start_addr, val);
}

// Time it takes to recharge the air compressor.
void set_blowout_wait(uint16_t val){
  _blowout_recharge_wait = val;
  EEPROM.writeByte(IRR_BLOWOUT_CHARGE_WAIT + _eeprom_start_addr, val);
}

