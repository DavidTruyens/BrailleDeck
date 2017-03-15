/*
 Name:		BrailleDeck.ino
 Created:	3/13/2017 1:34:26 PM
 Author:	David
*/

/*
Breadboard pin's (using the RS50 connector numbers)
10 GND
4 is pulled low when the board is connected (so it becomes a second gnd)

Perkings PinNr	->		Uno PinNr
	Key1 = 1	->		 4
	Key2 = 3	->		 5
	Key3 = 5	->		 7
	Key4 = 7	->		 9
	Key5 = 9	->		 11
	Key6 = 8	->		 10

These key's use the 4 pin as their GND
Enter = 1 (Shared with Key1)
Backspace = 3 (Shared with Key2)
Space = 6 (Not shared, uno  PinNr 8)

Pin 2 is not used

*/

int Spacepin = 8;
int Enterpin = 4;
int Backpin = 5;

int Keypins[6] = { 4,5,7,9,11,10 };
const int PowerTwo[6] = { 1,2,4,8,16,32 };

char BailleABC[73] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST";

void setup() {
	// put your setup code here, to run once:
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	Serial.begin(9600);

	for (int PinNr = 4; PinNr <= 11; PinNr++) {
		pinMode(PinNr, INPUT_PULLUP);
	}

	Serial.println("Waiting for connection");
	bool connected = false;
	int connectorcheck = HIGH;
	while (!connected) {
		connectorcheck = digitalRead(6);
		if (connectorcheck == HIGH) {
			Serial.print(".");
		}
		else {
			connected = true;
			digitalWrite(13, HIGH);
		}
	}
	Serial.println("");
	Serial.println("connected!");

}

bool Active = false;

int EnterPreviousState = HIGH;
int BackPreviousState = HIGH;

void loop() {
	//Check connection
	CheckConnection();

	//Check space, enter and backspace
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
			/*Serial.print("spacebuttonstate: ");
			Serial.println(SpaceButton);*/
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
		//Serial.print(".");
	}
	Active = false;

	//Serial.println("x");
	int BrailleIndex = 0;

	for (BrailleCounter = 0; BrailleCounter <= 5; BrailleCounter++) {
		//Serial.print(KeyRecordings[BrailleCounter]);
		BrailleIndex = BrailleIndex + KeyRecordings[BrailleCounter] * PowerTwo[BrailleCounter];
		/*Serial.print(",");
		Serial.println(BrailleIndex);*/
	}

	if (BrailleIndex == 0) {
		Serial.print(" ");
	}
	else if (SpaceRecording == 1) {
		BrailleIndex--;
		/*Serial.print("Braille index: ");
		Serial.println(BrailleIndex);*/
		Serial.print(BailleABC[BrailleIndex]);
	}
	else if (BrailleIndex==1){
		Serial.println("");		
	}
	else if (BrailleIndex == 2) {
		Serial.print("BACK");
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
void CheckConnection() {
	ConnectionChecker = digitalRead(6);
	if (ConnectionChecker == HIGH) {
		Serial.println("Connection Lost");
		digitalWrite(13, LOW);
	}
	else {
		digitalWrite(13, HIGH);
	}
}