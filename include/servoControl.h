#pragma once
#include <Arduino.h>
#include <Servo.h>

#define TEMPSASSERVE 20

class servoControl
{
private:
    /* data */
    Servo servo;
    int commande;
    double position;
    double vitesePrecedent;
    double vitesseMax = 5;
    double accelrationMax = 0.3;
    unsigned long delayPrecedent;
public:
    servoControl();
    void setup(int pin,int defaultPos = 0,double _vitesseMax = 5, double _accelrationMax = 0.3);
    void setVitesseAcceleration(double vitesse, double acceleration);
    void setpos(int pos);
    void loop(void);
    ~servoControl();
};


