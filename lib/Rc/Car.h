#ifndef CAR
#define CAR

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#include <SPI.h>
#endif

// Pin Definitions for MOTORs
#define ENA 4  // Analog
#define ENB 5  // Analog

// MOTOR LEFT CTL
#define IN1 29  // Digital
#define IN2 27
// MOTOR RIGHT CTL
#define IN3 25
#define IN4 23

#define MOTOR_LEFT_OFFSET 0
#define MOTOR_HIGH_SPEED_FROM 60
#define MOTOR_OFFSET 0
#define MOTOR_BRAKE_TIMEOUT 100  // ms

struct CarInfoData {
    int status;
    int pwmLeft;
    int pwmRight;
    int direction;
};

class Car {
   public:
    enum Direction { FORWARD = 1, BACKWARD = 0 };
    enum Status { MOVING_BD = 1, MOVING_FD = 2, BRAKING = 3, STOPPED = 4, TURN_RIGHT = 5, TURN_LEFT = 6 };

    Car() {
        data.status = -1;
        data.direction = -1;
        data.pwmLeft = -1;
        data.pwmRight = -1;
    }
    void init();
    void start_braking();
    void stop_braking();
    void run(int pwmThrottle, int pwmDirection);
    void stop();
    CarInfoData read_data();

   private:
    CarInfoData data;
    long start_braking_time;
    int braking_time;
    void set_direction(Direction direction);
    void speed_up(int pwmLeft, int pwmRight, Direction direction);
    void turn(int pwmThrottle, int pwmDirection);
};

#endif  //