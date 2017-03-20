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
	Key4		= 7		->		9					11
	Key5		= 9		->		11					12
	Key6		= 8		->		10					A3/17
	Space		= 6		->		8					A2/16
	Conection	= 4		->		6					A1/15
	GND			= 10	->		GND					GND
	
LCD and RTC			SDA and SLC
SD Card				SCK, MOSI and MISO
					Chip Select						10

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

#include <RTClib.h>
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include <SPI.h>
#include <SD.h>

// LCD Variables Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);
int LCDChar = 20;
int LCDLines = 4;

// SD Variables
File myFile;

// RTC Variables
RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = { "Zondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrijdag", "Zaterdag" };

//Uno Pins
//int Spacepin = 8;
//int ConnectionPin = 6;
//int Keypins[6] = { 4,5,7,9,11,10 };

//M0 Pins
int Spacepin = 16;
int ConnectionPin = 15;
int Keypins[6] = { 5,6,9,11,12,17 };
int chipSelect = 10;
int Uppin = 14;
int Downpin = 18;

const int PowerTwo[6] = { 1,2,4,8,16,32 };

//full list of characters; but specials are not working, so they're replaced with # as well as blanks 
//char BrailleABC[65] = "a,b'k;l#cif#msp#e:h*o!r#djg@ntq#�?�-u(v#���#x��#�.�)z=�#�w�#y��";
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

	// Starting LCD
	lcd.begin(LCDChar, LCDLines);
	lcd.print("Brailledeck");
	Serial.println("lcd started");

	// Starting RTC
	if (!rtc.begin()) {
		Serial.println("Couldn't find RTC");
		while (1);
	}

	// Starting SD Card
	Serial.print("Initializing SD card...");
	if (!SD.begin(10)) {
		Serial.println("initialization failed!");
		return;
	}
	Serial.println("initialization done.");

	//Creating SD file
	if (SD.exists("example.txt")) {
		Serial.println("example.txt exists.");
	}
	else {
		Serial.println("example.txt doesn't exist.");
	}

	// open a new file and immediately close it:
	Serial.println("Creating example.txt...");
	myFile = SD.open("example.txt", FILE_WRITE);
	
	// Getting weekday and writing to file
	DateTime now = rtc.now();
	String weekday = daysOfTheWeek[now.dayOfTheWeek()];
	Serial.print("Vandaag is het ");
	Serial.println(weekday);
	myFile.print("Vandaag is het ");
	myFile.println(weekday);
	myFile.close();
	
	//Setting Perkings pin inputs
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

	Decoding(BrailleIndex, SpaceRecording);
}

void Decoding(int BrIndex, int Space) {
	if (BrIndex == 0) {
		LCDWriting(1, '#');
		CapsPermanent = false;
		Number = false;
	}
	else if (Space == 1) {
		BrIndex--;
		//Serial.print("Braille index: ");
		//Serial.println(BrailleIndex);
		if (BrIndex == 39) {
			Caps = true;
		}
		else if (BrIndex == 23) {
			CapsPermanent = true;
		}
		else if (BrIndex == 59) {
			Number = true;
		}
		else {
			if (Caps||CapsPermanent) {
				LCDWriting(0, BrailleABCaps[BrIndex]);
				Caps = false;
			}
			else if (Number) {
				LCDWriting(0, BrailleNumbers[BrIndex]);
			}
			else {
				LCDWriting(0, BrailleABC[BrIndex]);
			}
		}
	}
	else if (BrIndex == 1) {
		LCDWriting(2, '#');
	}
	else if (BrIndex == 2) {
		LCDWriting(3, '#');
	}
}

int RowNR = 0;
int PosNR = 0;
String LastWord = "";
char Typed[50][20];
int LineNr = 0;

void LCDWriting(int SEBack,char inPut) {
	if (SEBack == 1) {
		LastWord = "";
		lcd.print(" ");
		PosNR++;
		Typed[LineNr][PosNR] = ' ';
		Serial.print(" ");
	}
	else if (SEBack == 2) {
		RowNR++;
		LineNr++;
		PosNR = 0;
		LastWord = "";
		lcd.setCursor(PosNR, RowNR);
		Serial.println("");
	}
	else if (SEBack == 3) {
		Serial.print("BACK");
		PosNR--;
		LastWord.remove(LastWord.length() - 1);
		lcd.setCursor(PosNR, RowNR);
		lcd.print(" ");
		lcd.setCursor(PosNR, RowNR);
	}
	else {
		Serial.print(inPut);
		lcd.print(inPut);
		LastWord += inPut;
		PosNR++;
	}
	if (PosNR == LCDChar) {
		RowNR++;
		PosNR = 0;
		lcd.setCursor(PosNR, RowNR);
	}
	else if (PosNR == 1) {
		Serial.println("first position word");
		if (LastWord.length() > 1) {
			lcd.setCursor(LCDChar - int(LastWord.length()) + 1, RowNR - 1);
			for (int l = 1; l <= LastWord.length() - 1; l++) {
				lcd.print(" ");
			}
		}
		lcd.setCursor(0, RowNR);
		lcd.print(LastWord);
		PosNR = LastWord.length();
	}
	Serial.println(LastWord.length());
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