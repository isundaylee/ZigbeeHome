#include <Arduino.h>
#include <SoftwareSerial.h>

const int PIN_TX = 13;
const int PIN_RX = 12;

SoftwareSerial mySerial(PIN_RX, PIN_TX);

void setup() {
    mySerial.begin(9600);
    mySerial.println("Hello, world!");
}

void loop() {
    int ch;

    while ((ch = mySerial.read()) != -1) {
        mySerial.write((char) ch);
    }
}
