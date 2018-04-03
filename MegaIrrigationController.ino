/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * Date: March 19, 2018
 */

//#define NUM_ZONES 5

#define IRRIGATION_VERSION 1

// ======== EEPROM =========
// EEPROM simplify storing retrieving various data types. 
#include <EEPROMex.h>
#define EEPROM_VERSION 1  // track major changes to eeprom
// Start of eeprom is stored in EEPROM_LOCAL_CONFIG_ADDRESS
// "~/Arduino/libraries/MySensors/core/MyEepromAddresses.h"
// eeprom arangement will be offset by EEPROM_LOCAL_CONFIG_ADDRESS.
#define EEPROM_VERSION 0                               // uint8_t    1
#define IRR_STORE_BITS (EEPROM_VERSION + 1)            // uint8_t    2
#define IRR_NUM_ZONES (IRR_STORE_BITS + 1)             // uint8_t    3
#define IRR_MASTER_VALVE_PIN (IRR_NUM_ZONES + 1)       // uint8_t    4
#define IRR_RAIN_SESNOR_PIN (IRR_MASTER_VALVE_PIN + 1) // uint8_t    5
#define IRR_WIND_ID (IRR_RAIN_SESNOR_PIN + 1)          // uint16_t   7
#define IRR_RAIN_ID (IRR_WIND_ID + 2)                  // uint16_t   9
#define IRR_TEMPERATURE_ID (IRR_RAIN_ID + 2)           // uint16_t  11
#define IRR_BLOWOUT_CHARGE_WAIT (IRR_TEMPERATURE_ID + 2)//uint16_t  13
// With spare (7 bytes).
#define IRR_EEPROM_BYTES  20 // EEPROM Bytes needed for IRR_*

#define ZONE_NUM 0                                  //  1 uint8_t
#define ZONE_STORE_BITS (ZONE_NUM + 1)              //  2 uint8_t
#define ZONE_PIN (ZONE_STORE_BITS + 1)              //  3 uint8_t
#define ZONE_NAME (ZONE_PIN + 1)                    // 18 char[15] ZONE_NAME_SIZE
#define ZONE_RUNTIME (ZONE_NAME + ZONE_NAME_SIZE)   // 19 uint8_t (store in mintues)
#define ZONE_BLOWOUT_TIME (ZONE_RUNTIME + 2)        // 21 uint16_t (store in seconds)
#define ZONE_BLOWOUT_CYCLES (ZONE_BLOWOUT_TIME + 2) // 22 uint8_t
#define ZONE_IS_DRY_VALUE (ZONE_BLOWOUT_CYCLES + 1) // 24 uint16_t
#define ZONE_MOISTURE_ID (ZONE_IS_DRY_VALUE + 2)    // 26 uint16_t
// With spare (4 bytes)
#define ZONE_EEPROM_BYTES 30 // EEPROM Bytes needed for each zone;

#define SCHEDULE_NUM 0                              //  1 uint8_t
#define SCHEDULE_STORE_BITS ( SCHEDULE_NUM + 1 )    //  3 uint16_t
#define SCHEDULE_ZONES ( SCHEDULE_STORE_BITS + ? )  //  5 uint16_t 16 zones.
#define SCHEDULE_START1 ( SCHEDULE_ZONES + ? )      //  start time e.g. 6AM
#define SCHEDULE_START2 ( SCHEDULE_START1 + ? )     //  start time 2 e.g. 6PM
#define SCHEDULE_REPEAT_DELAY ( SCHEDULE_START2 + ? ) // delay before repeating 0.


// Map Store bits for Settings _storebits
#define SETTING_BIT_IN_EEPROM 0      // Set to 1 once saved.
#define SETTING_BIT_ENABLE 1         // Set to 1 when system is on.
#define SETTING_BIT_MASTER_VALVE_NORMALLY_OPEN 2 // Set to 1 when normally open.
#define SETTING_BIT_ZONE_NORMALLY_OPEN 3 // Set to 1 when zones are normally open.
#define SETTING_BIT_RAIN_NORMALLY_OPEN 4 // Set to 1 when rain sensor is normally open.

