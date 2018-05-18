/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * Date: April 13, 2018
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/*
 * TODO:
 * 
 */

#define IRRIGATION_VERSION 1
// ======== EEPROM =========
// EEPROM simplify storing retrieving various data types. 
#include <EEPROMex.h>
#define EEPROM_VERSION 2  // track major changes to eeprom
// EEPROM defaults are usually 0 or 255, so we can detect new hardware.

#define ZONE_NAME_SIZE 15 // Zone name size 14+NUL

// Start of eeprom is stored in EEPROM_LOCAL_CONFIG_ADDRESS
// "~/Arduino/libraries/MySensors/core/MyEepromAddresses.h"
// eeprom arangement will be offset by EEPROM_LOCAL_CONFIG_ADDRESS.
// Settings EEPROM Map
#define IRR_EEPROM_VERSION 0                           // uint8_t    1
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

// Schedule EEPROM Map
// Fixed number of schedules so the rest of the eeprom can be used for zones.
#define NUMBER_OF_SCHEDULES 4                                //  Changing this requires resetting eeprom as zones eprom saves follow after.
#define SCHEDULE_STORE_BITS 0                                //  2 uint16_t
#define SCHEDULE_START_TIME ( SCHEDULE_STORE_BITS + 2 )      //  4 uint16_t start time e.g. 6AM (minutes since midnight)
#define SCHEDULE_REPEAT_DELAY ( SCHEDULE_START_TIME + 2 )    //  8 uint16_t delay before repeating 0.
#define SCHEDULE_EVERY_NTH_DAY ( SCHEDULE_REPEAT_DELAY + 2 ) //  9 unit8_t every nth day
#define SCHEDULE_ZONES ( SCHEDULE_EVERY_NTH_DAY + 1 )        //  13 uint32_t 32 zones.
#define SCHEDULE_EEPROM_BYTES 16 // EEPROM Bytes needed for each schedule with 4 spare bites for growth

// Zone EEPROM Map
#define ZONE_STORE_BITS 0                           //  1 uint8_t
#define ZONE_PIN (ZONE_STORE_BITS + 1)              //  2 uint8_t
#define ZONE_NAME (ZONE_PIN + 1)                    // 17 char[15] ZONE_NAME_SIZE
#define ZONE_RUNTIME (ZONE_NAME + ZONE_NAME_SIZE)   // 18 uint8_t (store in mintues)
#define ZONE_BLOWOUT_TIME (ZONE_RUNTIME + 2)        // 20 uint16_t (store in seconds)
#define ZONE_BLOWOUT_CYCLES (ZONE_BLOWOUT_TIME + 2) // 21 uint8_t
#define ZONE_IS_DRY_VALUE (ZONE_BLOWOUT_CYCLES + 1) // 23 uint16_t
#define ZONE_MOISTURE_ID (ZONE_IS_DRY_VALUE + 2)    // 25 uint16_t
// With spare (5 bytes)
#define ZONE_EEPROM_BYTES 30 // EEPROM Bytes needed for each zone;


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
#define SCHEDULE_BIT_SUN      1 // First day of week bit 1 is also 1 : )
#define SCHEDULE_BIT_MON      2
#define SCHEDULE_BIT_TUE      3
#define SCHEDULE_BIT_WED      4
#define SCHEDULE_BIT_THU      5
#define SCHEDULE_BIT_FRI      6
#define SCHEDULE_BIT_SAT      7
#define SCHEDULE_BIT_ANY_DAY  8 // Water any/every day
#define SCHEDULE_BIT_EVEN     9 // Water even days
#define SCHEDULE_BIT_ODD     10 // Water odd days
#define SCHEDULE_BIT_NTH_DAY 11

