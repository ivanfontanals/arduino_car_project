#ifndef RECEIVER
#define RECEIVER

#undef NULL /* in case <stdio.h> has defined it. */
#define NULL 0

#include "Constants.h"
#include "IBusBM.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#include <SPI.h>
#endif

#define RECEIVER_DIRECTION_CHANNEL 3  // Left joystick (Left/Right)
#define RECEIVER_FORWARD_CHANNEL 1    // Right joystick (Up/Down)

// ON/OFF
#define RECEIVER_STATUS_CHANNEL 4  // Top-Left switch (SWA) On/Off)

// 3 modes: ONE;TWO;THREE (SWC)
#define RECEIVER_MODE_CHANNEL 5

struct ReceiverData {
    bool enabled = false;
    // Motors PWM value
    uint16_t throttle;
    uint16_t direction;
    // Mode (ONE;TWO;THREE)
    int mode;
};

class Receiver {
   private:
    IBusBM ibusRc;
    HardwareSerial& ibusRcSerial = Serial1;
    HardwareSerial& debugSerial = Serial;
    int readChannel(byte channelInput);
    int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue);

   public:
    // FAILSAFE is the mode detected when we have a signal lost. We need to
    // upgrade the receiver to change this behavior
    enum Mode { ONE = 1, TWO = 2, FAILSAFE = 3 };
    Receiver() {}
    void init();
    ReceiverData read();
    void debug_channel(byte channelInput);
    void debug();
};
#endif