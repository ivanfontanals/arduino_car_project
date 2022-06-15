#include "Receiver.h"

void Receiver::init() { ibusRc.begin(ibusRcSerial); }

ReceiverData Receiver::read() {
    ReceiverData data;
    data.throttle = 0;
    data.direction = 0;
    int carReady = Receiver::readChannel(RECEIVER_STATUS_CHANNEL);
    data.enabled = carReady > 0;
    if (carReady > 0) {
        data.throttle = Receiver::readChannel(RECEIVER_FORWARD_CHANNEL);
        data.direction = Receiver::readChannel(RECEIVER_DIRECTION_CHANNEL);
        int mode = Receiver::readChannel(RECEIVER_MODE_CHANNEL);
        if (mode < 0)
            data.mode = Mode::ONE;
        else if (mode == 0)
            data.mode = Mode::TWO;
        else if (mode > 0)
            data.mode = Mode::FAILSAFE;
    }

    return data;
}

int Receiver::readChannel(byte channelInput) { return Receiver::readChannel(channelInput, MOTOR_MIN_PWM, MOTOR_MAX_PWM, 0); }

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int Receiver::readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
    uint16_t ch = ibusRc.readChannel(channelInput);
    if (ch < 1010) return minLimit;
    if (ch > 1990) return maxLimit;
    if (ch > 1490 && ch < 1510) return defaultValue;
    return map(ch, 1000, 2000, minLimit, maxLimit);
}

void Receiver::debug_channel(byte channelInput) {
    int value = Receiver::readChannel(channelInput, MOTOR_MIN_PWM, MOTOR_MAX_PWM, 0);
    debugSerial.print("Ch");
    debugSerial.print(channelInput);
    debugSerial.print(": ");
    debugSerial.print(value);
    debugSerial.print(", ");
}

void Receiver::debug() {
    for (int i = 0; i < 6; i++) Receiver::debug_channel(i);
    debugSerial.println("");
}