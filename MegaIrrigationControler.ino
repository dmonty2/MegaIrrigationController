/*
 * Description: Irrigation Controller with MySensors integration
 * Author: Dean Montgomery
 * Date: March 19, 2018
 */

//#define MY_DEBUG
#define MY_RF24_IRQ_PIN 3
#define MY_RADIO_NRF24
#include <MySensors.h>
#include "MegaIrrigation.h"


//
#define NUM_ZONES 5


// MySensors
MyMessage msg_ALL (0,0);  // initate and re-use to save memory.
#define LONG_WAIT 750                    // long wait between signals
#define SHORT_WAIT 50                    // short wait between signals
unsigned long currentMillis = millis(); // define here so it does not redefine in the loop.
unsigned long previousMillis = 0;
unsigned long previousDebounce = 0;
unsigned long previousOffMillis = 0; // countdown power off timer
irrigation Irrigation(NUM_ZONES);

void presentation()
{
  sendSketchInfo("Irrigation", "1.0");
  wait(LONG_WAIT);
  
  // Present each irrigation zone.

  //present(1, S_LIGHT,"Zone1", true);
  //wait(LONG_WAIT);
}


void setup(){
  
}

void loop(){

}