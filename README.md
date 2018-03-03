# MegaIrrigationController
Arduino Mega Irrigation Controller for MySensors

This is a rewrite of the MySensors IrrigationController for Arduino Mega.
Originally by Jim (BulldogLowell@gmail.com) and Pete (pete.will@mysensors.org)

This is in pre-alpha stage.

Idea is to simplify the circuit:
* Arduino Mega
* LCD 1602 shield with multi-button keypad
* Relays

Create a full featured irrigation controller that can run stand-alone and
receive weather, soil, rain input from MySensors in order to optimize watering cycles.

Ideas in order of importance:
* Include full screen menu to setup Zones and Schedules - not just hard-coded.
* Include logic to adjust water time based on:
 - soil moisture - moisture sensors at various soil depths ( wireless through MySensor mesh network )
 - weather / rain sensor.  Predicted rain should delay, reduce, or cancel a cycle.
 - wind sensor ( avoid wind blowing sprinklers onto driveway and rapid evaporation )
 - soil type: ( sand, loam, silt, clay etc )
 - sprinkler head type and water volume.
 - plant types ( deep or shallow watering, water coefficient e.g. Annual, Trees... Xeriscape )
 - ground slope.  ( i.e. multiple short cycles to prevent run-off. )
 - sun or shade.
 - based on the above variables adjust single or multiple cycles to optimize water needs and absorption as well as waste. Soil moisture sensors would be the most accurate at tracking needed water.
