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

/// A watering zone
class Zone {
  // Store in EEPROM end-user sets
  public:
    uint8_t   z_number;       /// Zone Number
    uint8_t   z_pin;          // Arduino Pin number
    char      z_name[14];    // Zone Name
    long      z_run_time;     // Zone Run Time
    long      z_blowout_time; // Zone Run Time for compressed air blow out.
    long      z_blow_cycles;  // Number of cycles for compressor air blow out. 
    uint16_t  z_is_dry_value; // Value at which the zone is considered dry.
    uint8_t   z_storebits;    // used to store on/off bits
    uint16_t  z_moisture_id;  // MySensors ID for moisture sensor.
    uint16_t  z_wind_id;      // MySensor ID for wind.

    // Store in RAM
    uint16_t  z_previous_moisture;  // Previous 
    uint16_t  z_current_moisture;   // Current Mosture sensor value
    uint8_t   z_rambits;            // used store

    Zone (void);
    void water_on(void);
    void water_off(void);
    void wind_sensor_on(void);
    void wind_sensor_off(void);

    inline bool is_on() {
      return bitRead(z_rambits,IS_ON);
    }

    inline bool use_moisture(){
      return bitRead(z_storebits,USE_MOISTURE);
    }

    inline bool use_weather(){
      return bitRead(z_storebits,USE_WEATHER);
    }

};

typedef enum {
  USE_MOISTURE = 0,       // Use Moisture sensor
  USE_WEATHER = 1,        // Use Weather forecast
  USE_WIND = 2,           // Use Wind sensor
  
} store_bit_pos;

typedef enum {
  IS_ON = 0,              // zone is on and running
} ram_bit_pos;


class Irrigation {
  public:
    uint8_t i_is_on = 0;             // Master on, off/rain
    uint8_t i_num_zones = 0;         // Number of zones.
    uint8_t i_master_valve_pin = 0;  // Pin for master valve
    uint8_t i_rain_sensor_pin = 0;   // Pin for rain sensor.
    uint8_t i_storebits = 0; /// store bits
    Irrigation(void);
    void run_all_zones(void);
    void run_one_zone(uint8_t zn);
    void stop(void);

};

#endif