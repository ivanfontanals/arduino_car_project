#include <Arduino.h>
#include "Constants.h"
#include <Car.h>
#include <DataLogger.h>
#include <Lcd.h>
#include <Lights.h>
#include <Receiver.h>
#include <Ultrasound.h>

const int PERIOD_TO_PRINT_SERIAL = 1000;
const int PERIOD_TO_PRINT_LCD = 500;
const int PERIOD_ULTRASONIC = 200;
const int PERIOD_CAR_UPDATE = 100;
const int PERIOD_MAIN_LOOP = 10;
const int PERIOD_LOG_DATA = 300;
const int PERIOD_FLUSH_DATA = 10000;


const float REDUCE_SPEED_FACTOR = 0.5f;

const int MONITOR_SPEED = 19200;



/**
 * Global Datalogger *
 */
DataLogger dataLogger;
Lcd lcd;
Car car;
Receiver receiver;
Ultrasound ultrasound;
Lights lights;

unsigned long time_print_lcd = 0;
unsigned long time_print_serial = 0;
unsigned long time_ultrasonic_sensor = 0;
unsigned long time_car_update = 0;
unsigned long time_main_loop = 0;
unsigned long time_log_data = 0;
unsigned long time_flush_data = 0;

unsigned long counter = 0;
UltrasoundData usData;

CarDataRecord last_record_written;


enum CollisionMode { NO_DANGER = 0, WARNING = 1, INMINENT = 2 };

struct CollisionData {
    bool warningFrontCrash = false;
    bool warningSideCrash = false;
    bool inminentFrontCrash = false;
    bool inminentSideCrash = false;
    CollisionMode mode = CollisionMode::NO_DANGER;
} collisionData;

// cppcheck-suppress unusedFunction
void setup() {
    Serial.begin(MONITOR_SPEED);
    last_record_written.status = -1;

    
    // dataLogger.convert_to_csv("Data01.bin", "output2.csv");
    // dataLogger.read("Data01.bin");
    lcd.init();
    // SD init
    if (LOG_DATA) dataLogger.init();
    lcd.print("Initializing...", Lcd::Row::FIRST);
    receiver.init();
    ultrasound.init();
    lights.init();
    car.init();
    usData = ultrasound.read();
    lcd.print("Ok. Ready", Lcd::Row::FIRST);
    
}

void updateCollisionData(ReceiverData& receiverData, UltrasoundData& usData) {
    collisionData.warningFrontCrash = usData.front_middle <= WARNING_DISTANCE_FRONT;  // Distance to wall small
    collisionData.warningSideCrash = (usData.front_right <= WARNING_DISTANCE_SIDES ||
                            usData.front_left <= WARNING_DISTANCE_SIDES);  // Distance to wall small
    collisionData.inminentFrontCrash =  usData.front_middle <= STOP_DISTANCE_FRONT;   // Distance to wall small
    collisionData.inminentSideCrash = (usData.front_right <= STOP_DISTANCE_SIDES ||
                            usData.front_left <= STOP_DISTANCE_SIDES);  // Distance to wall small
    if (collisionData.inminentFrontCrash) {
        collisionData.mode = INMINENT;
    }
    else if (collisionData.warningFrontCrash) {
        collisionData.mode = WARNING;
    }
    else collisionData.mode = NO_DANGER;
}

/*
   Drive with the RC receiver
*/
void manual_drive(ReceiverData& receiverData, UltrasoundData& usData, CarInfoData& carData) {
    int throttle = receiverData.throttle;
    int direction = receiverData.direction;


    
    if (collisionData.mode == CollisionMode::INMINENT) {
        switch (carData.status) {
            case Car::Status::BRAKING:
                car.stop_braking();
                break;
            case Car::Status::MOVING_FD:
                car.start_braking();  // After brake, the status must be STOPPED (Blocking operation)
                break;
            case Car::Status::MOVING_BD:
                car.run(throttle, direction);
                break;
            default:
                // We only allow going backwards or turn until the danger disappears
                if (throttle <= 0 || direction != 0) {
                    car.run(throttle, direction);
                }
                break;
        }
    } else {
        car.run(throttle, direction);
    }
    
   //car.run(throttle, direction);
}

