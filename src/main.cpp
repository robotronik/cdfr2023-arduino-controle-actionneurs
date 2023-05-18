#include <Arduino.h>
#include <AccelStepper.h>
#include "servoControl.h"
#include <Arduino.h>
#include <string.h>
#include <Wire.h>
#include <Servo.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>


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

// Configuration de l'ESC
#define PINESC 9
#define MIN_PULSE_WIDTH 1000
#define MAX_PULSE_WIDTH 2000
#define ARM_DELAY 2000

// Configuration Servomoteur porte Aspiration
#define PINASPIRATION 3

// Configuration des étagères
#define PINETAGEREBAS 6
#define PINETAGEREMILIEU 7
#define PINETAGEREHAUT 8

// Configuration de la pince 
#define PINPINCE 5

// Configuration du module Bluetooth
#define PINBT_TX NULL
#define PINBT_RX NULL
#define PINKEY NULL
#define PINSTATE NULL

// Configuration écran I2C
#define I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//communication BT
#define statePin 2
#define keyPin 5
#define hc05BR 38400

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
//VARIABLES
char buffer[TAILLEBUFFER] = {0};
unsigned int index = 0;
int bitRequestI2C;
int score;
int score_affiche;
int score_recu=0;
int score_panier;
//addresses BT des modules BT utilisés :
//Robot: 98d3:11:fcc42f
//Panier: 98D3,41,F62269


uint64_t lastKeepalive = 0;

//OBJETS
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
servoControl servos[7];
const int numservos = sizeof(servos)/sizeof(servos[0]);
Servo esc;
SSD1306AsciiAvrI2c oled;


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

	// Configuration I2C
	Wire.begin(100);
	


	// Configuration Broches de la platine
	pinMode(20, INPUT); // Bouton 1 
	pinMode(21, INPUT); // Bouton 2
	digitalWrite(21, LOW);
	digitalWrite(20, LOW);
	Wire.setTimeout(1000);


	// Configuration ESC
	initESC();

	// Configuration Bluetooth
	initBluetooth();

	// Configuration LCD
	initLCD();

	//Configuration Servo
	servos[0].setup(2); 		// Non assigné
	servos[1].setup(PINASPIRATION); 		// Porte d'aspiration
	servos[2].setup(4,180); 	// N'existe pas
	servos[3].setup(PINPINCE,0); 		// Position de la pince
	servos[4].setup(PINETAGEREBAS,180,5,0.3); // étagère du bas
	servos[5].setup(PINETAGEREMILIEU,180,5,0.3); // étagère du milieu
	servos[6].setup(PINETAGEREHAUT,180,5,0.3); // étagère du haut


	/* Définitions des servos de 0 à 7 :
	Servo 0 : 
	*/

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
	//ecran oled pour l'affichage des points
	oled.begin(&Adafruit128x64, I2C_ADDRESS);
	oled.setFont(System5x7);
	oled.clear();
	oled.set1X();
	//modules BT
	pinMode(keyPin, OUTPUT);
	digitalWrite(keyPin, LOW); // Mode AT  
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
		unsigned int readstartidx = 0;
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
					if (readstartidx == i)
					{
						break;
					}
					
					char name[16] = {0};
					char value[TAILLEBUFFER] = {0};
					int numparsed = sscanf(&buffer[readstartidx], "%[^:\n]:%[^:\n]\n", name, value);
					if (numparsed >= 1)
					{
						/*Serial.print("Found command : at ");
						Serial.print(i);
						Serial.print(" length ");
						Serial.print(i-readstartidx);
						Serial.print(" name \"");
						Serial.print(name);
						Serial.print("\" value \"");
						Serial.print(value);
						Serial.println("\"");*/
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

	if (millis() >= lastKeepalive +1000)
	{
		Serial.print("keepalive:");
		Serial.println(millis(), HEX);
		lastKeepalive = millis();
	}
	

	for (int i = 0; i < numservos; i++)
	{
		servos[i].loop();
	}

	stepper1.run();
	//vu qu'écrire sur l'écran créé une fuite mémoire, on eessaye d'y écrire le moins de fois possible, 
	//pour cela, on attend la fin du match avant d'envoyer le score à l'arduino qui se charge d'afficher le score total (stratégie + panier)
	//l'arduino fait des requêtes aux panier afin de récupérer le score gagné grâce au panier
	if (score_recu == 1){
 		if (Serial1.available()) 
 		{
			//delay(100);
 			score_panier = Serial1.read();
			Serial1.flush();	
 		}
		score = score_recu + score_panier;
		if (score_affiche != score){
			oled.clear();
			oled.print(score);
			score_affiche = score;
		}
	}
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

template<class T, char const *fmt>
int ScanOne(char* svalue)
{
	int numread = 0;
	T value;
	uint8_t* valueptr = (uint8_t*)&value;
	int numscanned = sscanf(svalue, fmt, &value, &numread);
	/*Serial.print(numscanned);
	Serial.print(" ");
	Serial.print(value);
	Serial.print(" ");
	Serial.print(numread);
	Serial.print(" ");*/
	if (numscanned <1)
	{
		return 0;
	}
	
	for (uint8_t i = 0; i < sizeof(T); i++)
	{
		Wire.write(valueptr[i]);
	}
	switch (svalue[numread])
	{
	case ',':
		return numread+1;
		break;
	
	default:
		return numread; //force tripping next cycle
		break;
	}
}

const char I2CCommandFmt[] = "%hhd%n";

template<class T, char const *fmt>
void I2CSendRecv(char* svalue)
{
	Wire.beginTransmission(42);
	int index = 0, numread = 0;
	//Serial.print("send via i2c: ");
	numread = ScanOne<uint8_t, I2CCommandFmt>(svalue);
	int numwords = 0;

	T value;
	uint8_t* valueptr = (uint8_t*)&value;
	while (index < TAILLEBUFFER && numread > 0)
	{
		numwords++;
		index += numread;
		numread = ScanOne<T, fmt>(&svalue[index]);
		/*Serial.print(value);
		Serial.print(" ");
		Serial.print(numread);
		Serial.print("/");*/
	}
	//Serial.println("");
	Wire.endTransmission();
	/*Serial.print(" Sent ");
	Serial.print(numwords);
	Serial.print(" /receiving ");
	Serial.print(bitRequestI2C);
	Serial.println(" words by I2C");*/
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
			Serial.print(value, DEC);
			if(Wire.available()){
				Serial.print(",");
			}        
		}
		Serial.println("");
	}
}

