/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

// Dictionary to only store strings/words once in memory and join them together.
// This may not be worth the overhead of the functions needed to join strings.
// TODO: if there is enough free memory I may later change these from char
//       arrays to String objects.  In order to make the code more readable.
//       Also test progmem storage.
static const char txtActions[8] = "actions";
static const char txtSettings[9] = "settings";
static const char txtEnable[7] = "enable";
static const char txtRun[4] = "run";
static const char txtAll[4] = "all";
static const char txtSome[5] = "some";
static const char txtZone[5] = "zone";
static const char txtZones[6] = "zones";
static const char txtName[5] = "name";
static const char txtBlowout[8] = "blowout";
static const char txtNumberOf[10] = "number of";
static const char txtMaster[7] = "master";
static const char txtPIN[4] = "PIN";
static const char txtID[3] = "ID";
static const char txtNOpen[7] = "N.Open";
static const char txtTemp[5] = "temp";
static const char txtRain[5] = "rain";
static const char txtDry[4] = "dry";
static const char txtWind[5] = "wind";
static const char txtFreeze[7] = "freeze";
static const char txtWeather[8] = "weather";
static const char txtWait[5] = "wait";
static const char txtTime[5] = "time";
static const char txtMoisture[9] = "moisture";
static const char txtUseForecast[13] = "use forecast";
static const char txtAvoid[6] = "avoid";
static const char txtCycles[7] = "cycles";
static const char txtMini[5] = "mini";
static const char txtSchedule[9] = "schedule";
static const char txtDay[4] = "day";
static const char txtStart[6] = "start";
static const char txtRepeat[7] = "repeat";
static const char txtDelay[6] = "delay";
static const char txtEven[5] = "even";
static const char txtOdd[4] = "odd";
static const char txtAny[4] = "any";
static const char txtMin[4] = "min";
static const char txtSec[4] = "sec";
static const char txtSpecific[9] = "specific";
static const char txtInterval[9] = "interval";
static const char txtSet[4] = "set";
static const char txtLevel[6] = "level";
static const char txtYes[4] = "Yes";
static const char txtNo[3] = "No";
static const char txtDisabled[9] = "disabled";
static const char txtDefaultReset[14] = "default reset";


// Setup Menu flag type bits
void initializeMenu(){
  // Clear the bits
  int idx = 0;
  int num_items = 0;
  for (idx = menuRoot; idx <= menuLast; idx++){
    menuBits[idx] = 0;
  }
  // Branch Menu Items
  num_items = 5;
  int itemsBranch[num_items] = {menuRoot,menuActions,menuSettings,menuZone,menuSchedule};
  for (idx=0; idx < num_items; idx++){
    bitSet(menuBits[itemsBranch[idx]],menuBitIsBranch);
  }
  // Yes/No Menu Items
  num_items = 11;
  int itemsYN[num_items] = { menuEnable, menuMasterNormallyOpen, menuZoneNormallyOpen, menuRainNormallyOpen,
                      menuDefaultReset, menuZoneEnable, menuUseForecast, menuAvoidWind, menuAvoidFreeze,
                      menuMiniCycles, menuScheduleEnable };
  for (idx=0; idx < num_items; idx++){
    bitSet(menuBits[itemsYN[idx]],menuBitInputYesNo);
  }
  // Number Menu Items
  num_items = 14;
  int itemsNum[num_items] = { menuNumberOfZones, menuMasterPin, menuRainPin, menuRainID, menuTempID, menuWindID,
                       menuWeatherID, menuBlowOutWait, menuZoneRunTime, menuZonePin, menuMoistureID,
                       menuDryLevel, menuBlowOutTime, menuBlowOutCycles };
  for (idx=0; idx < num_items; idx++){
    bitSet(menuBits[itemsNum[idx]],menuBitInputNumber);
  }

  // Number Zones/Schedules
  bitSet(menuBits[menuZones], menuBitIsNumList);
  bitSet(menuBits[menuSchedules], menuBitIsNumList);

  // Text item
  bitSet(menuBits[menuZoneName], menuBitInputText);

  // Time item
  bitSet(menuBits[menuScheduleStartTime], menuBitInputTime);
}