// Map Store bits for Zones _zone_storebits
#define ZONE_BIT_IN_EEPROM    0 // Set to 1 once saved.
#define ZONE_BIT_ENABLED      1 // Zone is enabled.
#define ZONE_BIT_USE_MOISTURE 2 // Use Soil Moisture MySensor
#define ZONE_BIT_USE_RAIN     3 // Use Weather rain forecast.
#define ZONE_BIT_USE_WIND     4 // Use Wind sensor.
#define ZONE_BIT_USE_TEMP     5 // Use Temp sensor avoid freezing.
#define ZONE_BIT_MINI_CYCLE   6 // prevent runoff on steep slopes.

// Map Store bits for Schedule
#define SCHEDULE_BIT_ENABLED  0 // Set to 1 to enable
#define SCHEDULE_BIT_ANY_DAY  1 // Water any/every day
#define SCHEDULE_BIT_SUN      2
#define SCHEDULE_BIT_MON      3
#define SCHEDULE_BIT_TUE      4
#define SCHEDULE_BIT_WED      5
#define SCHEDULE_BIT_THU      6
#define SCHEDULE_BIT_FRI      7
#define SCHEDULE_BIT_SAT      8
#define SCHEDULE_BIT_EVEN     9 // Water even days
#define SCHEDULE_BIT_ODD     10 // Water odd days
#define SCHEDULE_BIT_NTH_DAY 11

// ============= LCD =============
// LCD library supports broken backlight bug.
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>
const int rs=8, en=9, db4=4, db5=5, db6=6, db7=7, bl=10, blLevel=HIGH;
hd44780_pinIO lcd(rs, en, db4, db5, db6, db7, bl, blLevel);
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


// ========== MySensors ==========
// MySensors setup
#define MY_DEBUG
#define MY_RF24_CE_PIN 49
#define MY_RF24_CS_PIN 53
//#define MY_RF24_IRQ_PIN 3
#define MY_RADIO_NRF24
#define MY_TRANSPORT_WAIT_READY_MS 3000 
#include <MySensors.h>
MyMessage msg_ALL (0,0);  // initate and re-use to save memory.
#define LONG_WAIT 750                    // long wait between signals
#define SHORT_WAIT 50                    // short wait between signals


// ======= Program variables =======
#define ZONE_NAME_SIZE 15 // Zone name size 14+NUL
unsigned long _currentMillis = millis(); // define here so it does not redefine in the loop.
unsigned long _previousMillis = 0;
unsigned long _previousDebounce = 0;
unsigned long _previousOffMillis = 0; // countdown power off timer



uint8_t  _version = IRRIGATION_VERSION;
// Settings saved in EEPROM
uint8_t  _eeprom_version = EEPROM_VERSION;
uint8_t  _storebits = 0;         // store bits
uint8_t  _num_zones = 0;         // Number of zones.
uint8_t  _master_valve_pin = 0;  // Pin for master valve
uint8_t  _rain_sensor_pin = 0;   // Pin for rain sensor.
uint16_t _wind_id = 0;           // MySensor ID for wind.
uint16_t _rain_id = 0;           // MySensor ID for rain.
uint16_t _temperature_id = 0;    // MySensor ID for temperature.
uint16_t _blowout_recharge_wait = 0; // Compressor recharge/rest wait time.
// Settings in RAM
uint8_t  _is_enabled = 0;        // Master ON OFF/rain
uint8_t  _is_running = 0;        // System is running.
//uint8_t  _is_enabled = 0;        // Master ON OFF/rain
int      _eeprom_start_addr = 0; // EEPROM start address.

// Zone saved in EEPROM
uint8_t  _zone_number = 0;       //  1 Zone Number
uint8_t  _zone_storebits = 0;    //  1 used to store on/off bits
uint8_t  _zone_pin = 0;          //  1 Arduino Pin number
char     _zone_name[ZONE_NAME_SIZE];   // 15 Zone Name 14char
uint16_t _zone_run_time = 0;     //  4 Zone Run Time (in minutes)
uint16_t _zone_blowout_time = 0; //  4 Zone Run Time for compressed air blow out. (in seconds)
uint8_t  _zone_blow_cycles = 0;  //  4 Number of cycles for compressor air blow out. 
uint16_t _zone_is_dry_value = 0; //  2 Value at which the zone is considered dry.
uint16_t _zone_moisture_id = 0;  //  2 MySensors ID for moisture sensor.
// Zone in RAM
uint8_t  _zone_is_on = 0;              // Zone is running
uint16_t _zone_previous_moisture = 0;  // Previous 
uint16_t _zone_current_moisture = 0;   // Current Mosture sensor value
int      _zone_eeprom_offset = 0;      // EEPROM address offset for this zone.



