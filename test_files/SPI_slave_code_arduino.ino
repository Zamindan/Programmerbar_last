#include <SPI.h>

volatile boolean dataReceived = false;
char receivedData[128]; // 128
volatile byte index = 0;
char slaveMessage[] = "Hello from Slave!";
volatile byte sendIndex = 0;

void setup() {
    Serial.begin(500000);
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE); // Enable SPI in slave mode
    SPI.attachInterrupt(); // Enable SPI interrupt
    SPDR = slaveMessage[sendIndex]; // Load first byte to send
}

ISR(SPI_STC_vect) {
    char c = SPDR; // Read received byte
    if (index < sizeof(receivedData) - 1) {
        receivedData[index++] = c;
        if (c == '\0') { // End of message
            dataReceived = true;
            index = 0;
        }
    } else {
        index = 0; // Reset on overflow
    }
    
    // Send next byte from slaveMessage
    sendIndex++;
    if (sendIndex >= sizeof(slaveMessage)) {
        sendIndex = 0; // Reset index if message is complete
    }
    SPDR = slaveMessage[sendIndex]; // Load next byte to send
}

void loop() {
    if (dataReceived) {
        Serial.print("Received: ");
        Serial.println(receivedData);
        dataReceived = false;
    }
}