// Track traversing into sub-menus & load settings from memory
void menuLevelEnter (uint8_t val){
  for (uint8_t i = 0; i <= 3; i++){
    if (menuParentTree[i] == 0){
      menuParentTree[i] = val;
      menuLevel = val;
      if (bitRead(menuBits[menuLevel], menuBitInputYesNo)){
        menuBoolVal = getBitVal(menuLevel);
      }
      if (bitRead(menuBits[menuLevel], menuBitInputNumber)){
        menuNumVal = getNumVal(menuLevel); 
      }
      if (bitRead(menuBits[menuLevel], menuBitIsNumList)){
        menuNumVal = 1;
      }
      return;
    }
  }
}

// Track traversing out of sub-menus.
void menuLevelExit(){
  for (uint8_t i = 3; i>=0 && i<=3; i--){
    if (menuParentTree[i] > 0){
      // 1 5 0 0
      menuSelected = menuParentTree[i];
      menuParentTree[i] = 0;
      if ( i > 0 ) {
        menuLevel = menuParentTree[i - 1];
      } else {
        menuLevel = menuRoot;
      }
      return;
    }
    menuLevel = menuRoot;
    menuSelected = menuEnable;
  }
}
// ====== Detect key-pad button press =======
// Buttons change behaviour based on sub-menu.
void checkButtonPress(){
  if (_currentMillis - _previousDebounceMillis > BUTTON_DEBOUNCE ){
    _previousDebounceMillis = _currentMillis;
  } else {
    return;
  }
  int adc_key_in = analogRead(0);
  if (adc_key_in > 1000){
    btnCurrent = btnNone;
    _autoRepeatRate = BUTTON_AUTO_REPEAT_SLOW_RATE;
    _autoRepeatStartMillis = _currentMillis;
    return;
  }
  else if (adc_key_in < 50){
    btnCurrent = btnRight;  
  }
  else if (adc_key_in < 250){
    btnCurrent = btnUp; 
  }
  else if (adc_key_in < 450){
    btnCurrent = btnDown; 
  }
  else if (adc_key_in < 650){
    btnCurrent = btnLeft; 
  }
  else if (adc_key_in < 850){
    btnCurrent = btnSelect;
  } else {
    btnCurrent = btnNone;
    _autoRepeatRate = BUTTON_AUTO_REPEAT_SLOW_RATE;
    _autoRepeatStartMillis = _currentMillis;
    return;
  }

  // Handle Auto Repeat button speed.
  if ( _currentMillis - _autoRepeatMillis > _autoRepeatRate ){
    _autoRepeatMillis = _currentMillis;
    if (_currentMillis - _autoRepeatStartMillis > BUTTON_AUTO_REPEAT_FAST_DELAY){
      _autoRepeatRate = BUTTON_AUTO_REPEAT_FAST_RATE;  
    }

    // Navigation keys change based on type of input.
    if(bitRead(menuBits[menuLevel],         menuBitIsBranch)){
      navigateBranch();
    } else if (bitRead(menuBits[menuLevel], menuBitInputYesNo)){
      naviagteYesNo();
    } else if (bitRead(menuBits[menuLevel], menuBitInputNumber)){
      navigateNumberChooser();
    } else if (bitRead(menuBits[menuLevel], menuBitIsNumList)){
      navigateZoneSchedule();
    }
    
    updateDisplay();
  }
}

// Button layout for navigating menu branches
void navigateBranch(){
  if (btnCurrent == btnDown){
    menuSelected = menuSelected + 1;
    if ( menuSelected > menuEnd ){
      menuSelected = menuStart;
    }
  }
  if (btnCurrent == btnUp){
    menuSelected = menuSelected - 1;
    if ( menuSelected < menuStart ){
      menuSelected = menuEnd;
    }
  }
  if (btnCurrent == btnRight || btnCurrent == btnSelect){
    menuLevelEnter(menuSelected);
  }
  if (btnCurrent == btnLeft){
    menuLevelExit();
  }
}

