#include <Arduino.h>
#include <Stepper.h>
#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial stickLink(3, 4);

bool isReverse = false;

unsigned long lastSonarTime = 0;
int currentDistance = 0;

const int STEPS = 2048;
Stepper myStepper(STEPS, 8, 10, 9, 11);
Servo myServo;
int steerAngle = 90;

int lastButtonState = HIGH;

int getDistanceCm() {
    digitalWrite(12, LOW);
    delayMicroseconds(2);
    digitalWrite(12, HIGH);
    delayMicroseconds(10);
    digitalWrite(12, LOW);

    unsigned long duration = pulseIn(13, HIGH, 15000);
    
    if (duration == 0) {
        return 0;
    }
    
    int distance = duration * 0.0343 / 2;
    return distance;
}

void setup() {
pinMode(2, INPUT_PULLUP);
Serial.begin(9600);
myServo.attach(6);
myServo.write(steerAngle);
stickLink.begin(9600);

pinMode(12, OUTPUT);
pinMode(13, INPUT);
}

void loop() {
    if (millis() - lastSonarTime >= 60) {
        lastSonarTime = millis();
        currentDistance = getDistanceCm();

        stickLink.write((uint8_t)constrain(currentDistance, 0, 255));

    Serial.print("Distance: ");
    Serial.print(currentDistance);
    Serial.println(" cm");
    }

if (stickLink.available() > 0) {
    int angle = stickLink.read();
    myServo.write(angle);
}
 int rawVal = analogRead(A0);
    int motorSpeed = map(rawVal, 0, 1023, 0, 17);
    int ButtonState = digitalRead(2);
    if (ButtonState == LOW && lastButtonState == HIGH) {
        isReverse = !isReverse;
        delay(50);
        if (isReverse) {
    Serial.println("GEAR: REVERSE (BACK)");
} else {
    Serial.println("GEAR: DRIVE (FORWARD)");
}
    } 
    lastButtonState = ButtonState;

    if (motorSpeed > 0) {
        myStepper.setSpeed(motorSpeed);

        if (isReverse == true){
            myStepper.step(-2); 
        } else if (currentDistance > 0 && currentDistance < 15){
                Serial.println("EMERGENCY STOP: OBSTACLE DETECTED");
                myStepper.step(0);
        } else {
            myStepper.step(2);
            }  
        }
    }

