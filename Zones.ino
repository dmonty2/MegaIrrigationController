// Caculate each zone's EEPROM area.
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


void water_on(){
  _is_running = 1;
  if(_zone_pin >= 1){
    if (zone_normally_open()){
      digitalWrite(_zone_pin, HIGH);
    } else {
      digitalWrite(_zone_pin, LOW);
    }
  }
}

void water_off(){
  _is_running = 0;
  if(_zone_pin >= 1){
    if (zone_normally_open()){
      digitalWrite(_zone_pin, LOW);
    } else {
      digitalWrite(_zone_pin, HIGH);
    }
  }
}

void wind_sensor_on(){
  bitSet(_zone_storebits,ZONE_BIT_USE_WIND);
}

void wind_sensor_off(void){
  bitClear(_zone_storebits,ZONE_BIT_USE_WIND);
}

void set_zone_name(char new_name[ZONE_NAME_SIZE]){
  strcpy(_zone_name,new_name);
  EEPROM.updateBlock<char>(ZONE_NAME + _zone_eeprom_offset, _zone_name, sizeof(_zone_name));
}

bool zone_in_eeprom(){
  return bitRead(_zone_storebits,ZONE_BIT_IN_EEPROM);
}

bool zone_is_enabled(){
  return bitRead(_zone_storebits,ZONE_BIT_ENABLED);
}

bool zone_use_moisture(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_MOISTURE);
}

bool zone_use_rain(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_RAIN);
}

bool zone_use_wind(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_WIND);
}

bool zone_use_temp(){
  return bitRead(_zone_storebits,ZONE_BIT_USE_TEMP);
}

bool zone_use_mini_cycle(){
  return bitRead(_zone_storebits,ZONE_BIT_MINI_CYCLE);
}

void set_zone_run_time(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _zone_run_time = val;
  EEPROM.writeByte(ZONE_RUNTIME + _zone_eeprom_offset, val);
}
void set_zone_pin(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _zone_pin = val;
  EEPROM.writeByte(ZONE_PIN + _zone_eeprom_offset, val);
}
void set_moisture_id(uint16_t val){
  _zone_moisture_id = val;
  EEPROM.writeByte(ZONE_MOISTURE_ID + _zone_eeprom_offset, val);
}
void set_dry_level(uint16_t val){
  _zone_is_dry_value = val;
  EEPROM.writeByte(ZONE_IS_DRY_VALUE + _zone_eeprom_offset, val);
}
void set_blowout_time(uint16_t val){
  _zone_blowout_time = val;
  EEPROM.writeByte(ZONE_BLOWOUT_TIME + _zone_eeprom_offset, val);
}
void set_blowout_cycles(uint16_t val){
  if (val >= 255){
    val = 255;
  }
  _zone_blow_cycles = val;
  EEPROM.writeByte(ZONE_BLOWOUT_CYCLES + _zone_eeprom_offset, val);
}