// Navigating a Yes/No leaf
void naviagteYesNo(){ 
  if (btnCurrent == btnDown || btnCurrent == btnUp){
    menuBoolVal = !menuBoolVal;
  }
  if (btnCurrent == btnLeft){
    menuLevelExit();
  }
  if (btnCurrent == btnSelect){
    setBitVal(menuLevel);
  }
}

// Navigating a Number chooser e.g. ID, PIN, etc.
// TODO if we are navigating menuScheduleRepeatDelay skip from 0 to sum of all zone times.
void navigateNumberChooser(){
  if (btnCurrent == btnDown && menuNumVal > 0){
      menuNumVal -= 1;
  }
  if ( btnCurrent == btnUp && menuNumVal <= 65000){
      menuNumVal += 1;
  }
  if (btnCurrent == btnLeft){
    menuLevelExit();
  }
  if (btnCurrent == btnSelect){
    setNumVal(menuLevel);
  }
}

// Navigating a List of Zones or Schedules
void navigateZoneSchedule(){
  if (btnCurrent == btnDown && menuNumVal > 1){
      menuNumVal -= 1;
  }
  uint8_t max_num = 1;
  if ( menuLevel == menuZones ){ max_num = _num_zones; }
  if ( menuLevel == menuSchedules ){ max_num = NUMBER_OF_SCHEDULES; }
  if ( btnCurrent == btnUp && menuNumVal < max_num ){
      menuNumVal += 1;
  }
  if ( menuNumVal <= 0 || menuNumVal > max_num ){
    menuNumVal = 1;
  }
  if (btnCurrent == btnLeft){
    menuLevelExit();
  }
  if (btnCurrent == btnRight){
    if (menuLevel == menuZones){
      loadZoneConfig(menuNumVal);
      menuLevelEnter(menuZone);
    }
    if (menuLevel == menuSchedules){
      loadScheduleConfig(menuNumVal);
      menuLevelEnter(menuSchedule);
    }
  }
}
/*
void menuBitRead(menuItems menuIdx, menuItemBits menuBitPos){
  bitRead(menuBitProperties[menuIdx], menuBitPos);
}
*/
void updateDisplay(){
  char line1[17] = "                ";  //TODO: not sure if screen needs end of line
  char line2[17] = "                ";
  // == Branch Display ==
  if (bitRead(menuBits[menuLevel],menuBitIsBranch)){
    switch (menuLevel) {
      case menuRoot:
        menuStart = menuActions;
        menuEnd = menuSchedules;
        break;
      case menuActions:
        menuStart = menuEnable;
        menuEnd = menuBlowOutZones;
        break;
      case menuSettings:
        menuStart = menuNumberOfZones;
        menuEnd = menuDefaultReset;
        break;
      case menuZone:
        menuStart = menuZoneEnable;
        menuEnd = menuMiniCycles;
        break;
      case menuSchedule:
        menuStart = menuScheduleEnable;
        menuEnd = menuScheduleRepeatDelay;
        break;
    }
    // Show two items of current display
    if ( menuSelected > menuEnd ){
      menuSelected = menuStart;
    }
    if ( menuSelected < menuStart ){
      menuSelected = menuEnd;
    }
    if ( btnCurrent == btnRight ) {
      menuSelected = menuStart;
    }
    getMenuText(line1, menuSelected);
    if ( menuSelected + 1 <= menuEnd ){
      getMenuText(line2, menuSelected + 1);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(">");
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.write(" ");
    lcd.print(line2);
  }
  // == Yes No Display ==
  if (bitRead(menuBits[menuLevel], menuBitInputYesNo)){
    getMenuText(line1, menuSelected);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    bool currentVal = getBitVal(menuLevel);
    if ( menuBoolVal == 1 ){
      strcpy(line2, txtYes);
      if (currentVal == 1){
        strcat(line2, "*");
      }
    } else {
      strcpy(line2, txtNo);
      if (currentVal == 0){
        strcat(line2, "*");
      }
    }
    line2[0] = toupper(line2[0]);
    lcd.print(line2); 
  }
  // == Number Display ==
  if (bitRead(menuBits[menuLevel], menuBitInputNumber)){
    getMenuText(line1, menuSelected);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    if ( menuNumVal == 0 ){
      strcpy(line2, "0 ");
      strcat(line2, txtDisabled);
      lcd.print(line2);
    } else {
      lcd.print(menuNumVal);
    }
  }
  // == Text Display ==
  if (bitRead(menuBits[menuLevel], menuBitInputText)){
    getMenuText(line1, menuSelected);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    if ( menuNumVal == 0 ){
      strcpy(line2, 0);
      strcat(line2, " ");
      strcat(line2, txtDisabled);
    } else {
      strcpy(line2, menuNumVal);
    }
    lcd.print(line2);
  }
  // == Time Display ==
  if (bitRead(menuBits[menuLevel], menuBitInputTime)){
    getMenuText(line1, menuSelected);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    if ( menuNumVal == 0 ){
      strcpy(line2, 0);
      strcat(line2, " ");
      strcat(line2, txtDisabled);
    } else {
      strcpy(line2, menuNumVal);
    }
    lcd.print(line2);
  }
  // == Zone or Schedule # Chooser Display ==
  if (bitRead(menuBits[menuLevel],menuBitIsNumList)){
    getMenuText(line1, menuSelected);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print("> ");
    lcd.print(menuNumVal);
  }

}


// get text for a menu item
void getMenuText(char *dest, menuItems mId){
  switch (mId) {
    case menuActions:
      strcpy(dest, txtActions);
      break;
    case menuSettings:
      strcpy(dest, txtSettings);
      break;
    case menuZones:
      strcpy(dest, txtZones);
      break;
    case menuZone:
      strcpy(dest, txtZone);
      break;
    case menuSchedules:
      strcpy(dest, txtSchedule);
      break;
    case menuSchedule:
      strcpy(dest, txtSchedule);
      break;
    case menuEnable:
      strcpy(dest, txtEnable);
      strcat(dest, " ");
      strcat(dest, txtAll);
      break;
    case menuRunAllZones:
      strcpy(dest, txtRun);
      strcat(dest, " ");
      strcat(dest, txtAll);
      strcat(dest, " ");
      strcat(dest, txtZones);
      break;
    case menuRunSomeZones:
      strcpy(dest, txtRun);
      strcat(dest, " ");
      strcat(dest, txtSome);
      strcat(dest, " ");
      strcat(dest, txtZones);
      break;
    case menuBlowOutZones:
      strcpy(dest, txtBlowout);
      strcat(dest, " ");
      strcat(dest, txtZones);
      break;
    case menuNumberOfZones:
      strcpy(dest, txtNumberOf);
      strcat(dest, " ");
      strcat(dest, txtZones);
      break;
    case menuMasterPin:
      strcpy(dest, txtMaster);
      strcat(dest, " ");
      strcat(dest, txtPIN);
      break;
    case menuMasterNormallyOpen:
      strcpy(dest, txtMaster);
      strcat(dest, " ");
      strcat(dest, txtNOpen);
      break;
    case menuZoneNormallyOpen:
      strcpy(dest, txtZone);
      strcat(dest, " ");
      strcat(dest, txtNOpen);
      break;
    case menuRainPin:
      strcpy(dest, txtRain);
      strcat(dest, " ");
      strcat(dest, txtPIN);
      break;
    case menuRainNormallyOpen:
      strcpy(dest, txtRain);
      strcat(dest, " ");
      strcat(dest, txtNOpen);
      break;
    case menuRainID:
      strcpy(dest, txtRain);
      strcat(dest, " ");
      strcat(dest, txtID);
      break;
    case menuTempID:
      strcpy(dest, txtTemp);
      strcat(dest, " ");
      strcat(dest, txtID);
      break;
    case menuWindID:
      strcpy(dest, txtWind);
      strcat(dest, " ");
      strcat(dest, txtID);
      break;
    case menuWeatherID:
      strcpy(dest, txtWeather);
      strcat(dest, " ");
      strcat(dest, txtID);
      break;
    case menuBlowOutWait:
      strcpy(dest, txtBlowout);
      strcat(dest, " ");
      strcat(dest, txtWait);
      break;
    case menuSetTime:
      strcpy(dest, txtSet);
      strcat(dest, " ");
      strcat(dest, txtTime);
      break;
    case menuDefaultReset:
      strcpy(dest, txtDefaultReset);
      break;
    case menuZoneEnable:
      strcpy(dest, txtZone);
      strcat(dest, " ");
      strcat(dest, txtEnable);
      break;
    case menuZoneName:
      strcpy(dest, txtZone);
      strcat(dest, " ");
      strcat(dest, txtName);
      break;
    case menuZoneRunTime:
      strcpy(dest, txtZone);
      strcat(dest, " ");
      strcat(dest, txtRun);
      strcat(dest, " ");
      strcat(dest, txtTime);
      break;
    case menuZonePin:
      strcpy(dest, txtZone);
      strcat(dest, " ");
      strcat(dest, txtPIN);
      break;
    case menuMoistureID:
      strcpy(dest, txtMoisture);
      strcat(dest, " ");
      strcat(dest, txtID);
      break;
    case menuDryLevel:
      strcpy(dest, txtDry);
      strcat(dest, " ");
      strcat(dest, txtLevel);
      break;
    case menuUseForecast:
      strcpy(dest, txtUseForecast);
      break;
    case menuAvoidWind:
      strcpy(dest, txtAvoid);
      strcat(dest, " ");
      strcat(dest, txtWind);
      break;
    case menuAvoidFreeze:
      strcpy(dest, txtAvoid);
      strcat(dest, " ");
      strcat(dest, txtFreeze);
      break;
    case menuBlowOutTime:
      strcpy(dest, txtBlowout);
      strcat(dest, " ");
      strcat(dest, txtTime);
      break;
    case menuBlowOutCycles:
      strcpy(dest, txtBlowout);
      strcat(dest, " ");
      strcat(dest, txtCycles);
      break;
    case menuMiniCycles:
      strcpy(dest, txtMini);
      strcat(dest, " ");
      strcat(dest, txtCycles);
      break;
    case menuScheduleEnable:
      strcpy(dest, txtSchedule);
      strcat(dest, " ");
      strcat(dest, txtEnable);
      break;
    case menuScheduleDay:
      strcpy(dest, txtDay);
      break;
    case menuScheduleZones:
      strcpy(dest, txtZones);
      break;
    case menuScheduleStartTime:
      strcpy(dest, txtStart);
      strcat(dest, " ");
      strcat(dest, txtTime);
      strcat(dest, "1");
      break;
    case menuScheduleRepeatDelay:
      strcpy(dest, txtRepeat);
      strcat(dest, " ");
      strcat(dest, txtDelay);
      break;
      
  }
  dest[0] = toupper(dest[0]);
}


bool getBitVal(menuItems mId){
  switch (mId){
    case menuEnable:
      return is_enabled();
      break;
    case menuMasterNormallyOpen:
      return master_valve_normally_open();
      break;
    case menuZoneNormallyOpen:
      return zone_normally_open();
      break;
    case menuRainNormallyOpen:
      return rain_normally_open();
      break;
    case menuDefaultReset:
      return 0;
      break;
    case menuZoneEnable:
      return zone_is_enabled();
      break;
    case menuUseForecast:
      return zone_use_rain();
      break;
    case menuAvoidWind:
      return zone_use_wind();
      break;
    case menuAvoidFreeze:
      return zone_use_temp();
      break;
    case menuMiniCycles:
      return zone_use_mini_cycle();
      break;
    case menuScheduleEnable:
      return schedule_is_enabled();
      break;
  }
}


void setBitVal(menuItems mId){
  switch (mId){
    case menuEnable:
      return set_is_enabled(menuBoolVal);
      break;
    case menuMasterNormallyOpen:
      return set_master_valve_normally_open(menuBoolVal);
      break;
    case menuZoneNormallyOpen:
      return set_zone_normally_open(menuBoolVal);
      break;
    case menuRainNormallyOpen:
      return set_rain_normally_open(menuBoolVal);
      break;
    case menuDefaultReset:
      if (menuBoolVal == 1){
        defaultReset();
        menuBoolVal = 0;
      }
      return 0;
      break;
      /*
    case menuZoneEnable:
      return set_zone_is_enabled(menuBoolVal);
      break;
    case menuUseForecast:
      return set_zone_use_rain(menuBoolVal);
      break;
    case menuAvoidWind:
      return set_zone_use_wind(menuBoolVal);
      break;
    case menuAvoidFreeze:
      return set_zone_use_temp(menuBoolVal);
      break;
    case menuMiniCycles:
      return set_zone_use_mini_cycle(menuBoolVal);
      break;
    case menuScheduleEnable:
      return set_schedule_is_enabled(menuBoolVal);
      break;
      */
  }
}


uint16_t getNumVal(menuItems mId){
  switch (mId){
    case menuNumberOfZones:
      return _num_zones;
      break;
    case menuMasterPin:
      return _master_valve_pin;
      break;
    case menuRainPin:
      return _rain_sensor_pin;
      break;
    case menuRainID:
      return _rain_id;
      break;
    case menuTempID:
      return _temperature_id;
      break;
    case menuWindID:
      return _wind_id;
      break;
    case menuWeatherID:
      return 0;  //TODO work out weather forcast interface
      break;
    case menuBlowOutWait:
      return _blowout_recharge_wait;
      break;
    case menuZoneRunTime:
      return _zone_run_time;
      break;
    case menuZonePin:
      return _zone_pin;
      break;
    case menuMoistureID:
      return _zone_moisture_id;
      break;
    case menuDryLevel:
      return _zone_is_dry_value;
      break;
    case menuBlowOutTime:
      return _zone_blowout_time;
      break;
    case menuBlowOutCycles:
      return _zone_blow_cycles;
      break;
  }
}


void setNumVal(menuItems mId){
  switch (mId){
    case menuNumberOfZones:
      return set_number_of_zones(menuNumVal);
      break;
    case menuMasterPin:
      return set_master_pin(menuNumVal);
      break;
    case menuRainPin:
      return set_rain_pin(menuNumVal);
      break;
    case menuRainID:
      return set_rain_id(menuNumVal);
      break;
    case menuTempID:
      return set_temp_id(menuNumVal);
      break;
    case menuWindID:
      return set_wind_id(menuNumVal);
      break;
    case menuWeatherID:
      return set_weather_id(menuNumVal);
      break;
    case menuBlowOutWait:
      return set_blowout_wait(menuNumVal);
      break;
    case menuZoneRunTime:
      return set_zone_run_time(menuNumVal);
      break;
    case menuZonePin:
      return set_zone_pin(menuNumVal);
      break;
    case menuMoistureID:
      return set_moisture_id(menuNumVal);
      break;
    case menuDryLevel:
      return set_dry_level(menuNumVal);
      break;
    case menuBlowOutTime:
      return set_blowout_time(menuNumVal);
      break;
    case menuBlowOutCycles:
      return set_blowout_cycles(menuNumVal);
      break;
  }
}


void backlightOff(){
  lcd.noBacklight();
  lcd.backlight();
}