void presentation()
{
  sendSketchInfo("Irrigation", "1.0");
  wait(LONG_WAIT);
  
  // Present each irrigation zone.

  //present(1, S_LIGHT,"Zone1", true);
  //wait(LONG_WAIT);
}

void setup(){
  init_irrigation();
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();
  lcd.print("Mega Irrigation");
  lcd.setCursor(3,1);
  lcd.print("Controller");
}

void loop(){
  _currentMillis = millis();
  checkButtonPress();   // Menu interaction.
  if (_is_enabled == 1){
    checkZoneTimer();   // Countdown and track Zones.
    checkSensors();     // Check moisture, rain, wind, temp
    checkSchedule();    // Start Time for water cycle.
  }
}


// ====== Actions =======

uint8_t checkButtonPress(){
  int adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 250)  return btnUP; 
  if (adc_key_in < 450)  return btnDOWN; 
  if (adc_key_in < 650)  return btnLEFT; 
  if (adc_key_in < 850)  return btnSELECT;
  return btnNONE;
}

void checkZoneTimer(){
  if (_is_running == 1){
    // put single, multiple, or all zones in an array and cycle through each
    // for (uint8_t i = 0; i <= num_zones; i++ ){
    //}
  }
}

// Check sensors every hour.
void checkSensors(){
  
}

// Check schedule for when to start zones.
void checkSchedule(){
  if (_is_running == 0){

  }
}

void run_all_zones(void){
  for (uint8_t zone_num = 0; zone_num <= _num_zones; zone_num++ ){
    loadZoneConfig(zone_num);
  }

}

void run_some_zones(void){

}

void run_one_zone(uint8_t zone){

}

void stop(void){

}

void blowout_zones(void){

}

void backlightOff(){
  lcd.noBacklight();
  lcd.backlight();
}

void init_irrigation(){
  int eeprom_start = EEPROM_LOCAL_CONFIG_ADDRESS + 255;
  EEPROM.setMemPool(eeprom_start, EEPROMSizeMega);
  _eeprom_start_addr = eeprom_start;
  loadSettingsConfig();
}

// ===== Settings =====

void loadSettingsConfig(){
  _eeprom_version = EEPROM.readByte(EEPROM_VERSION + _eeprom_start_addr);
  _storebits = EEPROM.readByte(IRR_STORE_BITS + _eeprom_start_addr);
  _is_enabled = is_enabled();
  _num_zones = EEPROM.readByte(IRR_NUM_ZONES + _eeprom_start_addr);
  if (_num_zones == 0){
    _num_zones = 5;
  }
  _master_valve_pin = EEPROM.readByte(IRR_MASTER_VALVE_PIN + _eeprom_start_addr);
  _rain_sensor_pin = EEPROM.readByte(IRR_RAIN_SESNOR_PIN + _eeprom_start_addr);
  _wind_id = EEPROM.readInt(IRR_WIND_ID + _eeprom_start_addr);
  _rain_id = EEPROM.readInt(IRR_RAIN_ID + _eeprom_start_addr);
  _temperature_id = EEPROM.readInt(IRR_TEMPERATURE_ID + _eeprom_start_addr);
  _blowout_recharge_wait = EEPROM.readInt(IRR_BLOWOUT_CHARGE_WAIT + _eeprom_start_addr);
}

bool in_eeprom(){
  return bitRead(_storebits,SETTING_BIT_IN_EEPROM);
}

bool is_enabled(){
  return bitRead(_storebits,SETTING_BIT_ENABLE);
}

bool master_valve_normally_open(){
  return bitRead(_storebits,SETTING_BIT_MASTER_VALVE_NORMALLY_OPEN);
}

bool zone_normally_open(){
  return bitRead(_storebits,SETTING_BIT_ZONE_NORMALLY_OPEN);
}

bool rain_normally_open(){
  return bitRead(_storebits,SETTING_BIT_RAIN_NORMALLY_OPEN);
}


// ===== Zones ======

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




