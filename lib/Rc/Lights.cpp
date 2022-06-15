#include "Lights.h"

void Lights::init() {
    pinMode(LED_US_LEFT, OUTPUT);
    pinMode(LED_US_CENTER, OUTPUT);
    pinMode(LED_US_RIGHT, OUTPUT);
    pinMode(LED_REAR_DIRECTION, OUTPUT);
}

void Lights::handle_ultrasensor_lights(bool left, bool middle, bool right) {
    if (us_right != right) {
        us_right = right;
        digitalWrite(LED_US_RIGHT, us_right ? HIGH : LOW);
    }
    if (us_left != left) {
        us_left = left;
        digitalWrite(LED_US_LEFT, us_left ? HIGH : LOW);
    }
    if (us_middle != middle) {
        us_middle = middle;
        digitalWrite(LED_US_CENTER, us_middle ? HIGH : LOW);
    }
}

void Lights::handle_rear_lights(bool status) {
    if (rear != status) {
        rear = status;
        digitalWrite(LED_REAR_DIRECTION, status ? HIGH : LOW);
    }
}

void Lights::handle_brake_lights(bool status) {
    if (rear != status) {
        rear = status;
        digitalWrite(LED_REAR_DIRECTION, status ? HIGH : LOW);
    }
}
