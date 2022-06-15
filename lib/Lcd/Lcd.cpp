#include "Lcd.h"

void Lcd::init() {
    // Inicializar el LCD
    this->lcd.init();
    this->lcd.clear();
    this->lcd.setBacklight(0);
    Lcd::print("Init display...", Lcd::Row::FIRST);
}

void Lcd::print(const char* msg, Lcd::Row row) {
    char myBuffer[24] = "";
    memset(myBuffer, ' ', 24);
    memcpy(myBuffer, msg, strlen(msg));
    this->lcd.setCursor(0, (int)row);
    this->lcd.print(myBuffer);
}

void Lcd::clear() { this->lcd.clear(); }

void Lcd::print_car_info(int collition, int mode, UltrasoundData& usData, CarInfoData& carData) {
    char buffer[24] = "";
    sprintf(buffer, "M:%d C:%d S:%d", mode, collition, carData.status);
    Lcd::print(buffer, Row::FIRST);

    char buffer2[24] = "";
    sprintf(buffer2, "%3.3d.%3.3d.%3.3d", usData.front_left, usData.front_middle, usData.front_right);
    Lcd::print(buffer2, Row::SECOND);
}