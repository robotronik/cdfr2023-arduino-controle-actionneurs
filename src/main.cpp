#include <Arduino.h>
#include <AccelStepper.h>
#include "servoControl.h"
#include <Arduino.h>
#include <Wire.h>


//*********************************
// MACROS
//*********************************

#define TAILLEBUFFER 256

// Configuration des broches de commande du moteur
#define DIR_PIN 13
#define STEP_PIN 12

// Configuration du moteur pas à pas
#define STEPS_PER_REV 200  // Nombre de pas pour une révolution complète
#define MAX_SPEED 6000  // Vitesse maximale (en pas par seconde)
#define MAX_ACCEL 5000 // Accélération maximale (en pas par seconde carré)


#define PINMOTEUR A7

#define LED_IU_4 A13
#define LED_IU_2 A12
#define LED_IU_3 52
#define LED_IU_1 50
#define BOUTON_IU_3 A11
#define BOUTON_IU_2 A10
#define BOUTON_IU_4 48
#define BOUTON_IU_1 46

//*********************************
// FONCTIONS
//*********************************
void traitementDesCommandes(char* name,char* svalue);

//*********************************
// VARAIBLES OBJETS
//*********************************
//VARAIBLES
unsigned long delayTemp = millis()+5000;
unsigned long delayTemp2 = millis()+0;
char buffer[256] = {0};
int index = 0;
int bitRequestI2C;

//OBJETS
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
servoControl servo1; 
servoControl servo2; 
servoControl servo3; 
servoControl servo4; 
servoControl servo5; 
servoControl servo6; 
servoControl servo7; 


//*********************************
// SETUP
//*********************************
void setup() {
    // Configuration du moteur pas à pas
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  stepper1.setMaxSpeed(MAX_SPEED);
  stepper1.setAcceleration(MAX_ACCEL);

  // Configuration Serial
  Serial.begin(115200);
  Wire.begin(100);
  pinMode(20, INPUT);
  pinMode(21, INPUT);
  digitalWrite(21, LOW);
  digitalWrite(20, LOW);
  Wire.setTimeout(1000);

  //Configuration Servo
  servo1.setup(2);
  servo2.setup(3);
  servo3.setup(4,180);
  servo4.setup(5,0);
  servo5.setup(6,180,5,0.3);
  servo6.setup(7,180,5,0.3);
  servo7.setup(8,180,5,0.3);

  pinMode(PINMOTEUR,OUTPUT);
  digitalWrite(PINMOTEUR,0);

  pinMode(24, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);

  pinMode(BOUTON_IU_1, INPUT);
  pinMode(BOUTON_IU_2, INPUT);
  pinMode(BOUTON_IU_3, INPUT);
  pinMode(BOUTON_IU_4, INPUT);
  pinMode(LED_IU_1, OUTPUT);
  pinMode(LED_IU_2, OUTPUT);
  pinMode(LED_IU_3, OUTPUT);
  pinMode(LED_IU_4, OUTPUT);

}



//*********************************
// LOOP
//*********************************
void loop() {
  if (Serial.available()) {
    // lire les caractères entrants jusqu'à ce que nous atteignions le caractère '\n'
    while (Serial.available() && index < sizeof(buffer)) {
      char c = Serial.read();
      buffer[index] = c;
      index++;
      if (c == '\n') {
        index = 0;
        char name[16] = {0};
        char value[TAILLEBUFFER] = {0};
        sscanf(buffer, "!%[^:]:%s", name, value);
        traitementDesCommandes(name,value);
      }
    }  
  }


  servo1.loop();
  servo2.loop();
  servo3.loop();
  servo4.loop();
  servo5.loop();
  servo6.loop();
  servo7.loop();
  stepper1.run();

  // Serial.println("ok1");
  // if(1){
  //   Wire.beginTransmission(42);
  //   Wire.write(10);
  //   Wire.endTransmission();
  // } 
  // Serial.println("FIN1");
  
  // delay(100);

  // Serial.println("ok2");
  // if(1){
  //   Wire.beginTransmission(42);
  //   Wire.write(11);
  //   Wire.endTransmission();
  // } 
  // Serial.println("FIN2");

  // delay(100);  
}

