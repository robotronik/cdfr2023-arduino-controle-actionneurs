#include <Arduino.h>
#include <AccelStepper.h>
#include "servoControl.h"
#include <Arduino.h>
#include <string.h>
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
#define MAX_SPEED 1000  // Vitesse maximale (en pas par seconde)
#define MAX_ACCEL 2000 // Accélération maximale (en pas par seconde carré)


#define PINMOTEUR A7

#define LED_IU_4 A13
#define LED_IU_2 A12
#define LED_IU_3 52
#define LED_IU_1 50
#define BOUTON_IU_3 A11
#define BOUTON_IU_2 A10
#define BOUTON_IU_4 48
#define BOUTON_IU_1 46

const char UIB_pins[] = {BOUTON_IU_4, BOUTON_IU_3, BOUTON_IU_2, BOUTON_IU_1};
const char UIL_pins[] = {LED_IU_4, LED_IU_3, LED_IU_2, LED_IU_1};

//*********************************
// FONCTIONS
//*********************************
void traitementDesCommandes(char* name,char* svalue);

//*********************************
// VARAIBLES OBJETS
//*********************************
//VARAIBLES
char buffer[256] = {0};
unsigned int index = 0;
int bitRequestI2C;

//OBJETS
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
servoControl servos[7];
const int numservos = sizeof(servos)/sizeof(servos[0]);


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
	servos[0].setup(2);
	servos[1].setup(3);
	servos[2].setup(4,180);
	servos[3].setup(5,0);
	servos[4].setup(6,180,5,0.3);
	servos[5].setup(7,180,5,0.3);
	servos[6].setup(8,180,5,0.3);

	pinMode(PINMOTEUR,OUTPUT);
	digitalWrite(PINMOTEUR,0);

	pinMode(24, INPUT_PULLUP);
	pinMode(26, INPUT_PULLUP);
	pinMode(28, INPUT_PULLUP);
	pinMode(30, INPUT_PULLUP);

	for (int i = 0; i < 4; i++)
	{
		pinMode(UIB_pins[i], INPUT);
		pinMode(UIL_pins[i], OUTPUT);
	}

}



//*********************************
// LOOP
//*********************************
void loop() {
	if (Serial.available()) {
		int numtoread = Serial.available();
		int spaceleft = sizeof(buffer)-1-index;
		if (numtoread > spaceleft)
		{
			numtoread = spaceleft;
		}
		int numread = Serial.readBytes(&buffer[index], numtoread);
		
		index += numread;
		int readstartidx = 0;
		buffer[index] = '\0';
		for (unsigned int i = 0; i < index; i++)
		{
			switch (buffer[i])
			{
			case '!':
				readstartidx = i+1; //command starts right after
				break;
			case '\n': //process the command
				{
					char name[16] = {0};
					char value[TAILLEBUFFER] = {0};
					int numparsed = sscanf(&buffer[readstartidx], "%[^:\n]:%[^:\n]\n", name, value);
					if (numparsed == 2)
					{
						traitementDesCommandes(name,value);
					}
					readstartidx = i+1;
				}
				break;
			default:
				break;
			}
		}
		//no command was parsed + buffer full = reset
		if (index >= sizeof(buffer)-1 && readstartidx == 0)
		{
			index = 0;
			memset(buffer, 0, sizeof(buffer));
		}
		else
		{
			memmove(buffer, &buffer[readstartidx], index-readstartidx);
			index -= readstartidx;
			buffer[index] = '\0';
		}
		
		
	}

	for (int i = 0; i < numservos; i++)
	{
		servos[i].loop();
	}

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

template<class T>
void I2CSendRecv(char* svalue, const char* parseformat)
{
	Wire.beginTransmission(42);
	const char* sendformat = parseformat;
	int index = 0, numread;
	T value;
	uint8_t* valueptr = (uint8_t*)&value;
	while (index < TAILLEBUFFER && sscanf(&svalue[index], sendformat, &value, &numread)==2)
	{
		if (index == 0)
		{
			Wire.write((uint8_t)value); //first read : command (as 1 byte)
		}
		else
		{
			for (uint8_t i = 0; i < sizeof(T); i++)
			{
				Wire.write(valueptr[i]);
			}
		}
		index += numread;
		switch (svalue[index])
		{
		case ',': //more to transmit
			index++;
			break;
		
		default: //error type of zeros
			goto endsend;
			break;
		}
	}
endsend:
	Wire.endTransmission();

	if(bitRequestI2C!=0){
		Wire.requestFrom(42, bitRequestI2C);
		bitRequestI2C = 0;
		Serial.print("I2C:");
		while (Wire.available()) {  // while byte are available (Normaly the while are process 1 time to get the angle in degres)
			value = 0;
			for (uint8_t i = 0; i < sizeof(T); i++)
			{
				valueptr[i] = Wire.read();
			}
			Serial.print(value);
			if(Wire.available()){
				Serial.print(",");
			}        
		}
		Serial.println("");
	}
}

void traitementDesCommandes(char* name,char* svalue){

	int servoidx;
	int luiidx;
	int buiidx;
	if(sscanf(name,"servo%d", &servoidx) == 1){
		int ivalue;
		sscanf(svalue, "%d", &ivalue);
		if (servoidx == 4)
		{
			ivalue = 180-ivalue;
		}
		servoidx--;
		servos[servoidx].setpos(ivalue);
		return;
	}
	else if(strcmp(name,"stepper1") == 0){
		int ivalue;
		sscanf(svalue, "%d", &ivalue);
		stepper1.moveTo(ivalue);
		return;
	}
	if(strcmp(name,"moteur1") == 0){
		int ivalue;
		sscanf(svalue, "%d", &ivalue);
		digitalWrite(PINMOTEUR,ivalue);
		return;
	}
	if(strcmp(name,"bouton1") == 0){
		Serial.print("bouton1:");
		Serial.println(!digitalRead(24));
		return;
	}
	if(sscanf(name,"boutonUI%x", &buiidx) == 1){
		Serial.print(name);
		Serial.print(":");
		Serial.println(digitalRead(UIB_pins[buiidx-1]));
		return;
	}
	if(sscanf(name,"ledUI%x", &luiidx) == 1){
		int ivalue;
		sscanf(svalue, "%d", &ivalue);
		digitalWrite(UIL_pins[luiidx-1],ivalue);
		return;
	}
	if(strcmp(name,"I2CRequest") == 0){
		sscanf(svalue, "%d", &bitRequestI2C);  
		return;  
	}
	if(strcmp(name,"I2CSend") == 0){
		I2CSendRecv<uint16_t>(svalue, "%d%n");
		return;
	}
	if(strcmp(name,"IFS") == 0){
		I2CSendRecv<float>(svalue, "%f%n");
		return;
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

