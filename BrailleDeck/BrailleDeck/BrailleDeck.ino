/*
 Name:		BrailleDeck.ino
 Created:	3/13/2017 1:34:26 PM
 Author:	David
*/

/*
Breadboard pin's (using the RS50 connector numbers)
10 GND
4 is pulled low when the board is connected (so it becomes a second gnd)
Key1 = 1
Key2 = 3
Key3 = 5
Key4 = 7
Key5 = 9
Key6 = 8

These key's use the 4 pin as their GND
Enter = 1
Space = 6
Backspace = 3

Pin 2 is not used

Perkings to Uno Pins
1 - 4
3 - 5
4 - 6
5 - 7
6 - 8
7 - 9
8 - 10
9 - 11
10 - GND
*/

int Keys[8] = {4,5,6,7,8,9,10,11};
int ButtonStates[9] = { 0,0,0,0,0,0,0,0 };

unsigned long seconds = 0;
bool Debug = false;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	
	for (int PinNr = 4; PinNr <= 11; PinNr++) {
		pinMode(PinNr, INPUT_PULLUP);
	}

	Serial.println("Waiting for connection");
	bool connected = false;
	int connectorcheck = HIGH;
	while (!connected) {
		connectorcheck=digitalRead(6);
		if (connectorcheck == HIGH) {
			Serial.print(".");
		}
		else {
			connected = true;
		}
	}
	Serial.println("");
	Serial.println("connected!");

	//for (int State = 1; State <= 8; State++) {
	//	ButtonStates[State] = digitalRead(Keys[State]);
	//	Serial.print(ButtonStates[State]);
	//}

	//seconds = millis();

}

unsigned long timelapse;
int BtPressed[9] = { 0,0,0,0,0,0,0,0 };
bool ButtonPressed = false;

char BailleABC[73] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST";
int Recordings[6] = { 0,0,0,0,0,0 };
int Recording = 0;
int Recordcheck = 0;

void loop() {
	//Check connection
	CheckConnection();

	//Check space, enter and backspace

	//if space check for other buttons

	//if number record number

	//Recording = CheckButtons();
	//// record buttons
	//while (Recording == 1) {
	//	ButtonPressed = true;
	//	for (int State = 1; State < 9; State++) {
	//		ButtonStates[State] = digitalRead(Keys[State]);
	//		if (ButtonStates[State] = 0) {
	//			Recordings[State] = 1;
	//		}
	//		else {
	//			Recordcheck = 0;
	//		}
	//	}

	//	if (Recordcheck == 0) {
	//		Recording = 0;
	//	}
	//}

	//int Index = 0;

	//if (ButtonPressed) {
	//	ButtonPressed = false;
	//	//enter
	//	if (Recordings[7] == 1) {
	//		Serial.println("");
	//	}
	//	//backspace
	//	else if (Recordings[8] == 1) {
	//		Serial.print("backspace");
	//	}
	//	else {
	//		for (int j = 1; j < 7; j++) {
	//			Index = Index + Recordings[j] * pow(2, j);
	//			Recordings[j] = 0;
	//		}
	//		if (Index == 0) {
	//			Serial.print(" ");
	//		}
	//		else {
	//			Serial.print(BailleABC[Index]);
	//		}
	//	}
	//}

	//// debug
	//timelapse = millis() - seconds;
	//if (timelapse >= 1000) {
	//	seconds = millis();
	//}
}

int PressCheck = 0;
int CheckButtons() {
	// check buttons
	for (int State = 1; State < 9; State++) {
		ButtonStates[State] = digitalRead(Keys[State]);
		if (ButtonStates[State] == 0) {
			PressCheck = 1;
			break;
		}
	}
	return PressCheck;
}

int ConnectionChecker = HIGH;
void CheckConnection() {
	ConnectionChecker = digitalRead(6);
	if (ConnectionChecker==HIGH) {
		Serial.println("Connection Lost");
	}
}
void DebugPrint(String msg) {
	if (Debug) {
		Serial.print(msg);
	}
}

void DebugPrintLN(String msg) {
	if (Debug) {
		Serial.println(msg);
	}
}