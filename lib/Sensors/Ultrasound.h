#ifndef ULTRASOUND
#define ULTRASOUND

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#include <SPI.h>
#endif

#include <Ultrasonic.h>

struct UltrasoundData {
    unsigned short front_right;
    unsigned short front_left;
    unsigned short front_middle;
};

class Ultrasound {
   private:
    Ultrasonic ultrasonicM = 0;
    Ultrasonic ultrasonicFR = 0;
    Ultrasonic ultrasonicFL = 0;

   public:
    Ultrasound() {}
    void init();
    UltrasoundData read();
};
#endif