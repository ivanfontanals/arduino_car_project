#include "Ultrasound.h"

void Ultrasound::init() {
    this->ultrasonicM = Ultrasonic(2, 3);
    this->ultrasonicFR = Ultrasonic(8, 9);
    this->ultrasonicFL = Ultrasonic(6, 7);
}

UltrasoundData Ultrasound::read() {
    UltrasoundData data;
    int fr = ultrasonicFR.read();
    int fm = ultrasonicM.read();
    int fl = ultrasonicFL.read();
    data.front_right = fr>300?300:fr;
    data.front_middle = fm>300?300:fm;
    data.front_left = fl>300?300:fl;
    return data;
}