const int tengPin = A0;       // Analog input
const int outputPin = 7;      // Digital output pin
const int threshold = 300;    // Adjust based on your TENG output dip

// Cooldown to prevent repeated triggers
const unsigned long cooldown = 300;
unsigned long lastTriggerTime = 0;

bool latchState = LOW;

// Reads minimum value over 32 samples
int get_min_smoothed() {
  int minValue = 1023;

  for (int i = 0; i < 32; i++) {
    int reading = analogRead(tengPin);
    if (reading < minValue) {
      minValue = reading;
    }
    delay(1);  // Small delay between samples
  }

  return minValue;
}

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
}

void loop() {
  int minValue = get_min_smoothed();
  unsigned long now = millis();

  // Trigger only if minValue drops below threshold and cooldown has passed
  if (minValue < threshold && (now - lastTriggerTime > cooldown)) {
    latchState = HIGH;
    lastTriggerTime = now;
  }

  // Keep pulse HIGH for 50ms
  if ((now - lastTriggerTime) > 50) {
    latchState = LOW;
  }

  digitalWrite(outputPin, latchState);

  // Serial Plotter output
  Serial.print(minValue);
  Serial.print('\t');
  Serial.println(threshold);

  delay(5);  // ~200Hz loop
}
