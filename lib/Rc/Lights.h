#ifndef LIGHTS
#define LIGHTS

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#include <SPI.h>
#endif

#define LED_US_LEFT 12
#define LED_US_CENTER 11
#define LED_US_RIGHT 13
#define LED_REAR_DIRECTION 10

class Lights {
   public:
    Lights() {}
    void init();
    void handle_ultrasensor_lights(bool left, bool middle, bool right);
    void handle_brake_lights(bool on);
    void handle_rear_lights(bool on);

   private:
    bool us_left = false;
    bool us_middle = false;
    bool us_right = false;
    bool rear = false;
};

#endif  //