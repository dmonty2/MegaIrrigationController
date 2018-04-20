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
    scheduleTracker[i].curent_running_zone = 0; // 0 = not running.  1+ means this schedule is running.
    scheduleTracker[i].next_start = calculate_next_start_today(_schedule_start_time); // 0 = disabled.
  }
}

// Called from menu to populate the menu values
void loadScheduleConfig(uint8_t num){
  _schedule_number = num;
  set_schedule_eeprom_offset();
  _schedule_storebits = EEPROM.readByte(SCHEDULE_STORE_BITS + _schedule_eeprom_offset);
  _schedule_start_time = EEPROM.readInt(SCHEDULE_START_TIME + _schedule_eeprom_offset);
  _schedule_repeat_delay = EEPROM.readInt(SCHEDULE_REPEAT_DELAY + _schedule_eeprom_offset);
  _schedule_every_nth_day = EEPROM.readInt(SCHEDULE_EVERY_NTH_DAY + _schedule_eeprom_offset);
  _schedule_zones = EEPROM.readLong(SCHEDULE_ZONES + _schedule_eeprom_offset);
}

void resetScheduleConfig(){
  for ( int i = 1; i <= NUMBER_OF_SCHEDULES; i++ ){
    _schedule_number = i;
    set_schedule_eeprom_offset();
    _schedule_storebits = 0;
    EEPROM.updateInt(SCHEDULE_STORE_BITS + _schedule_eeprom_offset, _schedule_storebits);
    set_schedule_start_time(0);
    set_schedule_repeat_delay(0);
    set_schedule_every_nth_day(0);
    EEPROM.updateLong(SCHEDULE_ZONES + _schedule_eeprom_offset, 0);
  }  
}

// Check schedule for when to start zones.
void checkSchedule(){
  // TODO at midnight set schedule for the day.
  if (_current_running_zone >= 1 && _current_running_scedule == 0 ){
    // Loop through all schedules and check next run times.
    int current = hour() * 60 + minute();
    for (int i = 1; i <= NUMBER_OF_SCHEDULES; i++ ){
      // Check all schedule start times - if a schedule is running then the others will delay.
      if ( scheduleTracker[i].next_start != 0 && current > scheduleTracker[i].next_start ){
        startSchedule(i);
      }
    }
  }
  if (_current_running_zone == 0 && _current_running_scedule >= 1 ){
    // choose the next zone to run...
    int zone = scheduleTracker[_current_running_scedule].curent_running_zone + 1;
    bool found_zone = 0;
    for ( zone; zone <= _num_zones; zone++ ){
      if ( schedule_water_zone(zone) ){
        scheduleTracker[_current_running_scedule].curent_running_zone = zone;
        loadZoneConfig(zone);
        water_on();
        found_zone = 1;
      }
    }
    if ( found_zone == 0 ){
      // No more zones turn off.
      scheduleTracker[_current_running_scedule].curent_running_zone = 0;
      _current_running_scedule = 0;
      all_zones_off();  // TODO safety.
    }
  }
}

// Run a schedule
void startSchedule(int schedule_num){
  _current_running_scedule = schedule_num;
  loadScheduleConfig(schedule_num);
  for ( int zone = 1; zone <= _num_zones; zone++){
    if ( schedule_water_zone(zone) ){
      scheduleTracker[schedule_num].curent_running_zone = zone;
      loadZoneConfig(zone);
      water_on();
    }
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

void set_schedule_start_time(uint16_t val){
  _schedule_start_time = val;
  EEPROM.writeInt(SCHEDULE_START_TIME + _schedule_eeprom_offset, val);
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
    // TODO - handle repeat
    /*
    if ( current > sched_time &&  _schedule_repeat_delay > 0 ) ){
      int total_runtime = sum_zone_runtime();
      
      next = sched_time + _schedule_repeat_delay;
    }
    */
    return next;
  } else {
    return 0;
  }
}

// For repeat we need to find the sum of all zone runtimes 
int sum_zone_time(){
  int total_time = 0;
  for ( int i = 1; i <= _num_zones; i++){
    if ( schedule_water_zone(i)){
      total_time += get_zone_runtime(i);
    }
  }
  return total_time;
}

