#ifndef DATALOGGER
#define DATALOGGER

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#include <SPI.h>
#endif

#include "SdFat.h"

#define BUFFER_SIZE 512

struct CarDataRecord {
    unsigned long millis;//4

    // UltraSound sensors
    unsigned short us_left;//2
    unsigned short us_center;//2
    unsigned short us_right;//2

    // Motors PWM value
    unsigned char pwmleft;//1
    unsigned char pwmright;//1

    // NO_DANGER = 0, WARNING = 1, INMINENT = 2
    char collision = -1;//1

    // MOVING_BD = 1, MOVING_FD = 2, BRAKING = 3, STOPPED = 4
    char status = -1;//1

    // FORWARD = 1, BACKWARD = 0
    char direction = -1;//1
};

class DataLogger {
   public:
    DataLogger() {}

    /*
     * Initialize the SD card and creates the global log file. This method must
     * be call inside the setup method, after Serial initalization
     */
    void init();
    void log(CarDataRecord& record);
    void read(char* filename);
    void convert_to_csv(char* source, char* destination);
    void flush();

   private:
    uint16_t buffer_size = 0;
    long last_flush_time = 0;

    // File system object.
    SdFat sd;
    // Log file.
    SdFile file;

    void init_sd_card();
    char* get_filename();
    void writeHeader();
};

#endif