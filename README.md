# MechaRover: Powertrain & Active Collision Avoidance System 🏎️🛑

An autonomous embedded vehicular powertrain, steering kinematics, and active safety override system built with **Arduino Uno (ATmega328P)** and **M5StickC PLUS (ESP32)**.

---

## 🚀 Key Features & Architecture

### 1. ⚙️ Precision Stepper Propulsion & Analog Throttle
* **Actuator:** 28BYJ-48 5V Unipolar Stepper Motor with internal 64:1 planetary reduction (2048 steps/rev).
* **Driver:** ULN2003 4-phase Darlington transistor array.
* **Throttle Modulation:** Continuous analog ADC sampling (`analogRead(A0)`) dynamically mapping potentiometer voltage ($0 \dots 5\text{V}$) to motor rotational velocity ($0 \dots 17\text{ RPM}$).

### 2. 🔘 Tactile Gear Shifter (Drive / Reverse)
* **Input:** Tactile push button on Pin 2 utilizing internal hardware pull-up (`INPUT_PULLUP`).
* **State Machine:** Edge-detection transition logic (`ButtonState == LOW && lastButtonState == HIGH`) with software debouncing for seamless toggling between **Drive (Forward)** and **Reverse (Backward)** modes.

### 3. 🛡️ Active Collision Prevention (Tesla-style Auto-Brake)
* **Sensor:** HC-SR04 Ultrasonic Sonar (Trig: Pin 12, Echo: Pin 13).
* **Non-Blocking Telemetry:** Polled via asynchronous `millis()` timing loop to prevent CPU blocking and motor jitter.
* **Emergency Override:** If an obstacle is detected closer than **15 cm** while driving forward, the firmware instantly intercepts throttle control, halts motor steps (`step(0)`), and broadcasts an emergency alert while permitting reverse evacuation.

### 4. 🦾 IMU Gesture Teleoperated Steering
* **Actuator:** SG90 Micro Servo on Pin 6 (PWM).
* **Inter-Chip UART Bridge:** Receives live steering angle commands ($45^\circ \dots 135^\circ$) via `SoftwareSerial` transmitted from an M5StickC PLUS measuring 3D accelerometer gravity tilt (`accX`).

---

## 📌 Hardware Pinout & Wiring Map

| Component | Pin / Interface | Arduino Uno Pin | Description |
| :--- | :--- | :--- | :--- |
| **ULN2003 Driver** | `IN1, IN2, IN3, IN4` | `Pin 8, 9, 10, 11` | 4-Phase Stepper Coil Sequence |
| **Potentiometer** | Wiper (Signal) | `Pin A0` | Analog Throttle (0..1023) |
| **Shifter Button** | Switch Terminal | `Pin 2` | Digital Input (`INPUT_PULLUP`) |
| **SG90 Servo** | PWM Signal | `Pin 6` | Steering Angle ($45^\circ \dots 135^\circ$) |
| **HC-SR04 Sonar** | `TRIG` | `Pin 12` | 10µs Ultrasonic Pulse Burst |
| **HC-SR04 Sonar** | `ECHO` | `Pin 13` | Echo Return Pulse Timing |
| **M5Stick Link** | `TX (G26)` ➔ `RX` | `Pin 3` | UART Serial Telemetry Link (9600 baud) |
| **Power Rail** | `5V` & `GND` | `5V` & `GND` | Common Power Bus & Ground |

---

## 🧠 Core Autonomous Safety Algorithm

```cpp
// Non-blocking distance sampling via millis()
if (millis() - lastSonarTime >= 100) {
    lastSonarTime = millis();
    currentDistance = getDistanceCm();
}

// Drive & Collision Prevention Logic
if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);

    if (isReverse == true) {
        myStepper.step(-2); // Reverse is always permitted
    } else if (currentDistance > 0 && currentDistance < 15) {
        Serial.println("EMERGENCY STOP: OBSTACLE DETECTED");
        myStepper.step(0);  // Active brake override
    } else {
        myStepper.step(2);  // Road clear: forward drive
    }
}
🛠 Tech Stack
Languages: C++ (Embedded)
Frameworks: Arduino Core / PlatformIO / VS Code
Hardware: Arduino Uno R3, ESP32-PICO (M5StickC Plus), 28BYJ-48 Stepper, SG90 Servo, HC-SR04 Sonar, ULN2003.
Protocols & Drivers: Hardware PWM, Software UART, ADC Sampling, Microsecond Pulse Timing, Multi-tasking without delay().
📄 License
MIT
