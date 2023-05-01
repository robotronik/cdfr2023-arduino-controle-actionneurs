#include "servoControl.h"

servoControl::servoControl(){
    position = 0;
    commande = 0;
    vitesePrecedent = 0;
}

void servoControl::setup(int pin, int defaultPos,double _vitesseMax, double _accelrationMax){
    servo.attach(pin);
    servo.write(0);
    delayPrecedent = millis();
    position = defaultPos;
    commande = defaultPos;
    servo.write(position);
    vitesseMax = _vitesseMax;
    accelrationMax = _accelrationMax;
}

void servoControl::setVitesseAcceleration(double vitesse, double acceleration){
    vitesseMax = vitesse;
    accelrationMax = acceleration;
}

void servoControl::setpos(int pos){
    commande = pos;
}
void servoControl::loop(void){
    if (delayPrecedent < millis()){
        delayPrecedent = millis()+TEMPSASSERVE;
        double distance = (commande - position);
        double vitesse;
        if(distance>0){
            vitesse = vitesseMax;
        }
        else{
            vitesse = - vitesseMax;
        } 
        
        double distance_a_parcourir = (vitesePrecedent * vitesePrecedent) / (2 * accelrationMax);
        if(vitesePrecedent>0 && distance_a_parcourir > distance){
            vitesse = vitesePrecedent - accelrationMax;
        }
        else if(vitesePrecedent<0 && -distance_a_parcourir < distance){
            vitesse = vitesePrecedent + accelrationMax;
        }
        else if(vitesse > vitesseMax){
            vitesse = vitesseMax;
        }
        else if(vitesse < -vitesseMax){
            vitesse = -vitesseMax;
        }
        else if(vitesse - vitesePrecedent > accelrationMax){
            vitesse = vitesePrecedent + accelrationMax;
        }
        else if(vitesse - vitesePrecedent < -accelrationMax){
            vitesse = vitesePrecedent - accelrationMax;
        }
        vitesePrecedent = vitesse;
        position = position + vitesse;
        if(distance <= 2*accelrationMax && distance >= -2*accelrationMax);
        else{
            servo.write(position);
        }        
    }
}

servoControl::~servoControl()
{
}