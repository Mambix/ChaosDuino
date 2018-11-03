#include "BIP39.h"

#define VERSION "0.1.3"
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
uint8_t adcMode = 1;
bool bOK = false;
bool bForceFill = false;
uint8_t words = 24;

int tmpVal;
int val;
int val2 = 0;
int val3;

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

    adc_set_prescaler(ADC_PRE_PCLK2_DIV_2);
//    adc_set_prescaler(ADC_PRE_PCLK2_DIV_4);
//    adc_set_prescaler(ADC_PRE_PCLK2_DIV_6);
//    adc_set_prescaler(ADC_PRE_PCLK2_DIV_8);
    adc_set_sample_rate(ADC1, ADC_SMPR_1_5);
//    adc_set_sample_rate(ADC1, ADC_SMPR_1_5);
    adc_enable_single_swstart(ADC1);

    Serial.begin();
    // wait for serial monitor to be connected.
    while (!(Serial.isConnected() && (Serial.getDTR() || Serial.getRTS())))
    {
        // Turn the LED from off to on, or on to off
        digitalWrite(rLED, !digitalRead(rLED));
        delay(25); // fast blink
    }
    forceFill();
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

void dump() {
    Serial.write(pool, POOL_SIZE);
    digitalWrite(rLED, HIGH);
    bForceFill = true;
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
        } else if (cmd == "ATV") {
            Serial.print("ChaosDuino v");
            Serial.print(VERSION);
            Serial.println(" for PCB rev3 running...");
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
        } else if (cmd == "ATP0") {
                // binary mode; DEFAULT
                bMode = 0;
        } else if (cmd == "ATP1") {
                // string HEX mode
                bMode = 1;
        } else if (cmd == "ATP2") {
                // string DEC mode
                bMode = 2;
        } else if (cmd == "ATP?") {
                // what mode are we using
                Serial.print("ATP");
                Serial.println(bMode, DEC);
        } else if (cmd == "ATM0") {
                adcMode = 0;
                bForceFill = true;
        } else if (cmd == "ATM1") {
                adcMode = 1;
                bForceFill = true;
        } else if (cmd == "ATM2") {
                adcMode = 2;
                bForceFill = true;
        } else if (cmd == "ATM3") {
                adcMode = 3;
                bForceFill = true;
        } else if (cmd == "ATM4") {
                adcMode = 4;
                bForceFill = true;
        } else if (cmd == "ATM5") {
                adcMode = 5;
                bForceFill = true;
        } else if (cmd == "ATM6") {
                adcMode = 6;
                bForceFill = true;
        } else if (cmd == "ATM7") {
                adcMode = 7;
                bForceFill = true;
        } else if (cmd == "ATM?") {
                // what mode are we using
                Serial.print("ATM");
                Serial.println(adcMode, DEC);
        } else if (cmd == "ATV2") {
                Serial.print(val2);
        } else if (cmd == "ATV3") {
                Serial.print(val3);
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
        } else if (cmd == "ATDUMP") {
                // Dump all bytes from pool
                dump();
        } else if (cmd == "ATPOOL?") {
                // How many bytes in pool?
                int bytes = tail;
                if (tail<=head) {
                    bytes += POOL_SIZE;
                }
                bytes -= head;
                Serial.println(bytes, DEC);
        } else if (cmd == "BIP39W15") {
                words = 15;
        } else if (cmd == "BIP39W18") {
                words = 18;
        } else if (cmd == "BIP39W21") {
                words = 21;
        } else if (cmd == "BIP39W24") {
                words = 24;
        } else if (cmd == "BIP39") {
                // How many bytes in pool?
                int bytes = tail;
                if (tail<=head) {
                    bytes += POOL_SIZE;
                }
                bytes -= head;
                if (bytes<words) {
                    Serial.println("ERROR");
                } else {
                    for (int i=0; i<words; i++) {
                        int sum=0;
                        for (int w=0; w<4; w++) {
                            sum += readRawADC() & 0xff;
                        }
                        Serial.print(BIP39[sum]);
                        Serial.print(' ');
                    }
                    Serial.println();
                }
        } else if (cmd == "BIP39W?") {
                Serial.print("BIP39W");
                Serial.println(words, DEC);
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

uint8_t readADC() {
    switch (adcMode) {
        case 0: return readADC_1A();
        case 1: return readADC_1B();
        case 2: return readADC_1C();
        case 3: return readADC_1D();
        case 4: return readADC_1E();
        case 5: return readADC_1F();
        case 6: return readADC_1G();
        case 7: return readADC_1H();
    }
}

uint16_t readRawADC() {
    ADC1->regs->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->regs->SR & ADC_SR_EOC)) {}
    return (uint16_t)(ADC1->regs->DR & ADC_DR_DATA);
//    return analogRead(BitStream);
}

uint8_t readADC_1A() {
    val = readRawADC();
    return val & 0xff;
}

uint8_t readADC_1B() {
    val = readRawADC();
    tmpVal = val;
    val ^= val2;
    val ^= val3;
    val2 = val2 << 1;
    val3 = val3 >> 1;
    if (tmpVal & 0x100 == 0x100) {
        val2 = readRawADC();
    }
    if (tmpVal & 0x200 == 0x200) {
        val3 = readRawADC();
    }
    return val & 0xff;
}

uint8_t readADC_1C() {
    val = readRawADC();
    tmpVal = val;
    val ^= val2;
    val2 = val2 << 1;
    if (tmpVal & 0x100 == 0x100) {
        val2 |= 0x0001;
    }
    return val & 0xff;
}

uint8_t readADC_1D() {
    val = readRawADC();
    tmpVal = val;
    val ^= val2;
    val2 = val2 << 1;
    if (tmpVal & 0x200 == 0x200) {
        val2 |= 0x0001;
    }
    return val & 0xff;
}

uint8_t readADC_1E() {
    val = readRawADC();
    tmpVal = val;
    val ^= val2;
    val2 = val2 << 1;
    if (tmpVal & 0x400 == 0x400) {
        val2 |= 0x0001;
    }
    return val & 0xff;
}

uint8_t readADC_1F() {
    val = readRawADC();
    tmpVal = val;
    val ^= val2;
    val2 = val2 << 1;
    if (tmpVal & 0x400 == 0x400) {
        val2 ^= tmpVal >> 8;
    }
    return val & 0xff;
}

uint8_t readADC_1G() {
    val = readRawADC();
    tmpVal = val;
    val ^= val2;
    val2 = val2 << 4;
    val2 |= tmpVal >> 8;
    return val & 0xff;
}

uint8_t readADC_1H() {
    val = readRawADC();
    tmpVal = val;
    val ^= val2;
    val2 = val2 << 2;
    val2 |= (tmpVal >> 8) & 0x03;
    return val & 0xff;
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
            pool[head++] = readADC();
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
    bForceFill = false;
    digitalWrite(Entropy, HIGH);
    digitalWrite(gLED, HIGH);
    digitalWrite(rLED, LOW);
    if (bEntropyOff) {
        delay(100);
    }
    for (int i = 0; i < POOL_SIZE; i++)
    {
          pool[i] = readADC();
    }
    digitalWrite(gLED, LOW);
    if (bEntropyOff) {
        digitalWrite(Entropy, LOW);
    }
}