void traitementDesCommandes(char* name,char* svalue){
  if(strcmp(name,"servo1") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    servo1.setpos(ivalue);
  }
  if(strcmp(name,"servo2") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    servo2.setpos(ivalue);
  }
  if(strcmp(name,"servo4") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    servo3.setpos(180-ivalue);
  }
  if(strcmp(name,"servo4") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    servo4.setpos(ivalue);
  }
  if(strcmp(name,"servo5") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    servo5.setpos(ivalue);
  }
  if(strcmp(name,"servo6") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    servo6.setpos(ivalue);
  }
  if(strcmp(name,"servo7") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    servo7.setpos(ivalue);
  }
  if(strcmp(name,"stepper1") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    stepper1.moveTo(ivalue);
  }
  if(strcmp(name,"moteur1") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    digitalWrite(PINMOTEUR,ivalue);
  }
  if(strcmp(name,"bouton1") == 0){
    Serial.print("bouton1:");
    Serial.println(!digitalRead(24));
  }
  if(strcmp(name,"bouton2") == 0){
    Serial.print("bouton2:");
    Serial.println(!digitalRead(24));
  }
  if(strcmp(name,"bouton3") == 0){
    Serial.print("bouton3:");
    Serial.println(!digitalRead(24));
  }
  if(strcmp(name,"bouton4") == 0){
    Serial.print("bouton4:");
    Serial.println(!digitalRead(24));
  }
  if(strcmp(name,"boutonUI1") == 0){
    Serial.print("boutonUI1:");
    Serial.println(digitalRead(BOUTON_IU_1));
  }
  if(strcmp(name,"boutonUI2") == 0){
    Serial.print("boutonUI2:");
    Serial.println(digitalRead(BOUTON_IU_2));
  }
  if(strcmp(name,"boutonUI3") == 0){
    Serial.print("boutonUI3:");
    Serial.println(digitalRead(BOUTON_IU_3));
  }
  if(strcmp(name,"boutonUI4") == 0){
    Serial.print("boutonUI4:");
    Serial.println(digitalRead(BOUTON_IU_4));
  }
  if(strcmp(name,"ledUI1") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    digitalWrite(LED_IU_1,ivalue);
  }
  if(strcmp(name,"ledUI2") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    digitalWrite(LED_IU_2,ivalue);
  }
  if(strcmp(name,"ledUI3") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    digitalWrite(LED_IU_3,ivalue);
  }
  if(strcmp(name,"ledUI4") == 0){
    int ivalue;
    sscanf(svalue, "%d", &ivalue);
    digitalWrite(LED_IU_4,ivalue);
  }
  if(strcmp(name,"I2CRequest") == 0){
    sscanf(svalue, "%d", &bitRequestI2C);    
  }
  if(strcmp(name,"I2CSend") == 0){
    int16_t ivalue;
    int8_t commande;
    int  buffvalue = 0;

    Wire.beginTransmission(42);

    sscanf(svalue, "%d", &commande);
    while(buffvalue < TAILLEBUFFER && svalue[buffvalue]!=',' && svalue[buffvalue]!='\n'){
        buffvalue ++;
    }
    if(buffvalue < TAILLEBUFFER && svalue[buffvalue]==','){
        buffvalue ++;
    }
    Wire.write((uint8_t)commande);

    while(buffvalue < TAILLEBUFFER && svalue[buffvalue]!='\n'){
      sscanf(svalue + buffvalue, "%d", &ivalue);
      Wire.write((uint8_t)ivalue);
      Wire.write((uint8_t)(ivalue>>8));      
      while(buffvalue < TAILLEBUFFER && svalue[buffvalue]!=',' && svalue[buffvalue]!='\n'){
          buffvalue ++;
      }
      if(buffvalue < TAILLEBUFFER && svalue[buffvalue]==','){
          buffvalue ++;
      } 
    }
    Wire.endTransmission();

    if(bitRequestI2C!=0){
      Wire.requestFrom(42, bitRequestI2C);
      bitRequestI2C = 0;
      Serial.print("I2C:");
      while (Wire.available()) {  // while byte are available (Normaly the while are process 1 time to get the angle in degres)
        uint8_t c1 = Wire.read(); // receive the first byte (example : 0x01)
        uint8_t c2 = Wire.read(); // receive the second byte (example : 0x45)
        int final =  c1 | (c2<<8); // assemble the two bytes (example : 0x0145)
        Serial.print(final,DEC);
        if(Wire.available()){
          Serial.print(",");
        }        
      }
      Serial.println("");
    }
    
  }
    

    // while (sscanf(svalue + buffvalue, ",%d", &ivalue)){
    //   Serial.print(ivalue);
    //   Serial.println(svalue + buffvalue);
    //   while(buffvalue < TAILLEBUFFER && svalue[buffvalue]!=',' && svalue[buffvalue]!='\n'){
    //     buffvalue ++;
    //   }
    // }
    


    // while (svalue[buffvalue]!='\n'){
    //   sscanf(svalue + buffvalue, "%d", &ivalue);
    //   Serial.println(ivalue + buffvalue);
    //   while(svalue[buffvalue]!=',' && svalue[buffvalue]!='\n'){
    //     svalue ++;
    //   }
    // }
    
    // sscanf(svalue, "%d", &ivalue);
    // Serial.println(ivalue)
    //Wire.write(ivalue);  
  
  
  
}

