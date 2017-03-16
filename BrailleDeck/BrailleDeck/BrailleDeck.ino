/*
 Name:		BrailleDeck.ino
 Created:	3/13/2017 1:34:26 PM
 Author:	David
*/

/*
Breadboard pin's (using the RS50 connector numbers)
10 GND
4 is pulled low when the board is connected (so it becomes a second gnd)

Perkings PinNr			->		Uno PinNr			M0 PinNr
	Key1		= 1		->		4					5
	Key2		= 3		->		5					6
	Key3		= 5		->		7					9
	Key4		= 7		->		9					10
	Key5		= 9		->		11					11
	Key6		= 8		->		10					12
	Space		= 6		->		8					A2/16
	Conection	= 4		->		6					A1/15
	GND			= 10	->		GND					GND
	
LCD and RTC			SDA and SLC
SD Card				SCK, MOSI and MISO

Buttons
	UP							-					A3/17
	Down						-					A4/18

Buzzer							-					A5/19

These key's use the 4 pin as their GND
Enter = 1 (Shared with Key1)
Backspace = 3 (Shared with Key2)
Space = 6 (Not shared, uno  PinNr 8)

Pin 2 is not used

*/

#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include <SPI.h>
#include <SD.h>

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

//Uno Pins
//int Spacepin = 8;
//int ConnectionPin = 6;
//int Keypins[6] = { 4,5,7,9,11,10 };

//M0 Pins
int Spacepin = 16;
int ConnectionPin = 15;
int Keypins[6] = { 5,6,9,10,11,12 };
int chipSelect = 17;
int Uppin = 14;
int Downpin = 18;

const int PowerTwo[6] = { 1,2,4,8,16,32 };

//full list of characters; but specials are not working, so they're replaced with # as well as blanks 
//char BrailleABC[65] = "a,b'k;l#cif#msp#e:h*o!r#djg@ntq#â?ê-u(v#îöë#xèç#û.ü)z=à#ôwï#yùé";
const char BrailleABC[65] = "a,b'k;l#cif#msp#e:h*o!r#djg@ntq##?#-u(v#####x####.#)z=###w##y##";
const char BrailleABCaps[65] = "A,B'K;L#CIF#MSP#E:H*O!R#DJG@NTQ##?#-U(V#####X####.#)Z=###W##Y##";
const char BrailleNumbers[65] = "1#2#####396#####5#8#####407#####################################";
 
void setup() {
	// put your setup code here, to run once:
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	Serial.begin(9600);
	while (!Serial);
	Serial.println("Serial Started");

	lcd.begin(20, 4);
	// Print a message to the LCD.
	lcd.print("Brailledeck");
	Serial.println("lcd started");
	
	for (int PinNr = 0; PinNr <= 5; PinNr++) {
		pinMode(Keypins[PinNr], INPUT_PULLUP);
	}
	pinMode(Spacepin, INPUT_PULLUP);
	pinMode(ConnectionPin, INPUT_PULLUP);

	Serial.println("Waiting for connection");
	bool connected = false;
	int connectorcheck = HIGH;
	while (!connected) {
		connectorcheck = digitalRead(ConnectionPin);
		if (connectorcheck == HIGH) {
			//Serial.print(".");
		}
		else {
			connected = true;
			digitalWrite(13, HIGH);
		}
	}
	Serial.println("");
	Serial.println("connected!");
	lcd.clear();
}

bool Active = false;
 
int EnterPreviousState = HIGH;
int BackPreviousState = HIGH;

bool Caps = false;
bool CapsPermanent = false;
bool Number = false;

void loop() {
	//CheckConnection();

	if (Active) {
		CheckButtons();
	}
	else {
		Active = CheckActivity();
	}
}

int RowNR = 0;
int PosNR = 0;

