#include <Servo.h>

// ===== Servo Pin Assignments =====
#define servo_shoulder  3
#define servo_elbow     4
#define servo_wrist     5
#define servo_wrist_rotation  6
#define servo_gripper   7

#define comparator_pin A1

// Servo objects
Servo myservoshoulder;
Servo myservoelbow;
Servo myservowrist;
Servo myservowristrotation;
Servo myservogripper;

// Servo values from GUI
unsigned char servo[6];
int flag = 0;
const int BUFFER_SIZE = 10;
unsigned char buf[BUFFER_SIZE];

// EMG system
const int emgPin = A0;

int emgDelayCounter = 0;
const int emgDelayDuration = 100;  // ~1 second (10ms * 100)

bool latch1 = false;
bool latch2 = false;

int cooldown1 = 0;
const int cooldownLimit1 = 20;

int cooldown2 = 0;
const int cooldownLimit2 = 20;

int latch1Lockout = 0;
const int latch1LockoutDuration = 50;

// Gripper debounce system
bool gripperOpen = false;
int comparatorStableCounter = 0;
const int comparatorStableLimit = 10; // 10 loops ≈ 100ms stability needed

void setup() {
  myservoshoulder.attach(servo_shoulder); 
  myservoelbow.attach(servo_elbow);
  myservowrist.attach(servo_wrist);
  myservowristrotation.attach(servo_wrist_rotation);
  myservogripper.attach(servo_gripper);

  // Default positions
  myservoshoulder.write(130);
  myservoelbow.write(125);
  myservowrist.write(90);
  myservowristrotation.write(90);
  myservogripper.write(90);

  pinMode(comparator_pin, INPUT);
  Serial.begin(115200);
}

void clear_serial_buffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buf[i] = '\0';
  }
}

void update_servos_from_gui() {
  myservowrist.write(servo[3]);
  myservowristrotation.write(servo[4]);
  myservogripper.write(servo[5]);
}

int get_smoothed_values() {
  int num = 16;
  int total = 0;
  while (num--) {
    total += analogRead(emgPin);
    delay(1);
  }
  return total >> 4;
}

void loop() {
  // Read Serial Data
  while (Serial.available() > 0) {
    delay(5);
    Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
    flag = 1;
  }

  if (flag == 1) {
    flag = 0;
    for (int i = 0; i < 6; i++) {
      servo[i] = map(buf[i], 0, 128, 0, 180);
    }
    clear_serial_buffer();
    update_servos_from_gui();
  }

  // === EMG Logic ===
  int averageValue = get_smoothed_values();

  if (cooldown1 > 0) cooldown1--;
  if (cooldown2 > 0) cooldown2--;
  if (latch1Lockout > 0) latch1Lockout--;

  // Read comparator early
  int comparatorValue = digitalRead(comparator_pin);

  // === Gripper Stability Filter ===
  if (comparatorValue == HIGH) {
    if (comparatorStableCounter < comparatorStableLimit) comparatorStableCounter++;
  } else {
    if (comparatorStableCounter > 0) comparatorStableCounter--;
  }

  // Update gripperOpen status
  if (comparatorStableCounter >= comparatorStableLimit) {
    gripperOpen = true;
  } else if (comparatorStableCounter == 0) {
    gripperOpen = false;
  }

  // Reset EMG delay when comparator is HIGH (hand opening)
  if (comparatorValue == LOW) {
    emgDelayCounter = emgDelayDuration;
  }

  // Reduce EMG delay counter every loop
  if (emgDelayCounter > 0) emgDelayCounter--;

  // === Latch2 Logic ===
  if (cooldown2 == 0) {
    if ((averageValue >= 315 && averageValue <= 400) || (averageValue >= 100 && averageValue <= 285)) {
      latch2 = !latch2;
      cooldown2 = cooldownLimit2;
      latch1Lockout = latch1LockoutDuration;
    }
  }

  // === Corrected Latch1 Logic ===
  if (cooldown1 == 0 && latch1Lockout == 0 && comparatorValue == HIGH && emgDelayCounter == 0) {
    if (!latch1 && averageValue >= 305 && averageValue <= 315) {
      latch1 = true;
      cooldown1 = cooldownLimit1;
    } else if (latch1 && averageValue >= 285 && averageValue <= 295) {
      latch1 = false;
      cooldown1 = cooldownLimit1;
    }
  }

  // === Servo Control ===

  // Gripper controlled by stable signal
  if (gripperOpen) {
    myservogripper.write(100);  // Open
  } else {
    myservogripper.write(150); // Close
  }

  // Wrist rotation (latch1)
  if (latch1) {
    myservowristrotation.write(0);
  } else {
    myservowristrotation.write(180);
  }

  // Wrist bending (latch2)
  if (latch2) {
    myservowrist.write(90);
  } else {
    myservowrist.write(55);
  }

  // === Plot EMG average only ===
  Serial.print(averageValue);
  Serial.print('\t');
  Serial.print(250);
  Serial.print('\t');
  Serial.println(350);

  delay(10);
}
