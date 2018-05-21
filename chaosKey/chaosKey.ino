const int PinEN = 5;
const int InputPin = 15;

uint8_t tmp;
int val;

void setup() {
  pinMode(PinEN, OUTPUT);
  Serial.begin(250000);

  digitalWrite(PinEN, HIGH);
}

void loop() {
  val = analogRead(0);
  tmp = val & 0xff;
  Serial.write(tmp);
}
