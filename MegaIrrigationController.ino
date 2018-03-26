/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * Date: March 19, 2018
 */

#define NUM_ZONES 5

#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>
const int rs=8, en=9, db4=4, db5=5, db6=6, db7=7, bl=10, blLevel=HIGH;
hd44780_pinIO lcd(rs, en, db4, db5, db6, db7, bl, blLevel);
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

//Serial.begin(115200);
#define MY_DEBUG
#define MY_RF24_CE_PIN 49
#define MY_RF24_CS_PIN 53
//#define MY_RF24_IRQ_PIN 3
#define MY_RADIO_NRF24
#define MY_TRANSPORT_WAIT_READY_MS 3000
#include <MySensors.h>
#include "MegaIrrigation.h"
#include "MegaIrrigation.cpp"



// MySensors
MyMessage msg_ALL (0,0);  // initate and re-use to save memory.
#define LONG_WAIT 750                    // long wait between signals
#define SHORT_WAIT 50                    // short wait between signals
unsigned long currentMillis = millis(); // define here so it does not redefine in the loop.
unsigned long previousMillis = 0;
unsigned long previousDebounce = 0;
unsigned long previousOffMillis = 0; // countdown power off timer

// Innitiate with number of zones and EEPROM space just outside of MySensors EEPROM space.

void presentation()
{
  sendSketchInfo("Irrigation", "1.0");
  wait(LONG_WAIT);
  
  // Present each irrigation zone.

  //present(1, S_LIGHT,"Zone1", true);
  //wait(LONG_WAIT);
}

void setup(){
  
init_irrigation(NUM_ZONES, EEPROM_LOCAL_CONFIG_ADDRESS + 255);
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();
  lcd.print("Mega Irrigation");
  lcd.setCursor(3,1);
  lcd.print("Controller");
}

void loop(){

}

void backlightOff(){
  lcd.noBacklight();
  lcd.backlight();
}