// ============= LCD & keypad =============
// LCD library supports broken backlight bug.
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>
const int rs=8, en=9, db4=4, db5=5, db6=6, db7=7, bl=10, blLevel=HIGH;
hd44780_pinIO lcd(rs, en, db4, db5, db6, db7, bl, blLevel);
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
enum buttonItems {
  btnNone,
  btnRight,
  btnUp,
  btnDown,
  btnLeft,
  btnSelect
};

//buttonItems btnLast = btnNone;
buttonItems btnCurrent = btnNone;

enum menuItems {
  menuRoot,
    menuActions,
    menuSettings,
    menuZones,
    menuSchedules,
      menuEnable,            //Actions
      menuRunSchedule,
      menuRunSomeZones,
      menuBlowoutZones,
      menuNumberOfZones,     //Settings
      menuMasterPin,
      menuMasterNormallyOpen,
      menuZoneNormallyOpen,
      menuRainPin,
      menuRainNormallyOpen,
      menuRainID,
      menuTempID,
      menuWindID,
      menuWeatherID,
      menuBlowoutWait,
      menuSetTime,
      menuDefaultReset,
      menuZone,             // Zone number chooser
      menuZoneEnable,       // Zones details...
      menuZoneName,
      menuZoneRunTime,
      menuZonePin,
      menuMoistureID,
      menuDryLevel,
      menuUseForecast,
      menuAvoidWind,
      menuAvoidFreeze,
      menuBlowoutTime,
      menuBlowoutCycles,
      menuMiniCycles,
      menuSchedule,         // Schedule number chooser
      menuScheduleEnable,   //Schedule details...
      menuScheduleDay,
      menuScheduleZones,
      menuScheduleStartTime,
      menuScheduleRepeatDelay,
      menuLast
};
// Last menuItem in above enum is used to set settings array length.
uint8_t menuBits[menuLast];
enum menuItemBits {
  menuBitIsBranch,    // Branch
  menuBitInputYesNo,  // Leaf Yes/No
  menuBitInputNumber, // Leaf Number
  menuBitInputText,   // Leaf Text
  menuBitInputTime,   // Leaf Time
  menuBitIsNumList,   // Branch List of Zones or Schedules 1,2,3,4...n
  menuBitIsAction     // Immediate action - stop, run all, blow out all
};

uint8_t  menuBoolVal = 0;
uint16_t menuNumVal = 0;
char menuAsciiVal = 0;    // Track current char
int menuAsciiPos = 0;    // Posision in string.
char menuTextVal[ZONE_NAME_SIZE];

menuItems menuLevel = menuRoot;
menuItems menuSelected = menuActions;
menuItems menuStart = menuEnable;
menuItems menuEnd   = menuEnd;
uint8_t menuParentTree[4] = { 0, 0, 0, 0 };  // Used to track menu sub-levels







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

// ======= Time and Alarms =======
// TODO: https://github.com/mysensors/MySensorsArduinoExamples/blob/master/examples/TimeAwareSensor/TimeAwareSensor.ino
#include <TimeLib.h>
#include <TimeAlarms.h>


// ======= Program variables =======
#define BUTTON_DEBOUNCE 100
#define BUTTON_AUTO_REPEAT_SLOW_RATE 400
#define BUTTON_AUTO_REPEAT_FAST_RATE 100 
#define BUTTON_AUTO_REPEAT_FAST_DELAY 3000 // 3s delay before stepping up auto repeat rate
unsigned long _currentMillis = millis(); // define here so it does not redefine in the loop.
unsigned long _previousMillis = 0;
unsigned long _previousDebounceMillis = 0;
unsigned long _autoRepeatMillis = 0;
unsigned long _autoRepeatRate = BUTTON_AUTO_REPEAT_SLOW_RATE;
unsigned long _autoRepeatStartMillis = 0;
unsigned long _previousOffMillis = 0; // countdown power off timer



