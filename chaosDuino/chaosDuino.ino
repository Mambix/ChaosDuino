#define rLED PB6
#define gLED PB5
#define bLED PB4
#define Entropy PA1
#define BitStream 0

uint8_t BitStreamValues[] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(rLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(bLED, OUTPUT);
  pinMode(Entropy, OUTPUT);
  
  digitalWrite(rLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(bLED, LOW);
  digitalWrite(Entropy, HIGH);

  Serial.begin();
  Serial.begin();  // BAUD has no effect on USB serial: placeholder for physical UART
    // wait for serial monitor to be connected.
    while (!(Serial.isConnected() && (Serial.getDTR() || Serial.getRTS()))) {
      // Turn the LED from off to on, or on to off
      digitalWrite(bLED, !digitalRead(bLED));
      delay(50);         // fast blink
  }
  Serial.println("ChaosDuino rev3...");
//  analogReadResolution(12);
}

// the loop function runs over and over again forever
void loop() {
  readEntropy();
  Serial.write(BitStreamValues, 16);
//  for (int i=0; i<16; i++) {
//    checkValue(BitStreamValues[i] & 0xff);
//    Serial.print((uint8_t)(BitStreamValues[i] & 0xff));
////    Serial.print(BitStreamValues[i] & 0xff, HEX);
////    Serial.print(' ');
//  }
//  Serial.println(' ');
}

void readEntropy() {
  digitalWrite(bLED, HIGH);
  for (int i=0; i<16; i++) {
    BitStreamValues[i] = analogRead(BitStream) & 0xff;
  }
  digitalWrite(bLED, LOW);
}

void checkValue(int val) {
  if (val == 0xff) {
    digitalWrite(rLED, HIGH);
  }
  if (val == 0x00) {
    digitalWrite(rLED, LOW);
  }

  if (val == 0x80) {
    digitalWrite(gLED, !digitalRead(gLED));
  }
}
