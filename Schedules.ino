/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

// Initialize schedule timer.
void initScheduleConfig(){
  for (int i = 1; i <= NUMBER_OF_SCHEDULES; i++ ){
    loadScheduleConfig(i);
    scheduleTracker[i].is_running = 0;
    scheduleTracker[i].next_start1 = calculate_next_start_today(_schedule_start_time_1); // 0 = disabled.
    scheduleTracker[i].next_start2 = calculate_next_start_today(_schedule_start_time_2);  // 0 = disabled.
    // TODO: next_start1 next_start2; reset each to 0 when run.
    // this way overlapping times will run back to back. 
    // next_start1 != 0 && current > next_start1; then water.
  }
}

// Called from menu to populate the menu values
void loadScheduleConfig(uint8_t num){
  _schedule_number = num;
  set_schedule_eeprom_offset();
  _schedule_storebits = EEPROM.readByte(SCHEDULE_STORE_BITS + _schedule_eeprom_offset);
  _schedule_start_time_1 = EEPROM.readInt(SCHEDULE_START_TIME_1 + _schedule_eeprom_offset);
  _schedule_start_time_2 = EEPROM.readInt(SCHEDULE_START_TIME_2 + _schedule_eeprom_offset);
  _schedule_repeat_delay = EEPROM.readInt(SCHEDULE_REPEAT_DELAY + _schedule_eeprom_offset);
  _schedule_every_nth_day = EEPROM.readInt(SCHEDULE_EVERY_NTH_DAY + _schedule_eeprom_offset);
  _schedule_zones = EEPROM.readLong(SCHEDULE_ZONES + _schedule_eeprom_offset);
}

void resetScheduleConfig(){
  for ( int i = 1; i <= NUMBER_OF_SCHEDULES; i++ ){
    _schedule_number = num;
    set_schedule_eeprom_offset();
    _schedule_storebits = 0;
    EEPROM.updateInt(SCHEDULE_STORE_BITS + _schedule_eeprom_offset, _schedule_storebits);
    set_schedule_start_time1(0);
    set_schedule_start_time2(0);
    set_schedule_repeat_delay(0);
    set_schedule_every_nth_day(0);
    EEPROM.updateLong(SCHEDULE_ZONES + _schedule_eeprom_offset, 0);
  }  
}

// Check schedule for when to start zones.
void checkSchedule(){
  // TODO at midnight set schedule for the day.
  if (_is_running == 0){

  }
}

// Caculate each zone's EEPROM area.
void set_schedule_eeprom_offset(){
  // when addressing eeprom schedule num starts at zero;
  int num = _schedule_number;
  num -= 1;
  if ( num <= -1 ){
    num = 0;
    _schedule_number = 1;
  }
  _schedule_eeprom_offset = (int)(_eeprom_start_addr + IRR_EEPROM_BYTES + (num * SCHEDULE_EEPROM_BYTES));
}

bool schedule_is_enabled(){
  return bitRead(_schedule_storebits,SCHEDULE_BIT_ENABLED);
}

void set_schedule_is_enabled(bool val){
  save16Bit(_schedule_storebits, SCHEDULE_BIT_ENABLED, SCHEDULE_STORE_BITS + _schedule_eeprom_offset, val);
}

// read sun-sat,any,even,odd,nth (1-11)
bool schedule_day_bit(uint16_t bit_location){
  return bitRead(_schedule_storebits,bit_location);
}

// save sun-sat,any,even,odd,nth (1-11)
void set_schedule_day_bit(uint16_t bit_location, bool val){
  save16Bit(_schedule_storebits, bit_location, SCHEDULE_STORE_BITS + _schedule_eeprom_offset, val);
}

void set_schedule_start_time1(uint16_t val){
  _schedule_start_time_1 = val;
  EEPROM.writeInt(SCHEDULE_START_TIME_1 + _schedule_eeprom_offset, val);
}

void set_schedule_start_time2(uint16_t val){
  _schedule_start_time_2 = val;
  EEPROM.writeInt(SCHEDULE_START_TIME_2 + _schedule_eeprom_offset, val);
}

void set_schedule_repeat_delay(uint16_t val){
  _schedule_repeat_delay = val;
  EEPROM.writeInt(SCHEDULE_REPEAT_DELAY + _schedule_eeprom_offset, val);
}

void set_schedule_every_nth_day(uint16_t val){
  _schedule_every_nth_day = val;
  EEPROM.writeInt(SCHEDULE_EVERY_NTH_DAY + _schedule_eeprom_offset, val);
}

// zones that this schedule will water (0-32)
bool schedule_water_zone(uint16_t zone){
  return bitRead(_schedule_zones,zone);
}

void set_schedule_water_zone(uint16_t zone, bool val){
  save32Bit(_schedule_zones, zone, SCHEDULE_ZONES + _schedule_eeprom_offset, val);
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

// Call at midnight and after a water cycle 
int calculate_next_start_today(uint16_t &sched_time){
  if (schedule_is_enabled() && is_water_day()){
    int next, current = 0;
    current = hour() * 60 + minute();
    if ( current < sched_time ){
      next = sched_time;
    }
    if ( current > sched_time &&  current <= (_schedule_repeat_delay + sched_time) ){
      next = sched_time + _schedule_repeat_delay;
    }
    return next;
  } else {
    return 0;
  }
}