uint8_t  _version = IRRIGATION_VERSION;
// Settings saved in EEPROM
uint8_t  _eeprom_version = EEPROM_VERSION;
bool     _is_bootup = 1;         // detect bootup.
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
uint8_t  _current_running_zone = 0;  // Water zone is running.
uint8_t  _current_running_scedule = 0;  // Schedule is running zones.
//uint8_t  _is_enabled = 0;        // Master ON OFF/rain
int      _eeprom_start_addr = 0; // EEPROM start address.

enum systemStates {
  stateOff,
  stateRunningSchedule,
  stateRunningAllZones,
  stateRunningSomeZones
};
systemStates _systemState = stateOff;  // Track system state.




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
//uint8_t  _zone_is_on = 0;            // Zone is running
unsigned long _zone_timer_start = 0;   // Timer for zone.
unsigned long _zone_timer_end = 0;     // Timer for zone.
uint16_t _zone_previous_moisture = 0;  // Previous 
uint16_t _zone_current_moisture = 0;   // Current Mosture sensor value
int      _zone_eeprom_offset = 0;      // EEPROM address offset for this zone.

// Schedule saved in EEPROM
uint8_t  _schedule_number = 0;        // 1 Schedule Number
uint16_t _schedule_storebits = 0;     // 1 used to store on/off bits
uint16_t _schedule_start_time = 0;    // start time
uint16_t _schedule_repeat_delay = 0;  // repeat delay 0 = is no repeats.
uint8_t  _schedule_every_nth_day = 0; // every nth day starting on 1st of month
uint32_t _schedule_zones = 0;         // bit toggle which zones to water on this schedule
int      _schedule_eeprom_offset = 0; // EEPROM address offset for this schedule

// Schedule in RAM
bool _timeReceived = false; // Tracking Time
unsigned long _lastTimeRequest=0;
// Schedule timers
struct schTracker {
  int curent_running_zone;
  unsigned long next_start;
};
schTracker scheduleTracker[NUMBER_OF_SCHEDULES + 1]; // extra schedule is for run_all_zones and run_some_zones


void presentation()
{
  sendSketchInfo("Irrigation", "1.0");
  wait(LONG_WAIT);
  
  // Present each irrigation zone.

  //present(1, S_LIGHT,"Zone1", true);
  //wait(LONG_WAIT);
}

void setup(){
  _is_bootup = 1;
  init_irrigation();
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();
  lcd.print("Mega Irrigation");
  lcd.setCursor(3,1);
  lcd.print("Controller");
  initializeMenu();
  requestTime();
  _is_bootup = 0;
}

void loop(){
  _currentMillis = millis();
  checkButtonPress();   // Menu interaction.
  if (_is_enabled == 1){
    checkZoneTimer();   // Countdown and track Zones.
    checkSensors();     // Check moisture, rain, wind, temp
    checkSchedule();    // Start Time for water cycle.
  }
  // If time not received then 10s requests, else request time every hour.
  if ((!_timeReceived && (_currentMillis - _lastTimeRequest) > (10UL*1000UL))
    || (_timeReceived && (_currentMillis - _lastTimeRequest) > (60UL*1000UL*60UL))) {
    requestTime();  
    _lastTimeRequest = _currentMillis;
  }
}

// receive time from Controller.
void receiveTime(unsigned long time) {
  setTime(time);
  _timeReceived = true;
}

// Check sensors every hour.
void checkSensors(){
  
}


void init_irrigation(){
  // Leave space for MySensors eeprom.
  int eeprom_start = EEPROM_LOCAL_CONFIG_ADDRESS + 255;  
  _eeprom_start_addr = eeprom_start;
  EEPROM.setMemPool(eeprom_start, EEPROMSizeMega);  // TODO this line may not be needed.
  uint8_t stored_version = EEPROM.readByte(IRR_EEPROM_VERSION + _eeprom_start_addr);
  if (stored_version == 0 || stored_version == 255){
    defaultReset();
  }
  // Load initial settings from EEPROM into memory.
  initSettingsConfig();
  initScheduleConfig();
  initZoneConfig();
}