const char I2CSendFmt[] = "%d%n";
const char IFSFmt[] = "%lx%n";

void traitementDesCommandes(char* name,char* svalue){

	int servoidx;
	int luiidx;
	int buiidx;
	if(strcmp(name,"setscore:")==0){
		int ivalue;
		sscanf(svalue, "%d", &ivalue);
		score = ivalue;
		score_recu = 1; //vu qu'écrire sur l'écran créé une fuite mémoire, on eessaye d'y écrire le moins de fois possible, 
		//pour cela, on attend la fin du match avant d'envoyer le score à l'arduino qui se charge d'afficher le score total (stratégie + panier)
	}
	if(sscanf(name,"servo%d", &servoidx) == 1){
		int ivalue;
		//Serial.println("command is Servo command");
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
	if(strcmp(name,"escspeed") == 0){
		int ivalue;
		sscanf(svalue, "%d", &ivalue);
		runESC(ivalue);
		return;
	}
	if(strcmp(name,"escinit") == 0){ // Initialise l'ESC
		initESC();
		return;
	}
	if(strcmp(name,"escdisarm") == 0){ // Désarme l'ESC
		disarmESC();
		return;
	}
	if(strcmp(name,"escstop") == 0){ // Stoppe l'ESC
		runESC(0);
		return;
	}
	if(strcmp(name,"getcerisenumber") == 0){ // Retourne le nombre de cerise à la Lattepanda
		int nbCerise = getNbCerise();
		Serial.print("nbCerise:");
		Serial.println(nbCerise);
		return;
	}
	if(strcmp(name,"setscore") == 0){ 
		int ivalue;
		sscanf(svalue, "%d", &ivalue);
		printLCD(ivalue);
	}
	if(strcmp(name,"opendoor")){ // Ouvre la porte d'aspiration
		// Pin Servo 7
		servos[1].setpos(180);
		return;
	}
	if(strcmp(name,"closedoor")){ // Ferme la porte d'aspiration
		servos[1].setpos(0);
		return;
	}
	if(strcmp(name,"bouton1") == 0){
		//Serial.println("command is bouton command");
		Serial.print("bouton1:");
		Serial.println(!digitalRead(24));
		return;
	}
	if(sscanf(name,"boutonUI%x", &buiidx) == 1){
		
		//Serial.println("command is uib command");
		Serial.print(name);
		Serial.print(":");
		Serial.println(digitalRead(UIB_pins[buiidx-1]));
		return;
	}
	if(sscanf(name,"ledUI%x", &luiidx) == 1){
		
		//Serial.println("command is uil command");
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
		I2CSendRecv<int16_t, I2CSendFmt>(svalue);
		return;
	}
	if(strcmp(name,"I2CS32") == 0){
		I2CSendRecv<int32_t, IFSFmt>(svalue);
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

// Initiaisation de l'ESC
 void initESC(){

  esc.attach(PINESC, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  esc.writeMicroseconds(MIN_PULSE_WIDTH);
  delay(ARM_DELAY);
 }

  //Désarmement de l'ESC
  void disarmESC(){
    esc.detach();
  }

  //Fonction pour faire tourner l'ESC
  void runESC(int percentage){

    percentage = constrain(percentage, 0, 100);

    int pulseWidth = map(percentage, 0, 100, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);

    esc.writeMicroseconds(pulseWidth);
  
  }

  // Fonction pour demander le nombre de cerise
  int getNbCerise(){
	if(digitalRead(PINSTATE) && Serial1.available()){
	  sendRequestCerise();
	  return Serial1.read();
	}else{
		return -1;
	}
  }

  void sendRequestCerise(){
	Serial1.write('s');
  }


  void initLCD(){
	oled.begin(&Adafruit128x64, I2C_ADDRESS);
	oled.setFont(System5x7);
	oled.clear();
	oled.set1X();

  }

  void initBluetooth(){
	pinMode(PINKEY, OUTPUT);
	pinMode(PINSTATE, INPUT);
	digitalWrite(PINKEY, LOW);
	// digitalWrite(PINSTATE, HIGH);
  }

  void printLCD(int nbr){
	oled.clear();
	oled.println(nbr);
  }