  /*
   To calculate ETA for watering:
  // (on a fixed interval of time)
  if ( current_moisture <= is_dry_moisture){
     water();
  } else {
     periods_till_water =  (current_moisture - is_dry_value)/(previous_moisture - current_reading)
     if (periods_till_water <= 1 && periods_till_water >= 0){
        water();
     }
  }
  */


#ifndef MegaIrrigation_h
#define MegaIrrigation_h

// Store bits
#define SB_USE_MOISTURE 0
#define SB_USE_WEATHER 1
#define SB_USE_WIND 2

// Ram bits
#define RB_IS_ON 0

/// A watering zone
class Zone {
  // Store in EEPROM end-user sets
  public:
    uint8_t   number;       //  1 Zone Number
    uint8_t   pin;          //  1 Arduino Pin number
    char      name[15] = "              ";  // 14 Zone Name 14char
    long      run_time;     // 4 Zone Run Time
    long      blowout_time; // 4 Zone Run Time for compressed air blow out.
    long      blow_cycles;  // 4 Number of cycles for compressor air blow out. 
    uint16_t  is_dry_value; // 2 Value at which the zone is considered dry.
    uint8_t   storebits;    // 1 used to store on/off bits

    uint16_t  moisture_id;  // 2 MySensors ID for moisture sensor.
    uint16_t  wind_id;      // 2 MySensor ID for wind.

    // Store in RAM
    uint16_t  previous_moisture;  // Previous 
    uint16_t  current_moisture;   // Current Mosture sensor value
    uint8_t   rambits;            // used store

    Zone (void);
    void readConfig(void);
    void updateConfig(void);
    void water_on(void);
    void water_off(void);
    void wind_sensor_on(void);
    void wind_sensor_off(void);

    inline bool is_on() {
      return bitRead(rambits,RB_IS_ON);
    }

    inline bool use_moisture(){
      return bitRead(storebits,SB_USE_MOISTURE);
    }

    inline bool use_weather(){
      return bitRead(storebits,SB_USE_WEATHER);
    }

};




class Irrigation {
  public:
    uint8_t is_on = 0;             // Master on, off/rain
    uint8_t num_zones = 0;         // Number of zones.
    uint8_t master_valve_pin = 0;  // Pin for master valve
    uint8_t rain_sensor_pin = 0;   // Pin for rain sensor.
    uint8_t storebits = 0; /// store bits
    Irrigation(uint8_t num_zones);
    void run_all_zones(void);
    void run_one_zone(uint8_t zn);
    void stop(void);
    void readConfig(void);
    void updateConfig(void);
};

#endif