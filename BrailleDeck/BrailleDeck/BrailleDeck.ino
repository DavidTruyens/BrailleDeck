/*
 Name:		BrailleDeck.ino
 Created:	3/13/2017 1:34:26 PM
 Author:	David
*/

int Keys[9] = { 2, 3, 4, 5, 6, 7, 8,9,10 };
int ButtonStates[9] = { 0,0,0,0,0,0,0,0 };

unsigned long seconds = 0;
bool Debug = false;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	for (int PinNr = 1; PinNr < 9; PinNr++) {
		pinMode(Keys[PinNr], INPUT_PULLUP);
	}

	for (int State = 1; State < 9; State++) {
		ButtonStates[State] = digitalRead(Keys[State]);
		Serial.print(ButtonStates[State]);
	}

	seconds = millis();

}

unsigned long timelapse;
int BtPressed[9] = { 0,0,0,0,0,0,0,0 };
bool ButtonPressed = false;

char BailleABC[73] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST";
int Recordings[9] = { 0,0,0,0,0,0,0 };
int Recording = 0;
int Recordcheck = 0;

void loop() {
	//Check space, enter and backspace

	//if space check for other buttons

	//if number record number

	Recording = CheckButtons();
	// record buttons
	while (Recording == 1) {
		ButtonPressed = true;
		for (int State = 1; State < 9; State++) {
			ButtonStates[State] = digitalRead(Keys[State]);
			if (ButtonStates[State] = 0) {
				Recordings[State] = 1;
			}
			else {
				Recordcheck = 0;
			}
		}

		if (Recordcheck == 0) {
			Recording = 0;
		}
	}

	int Index = 0;

	if (ButtonPressed) {
		ButtonPressed = false;
		//enter
		if (Recordings[7] == 1) {
			Serial.println("");
		}
		//backspace
		else if (Recordings[8] == 1) {
			Serial.print("backspace");
		}
		else {
			for (int j = 1; j < 7; j++) {
				Index = Index + Recordings[j] * pow(2, j);
				Recordings[j] = 0;
			}
			if (Index == 0) {
				Serial.print(" ");
			}
			else {
				Serial.print(BailleABC[Index]);
			}
		}
	}

	// debug
	timelapse = millis() - seconds;
	if (timelapse >= 1000) {
		seconds = millis();
	}
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

void DebugPrint(String msg) {
	if (Debug) {
		Serial.print(msg);
	}
}

void DebugPrintLN(String msg) {
	if (Debug) {
		Serial.println(msg);
	}
}+-