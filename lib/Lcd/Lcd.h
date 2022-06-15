#ifndef LCD
#define LCD

#include <LiquidCrystal_I2C.h>

#include "Car.h"
#include "Ultrasound.h"

enum Row { FIRST = 0, SECOND = 1 };

class Lcd {
   private:
    // Creates the LCD object in the dirección  0x27, with  cols and 2 rows
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

   protected:
   public:
    enum Row { FIRST = 0, SECOND = 1 };
    Lcd() {}
    void init();
    void print(const char* msg, Lcd::Row row);

    void print_car_info(int collision, int mode, UltrasoundData& usData, CarInfoData& carData);
    void clear();
};

#endif  //