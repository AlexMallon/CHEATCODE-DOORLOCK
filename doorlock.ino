#include <AccelStepper.h>

const int stepsPerRevolution = 2048;
const int halfTurn = stepsPerRevolution / 2;
const int rpm = 15;

AccelStepper stepper(AccelStepper::FULL4WIRE, 10, 12, 11, 13);  // IN1, IN3, IN2, IN4

const int xPin = A0;
const int yPin = A1;
const int redPin = 8;
const int greenPin = 9;
const int bluePin = 10;

const int codeLength = 8;
char cheatCode[codeLength] = {'U', 'U', 'D', 'D', 'L', 'R', 'L', 'R'};
char inputBuffer[codeLength];
int bufferIndex = 0;

bool isStepping = false;
bool goingForward = false;

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed((rpm * stepsPerRevolution) / 60);
  stepper.setAcceleration(1000);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setLEDColor(255, 0, 0);  // Idle

  for (int i = 0; i < codeLength; i++) inputBuffer[i] = ' ';
}

void loop() {
  if (isStepping) {
    stepper.run();

    if (!stepper.isRunning()) {
      if (goingForward) {
        // Return trip
        stepper.move(-halfTurn);
        goingForward = false;
      } else {
        // Finished both directions
        isStepping = false;
        setLEDColor(255, 0, 0);
      }
    }

    return;
  }

  // Read joystick
  char dir = getDirection(analogRead(xPin), analogRead(yPin));
  if (dir != 'N') {
    Serial.print("Input: ");
    Serial.println(dir);

    inputBuffer[bufferIndex] = dir;
    bufferIndex = (bufferIndex + 1) % codeLength;

    if (checkCheatCode()) {
      Serial.println("Cheat code accepted!");
      setLEDColor(0, 255, 0);
      stepper.move(halfTurn);     // Always forward first
      isStepping = true;
      goingForward = true;
    }

    delay(250); // debounce
  }
}

char getDirection(int x, int y) {
  if (y < 200) return 'U';
  if (y > 800) return 'D';
  if (x < 200) return 'L';
  if (x > 800) return 'R';
  return 'N';
}

bool checkCheatCode() {
  for (int i = 0; i < codeLength; i++) {
    int index = (bufferIndex + i) % codeLength;
    if (inputBuffer[index] != cheatCode[i]) return false;
  }
  return true;
}

void setLEDColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}
