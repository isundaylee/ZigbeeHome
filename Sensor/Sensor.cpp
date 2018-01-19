#include <Arduino.h>
#include <SoftwareSerial.h>

const int PIN_TX = 4;
const int PIN_RX = 3;

SoftwareSerial mySerial(PIN_RX, PIN_TX);

void setup() {
    mySerial.begin(9600);
    mySerial.println("===========================");
}

void loop() {
    int ch;

    while ((ch = mySerial.read()) != -1) {
        mySerial.write((char) ch);
    }
}