void CheckButtons() {
	bool ButtonTrigger = true;
	int KeyRecordings[6] = { 0,0,0,0,0,0 };
	int SpaceRecording = 0;
	int SpaceButton = HIGH;
	int Lowtest = HIGH;
	bool ButtonPressed = true;

	int BrailleCounter = 0;

	while (ButtonTrigger) {

		ButtonPressed = false;
		delay(100);

		SpaceButton = digitalRead(Spacepin);

		if (SpaceButton == LOW) {
			//Serial.print("spacebuttonstate: ");
			//Serial.println(SpaceButton);
			SpaceRecording = 1;
			ButtonPressed = true;
		}

		for (int i = 0; i <= 5; i++) {
			Lowtest = digitalRead(Keypins[i]);

			if (Lowtest == LOW) {
				KeyRecordings[i] = 1;
				ButtonPressed = true;
			}
		}

		if (ButtonPressed) {
			ButtonTrigger = true;
		}
		else {
			ButtonTrigger = false;
		}
	}
	Active = false;

	int BrailleIndex = 0;

	for (BrailleCounter = 0; BrailleCounter <= 5; BrailleCounter++) {
		//Serial.print(KeyRecordings[BrailleCounter]);
		//delay(1);
		BrailleIndex = BrailleIndex + KeyRecordings[BrailleCounter] * PowerTwo[BrailleCounter];
		//Serial.print(",");
		//delay(1);
		//Serial.println(BrailleIndex);
		//delay(1);
	}

	if (BrailleIndex == 0) {
		Serial.print(" ");
		lcd.print(" ");
		PosNR++;
		CapsPermanent = false;
		Number = false;
	}
	else if (SpaceRecording == 1) {
		BrailleIndex--;
		//Serial.print("Braille index: ");
		//Serial.println(BrailleIndex);
		if (BrailleIndex == 39) {
			Caps = true;
		}
		else if (BrailleIndex == 23) {
			CapsPermanent = true;
		}
		else if (BrailleIndex == 59) {
			Number = true;
		}
		else {
			if (Caps) {
				Serial.print(BrailleABCaps[BrailleIndex]);
				lcd.print(BrailleABCaps[BrailleIndex]);
				Caps = false;
			}
			else if (CapsPermanent) {
				Serial.print(BrailleABCaps[BrailleIndex]);
				lcd.print(BrailleABCaps[BrailleIndex]);
			}
			else if (Number) {
				Serial.print(BrailleNumbers[BrailleIndex]);
				lcd.print(BrailleNumbers[BrailleIndex]);
			}
			else {
				Serial.print(BrailleABC[BrailleIndex]);
				lcd.print(BrailleABC[BrailleIndex]);
			}
		PosNR++;
		}
	}
	else if (BrailleIndex==1){
		Serial.println("");		
		RowNR++;
		PosNR = 0;
		lcd.setCursor(PosNR,RowNR);
	}
	else if (BrailleIndex == 2) {
		Serial.print("BACK");
		PosNR--;
		lcd.setCursor(PosNR, RowNR);
		lcd.print(" ");
		lcd.setCursor(PosNR, RowNR);
	}
	if (PosNR == 20) {
		RowNR++;
		PosNR = 0;
		lcd.setCursor(PosNR, RowNR);
	}
}

int Activitytest = HIGH;
bool CheckActivity() {
	for (int i = 0; i <= 5; i++) {
		Activitytest = digitalRead(Keypins[i]);
		if (Activitytest == LOW) {
			return true;
		}
	}

	Activitytest = digitalRead(Spacepin);
	if (Activitytest == LOW) {
		return true;
	}

	return false;
}

int ConnectionChecker = HIGH;
int ConnectionPrevState = HIGH;
void CheckConnection() {
	ConnectionChecker = digitalRead(ConnectionPin);
	if (ConnectionChecker != ConnectionPrevState) {
		ConnectionPrevState = ConnectionChecker;

		if (ConnectionChecker == HIGH) {
			Serial.println("Connection Lost");
			delay(100);
			digitalWrite(13, LOW);
		}
		else {
			digitalWrite(13, HIGH);
		}
	}
}