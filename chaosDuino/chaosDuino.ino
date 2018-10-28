#define rLED PB6
#define gLED PB5
#define bLED PB4
#define Entropy PA1
#define BitStream 0
#define POOL_SIZE 10000

char c;
String cmd = "";
uint8_t pool[POOL_SIZE] = {0};
int head = 0;
int tail = 0;
uint32_t cnt = 0;
uint16_t tmpCnt = 0;

bool bEntropyOff = true;
uint8_t bMode = 0;
bool bOK = false;
bool bForceFill = false;

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
    // wait for serial monitor to be connected.
    while (!(Serial.isConnected() && (Serial.getDTR() || Serial.getRTS())))
    {
        // Turn the LED from off to on, or on to off
        digitalWrite(rLED, !digitalRead(rLED));
        delay(25); // fast blink
    }
    Serial.println("ChaosDuino v0.1.0 for PCB rev3 running...");
    delay(100);
    forceFill();
    if (bEntropyOff) {
        digitalWrite(Entropy, LOW);
    }
}

// the loop function runs over and over again forever
void loop() {
    readSerial();
    if (bForceFill) {
        forceFill();
    } else {
        fillEntropyPool();
    }
    if (tmpCnt++ == 0){
        if (cnt++ >= 16) {
            cnt = 0;
            digitalWrite(bLED, HIGH);
        }
        if (cnt > 0) {
            digitalWrite(bLED, LOW);
        }
    }
}

void print(int cnt) {
    uint8_t c;
    if (bMode == 1) {
        Serial.print("0x");
    }
    for (int i=0; i<cnt; i++) {
        c = pool[tail++];
        if (bMode == 2) {
            Serial.print(c);
            if (i == (cnt-1)) {
                Serial.println();
            }
        }
        if (bMode == 1) {
            Serial.print(c, HEX);
            if (i == (cnt-1)) {
                Serial.println();
            }
        }
        if (bMode == 0) {
            Serial.write(c);
        }
        if (tail == POOL_SIZE) {
            tail = 0;
        }
        if (tail == head) {
            // OUT of entropy; exit
            digitalWrite(rLED, HIGH);
            bForceFill = true;
            if (cnt != POOL_SIZE) {
                Serial.println("EMPTY");
            }
            return;
        }
    }
}

void printSHA() {
    uint8_t c;
    for (int i=0; i<32; i++) {
        c = pool[tail++];
        Serial.print(c, HEX);
        if (tail == POOL_SIZE) {
            tail = 0;
        }
        if (tail == head) {
            // OUT of entropy; exit
            digitalWrite(rLED, HIGH);
            Serial.println("EMPTY");
            return;
        }
    }
    Serial.println();
}

void parseCommand() {
    if (cmd != "") {
        bool OK = bOK;
        if (cmd == "AT") {
            Serial.println("AT");
        } else if (cmd == "ATLR0") {
                // Red LED off
                digitalWrite(rLED, LOW);
        } else if (cmd == "ATLR1") {
                // Red LED on
                digitalWrite(rLED, HIGH);
        } else if (cmd == "ATLG0") {
                // Green LED off
                digitalWrite(gLED, LOW);
        } else if (cmd == "ATLG1") {
                // Green LED on
                digitalWrite(gLED, HIGH);
        } else if (cmd == "ATLB0") {
                // Blue LED off
                digitalWrite(bLED, LOW);
        } else if (cmd == "ATLB1") {
                // Blue LED on
                digitalWrite(bLED, HIGH);
        } else if (cmd == "ATE0") {
                // Entropy auto
                digitalWrite(Entropy, LOW);
                bEntropyOff = true;
        } else if (cmd == "ATE1") {
                // Entropy always on
                digitalWrite(Entropy, HIGH);
                bEntropyOff = false;
        } else if (cmd == "ATE?") {
                // what mode are we using
                Serial.print("ATE");
                if (bEntropyOff) {
                    Serial.println("1");
                } else {
                    Serial.println("0");
                }
        } else if (cmd == "ATM0") {
                // binary mode; DEFAULT
                bMode = 0;
        } else if (cmd == "ATM1") {
                // string HEX mode
                bMode = 1;
        } else if (cmd == "ATM2") {
                // string DEC mode
                bMode = 2;
        } else if (cmd == "ATM?") {
                // what mode are we using
                Serial.print("ATM");
                uint8_t m = 48 + bMode;
                Serial.write(m);
                Serial.println();
        } else if (cmd == "ATOK0") {
                // don't OK on success, only return Data
                bOK = false;
        } else if (cmd == "ATOK1") {
                // OK on success after data is returned
                bOK = true;
        } else if (cmd == "ATOK?") {
                // is it on/off?
                Serial.print("ATOK");
                if (bOK) {
                    Serial.println("1");
                } else {
                    Serial.println("0");
                }
        } else if (cmd == "ATC") {
                // Coin flip, 1=Head, 0=Tail
        } else if (cmd == "ATB") {
                // Return single byte from pool
                print(1);
        } else if (cmd == "ATI") {
                // Return single integer
                print(2);
        } else if (cmd == "ATL") {
                // Return single long
                print(4);
        } else if (cmd == "ATSHA256") {
                // Return 32 bytes from pool in hex
                printSHA();
        } else if (cmd == "ATPOOL") {
                // Dump all bytes from pool
                print(POOL_SIZE);
        } else if (cmd == "ATPOOL?") {
                // How many bytes in pool?
                int bytes = tail;
                if (tail<=head) {
                    bytes += POOL_SIZE;
                }
                bytes -= head;
                Serial.println(bytes);
        } else {
                // No idea what you send me!
                Serial.println("ERROR");
                OK = false;
        }
        if (OK) {
            Serial.println("OK");
        }
    }
}

void readSerial() {
    if (Serial.available()) {
        c = Serial.read();
        if (c == 13) {
            parseCommand();
            cmd = "";
        } else {
            cmd.concat(c);
        }
    }
}

void fillEntropyPool() {
    int t = tail;
    if (t < head)
    {
        t += POOL_SIZE;
    }
    if (head < t)
    {
        bForceFill = false;
        digitalWrite(Entropy, HIGH);
        digitalWrite(gLED, HIGH);
        digitalWrite(rLED, LOW);
        if (bEntropyOff) {
            delay(100);
        }
        while (head < t)
        {
            pool[head++] = analogRead(BitStream) & 0xff;
            if (head == POOL_SIZE)
            {
                head = 0;
                t -= POOL_SIZE;
            }
        }
        digitalWrite(gLED, LOW);
        if (bEntropyOff) {
            digitalWrite(Entropy, LOW);
        }
    }
}

void forceFill() {
    head = 0;
    tail = 0;
    for (int i = 0; i < POOL_SIZE; i++)
    {
        pool[i] = analogRead(BitStream) & 0xff;
    }
}