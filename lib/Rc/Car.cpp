#include "Car.h"

#include <Constants.h>

void Car::init() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    Car::speed_up(0, 0, Direction::FORWARD);
}

void Car::run(int pwmThrottle, int pwmDirection) {
    Direction direction = (pwmThrottle >= 0) ? Car::Direction::FORWARD : Car::Direction::BACKWARD;
    this->data.direction = direction;

    if (pwmThrottle == 0 && pwmDirection == 0) {
        Car::speed_up(0,0, direction);
        this->data.status = Car::Status::STOPPED; 
    }

    if (pwmThrottle !=0 && pwmDirection == 0) {
        this->data.status = (direction == FORWARD) ? Car::Status::MOVING_FD : Car::Status::MOVING_BD;
        Car::speed_up(abs(pwmThrottle), abs(pwmThrottle), direction);
    }
    else { // The car is going right or left
        Car::turn(pwmThrottle, pwmDirection);
    }
}

void Car::speed_up(int pwmLeft, int pwmRight, Car::Direction direction) {
    Car::set_direction(direction);
  
    if (this->data.pwmLeft != pwmLeft || this->data.pwmRight != pwmRight) {
        this->data.pwmLeft = pwmLeft;
        this->data.pwmRight = pwmRight;
        analogWrite(ENA, pwmLeft);
        analogWrite(ENB, pwmRight);
    }
}

/**
 * forward = 1
 * backward = 0
 */

void Car::set_direction(Direction direction) {
    // Motor left
    digitalWrite(IN2, direction);
    digitalWrite(IN1, !direction);
    // Motor right
    digitalWrite(IN3, direction);
    digitalWrite(IN4, !direction);
}

void Car::stop() { Car::speed_up(0, 0, Direction::FORWARD); }

/*
Only stop braking if the time is greater than braking_time
*/
void Car::stop_braking() {
    if (millis()-start_braking_time >= this->braking_time) {
        Car::speed_up(0, 0, Direction::FORWARD);
        this->data.status = Car::Status::STOPPED;
    }
}

void Car::start_braking() {
    if ( data.pwmLeft == data.pwmRight ) {
        this->data.status = Car::Status::BRAKING;
        Car::speed_up(data.pwmLeft + 20, data.pwmRight + 20, Direction::BACKWARD);
        this->start_braking_time = millis();
        if (data.pwmLeft < 60)
            this->braking_time = 80;
        else if (data.pwmLeft > 60 && data.pwmLeft < 100)
            this->braking_time = 100;
        else if (data.pwmLeft >= 100)
            this->braking_time = 130;
    }
}

/*
    pwm values for throttle and direction
*/
void Car::turn(int pwmThrottle, int pwmDirection) {
    int motor1 = min(MOTOR_MAX_PWM, abs(pwmThrottle) + abs(pwmDirection));
    int motor2 = max(0, abs(pwmThrottle) - abs(pwmDirection));

    if (pwmThrottle >= 0 && pwmDirection > 0) {  // Forward and right
        this->data.status = Car::Status::TURN_RIGHT;
        Car::speed_up(motor1, motor2, Car::Direction::FORWARD);
    } else if (pwmThrottle >= 0 && pwmDirection < 0) {  // Forward and left
        this->data.status = Car::Status::TURN_LEFT;
        Car::speed_up(motor2, motor1, Car::Direction::FORWARD);
    } else if (pwmThrottle < 0 && pwmDirection > 0) {  // Backward and left
        this->data.status = Car::Status::TURN_LEFT;
        Car::speed_up(motor2, motor1, Car::Direction::BACKWARD);
    } else if (pwmThrottle < 0 && pwmDirection < 0) {  // Backward and right
        this->data.status = Car::Status::TURN_RIGHT;
        Car::speed_up(motor1, motor2, Car::Direction::BACKWARD);
    }
}

CarInfoData Car::read_data() { return data; }