void log_data(ReceiverData& receiverData, UltrasoundData& usData, CarInfoData& carData, CollisionData& collisionData) {
    if ( carData.status < 0 || 
        (last_record_written.status >= 0 && last_record_written.status == Car::Status::STOPPED && carData.status == last_record_written.status))
        return;  // No log if the car is not moving from the last snapshot

    CarDataRecord record;
    long time = millis();
    
    record.millis = time;
    record.us_center = (unsigned short)usData.front_middle;
    record.us_left = (unsigned short)usData.front_left;
    record.us_right = (unsigned short)usData.front_right;
    record.pwmleft = (char)carData.pwmLeft;
    record.pwmright = (char)carData.pwmRight;
    record.collision = (char)collisionData.mode;
    record.status = (char)carData.status;
    record.direction = (char)carData.direction;
    dataLogger.log(record);
    last_record_written = record;
}

// cppcheck-suppress unusedFunction
void loop() {
    if (DEBUG) counter++;
    
    if (millis() >= time_ultrasonic_sensor + PERIOD_ULTRASONIC) {  // We read us sensor every PERIOD_ULTRASONIC
        time_ultrasonic_sensor += PERIOD_ULTRASONIC;
        usData = ultrasound.read();
        lights.handle_ultrasensor_lights(usData.front_left < STOP_DISTANCE_SIDES, usData.front_middle < STOP_DISTANCE_FRONT,
                                         usData.front_right < STOP_DISTANCE_SIDES);
    }
    
    
    CarInfoData carInfo = car.read_data();
    ReceiverData receiverData = receiver.read();
    updateCollisionData(receiverData, usData);

    if (millis() >= time_print_lcd + PERIOD_TO_PRINT_LCD) {
        time_print_lcd += PERIOD_TO_PRINT_LCD;
        lcd.print_car_info(collisionData.mode, 1, usData, carInfo);
    }
    
    
    if (DEBUG && millis() >= time_print_serial + PERIOD_TO_PRINT_SERIAL) {
        time_print_serial += PERIOD_TO_PRINT_SERIAL;
        char buffer[150] = "";
        sprintf(buffer,
                "millis:%lu, Loops:%lu/sec, Status:%d, Left:%d, Right:%d, Direction:%d, possibleCrash:%d, "
                "fl:%d, fm:%d, fr:%d",
                millis(), counter, carInfo.status, carInfo.pwmLeft, carInfo.pwmRight, receiverData.direction,
                collisionData.mode, usData.front_left, usData.front_middle, usData.front_right);
        Serial.println(buffer);
        counter = 0;
    }

    if (millis() >= time_car_update + PERIOD_CAR_UPDATE) {
        time_car_update += PERIOD_CAR_UPDATE;
        lights.handle_rear_lights(carInfo.direction == 0);
        if (receiverData.enabled) {
            switch (receiverData.mode) {
                case Receiver::Mode::ONE:
                    manual_drive(receiverData, usData, carInfo);
                    break;
                case Receiver::Mode::TWO:
                    lcd.print("Mode2: Car Armed!", Lcd::Row::FIRST);
                    car.stop();
                    break;
                case Receiver::Mode::FAILSAFE:
                    lcd.print("Signal lost!", Lcd::Row::FIRST);
                    car.stop();
                    break;
            }
        } else {
            car.stop();
            lcd.print("Car Disabled!", Lcd::Row::FIRST);
        }
        //
    }

    //Writing data to the log every PERIOD_LOG_DATA
    if (LOG_DATA && millis() >= (time_log_data + PERIOD_LOG_DATA)) {
        time_log_data += PERIOD_LOG_DATA;
        log_data(receiverData, usData, carInfo, collisionData);
    }

    //Flushing the log data every 10 seconds
    if (LOG_DATA && millis() >= (time_flush_data + PERIOD_FLUSH_DATA)) {
        time_flush_data += PERIOD_FLUSH_DATA;
        dataLogger.flush();
    }

}

