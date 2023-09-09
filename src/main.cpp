#include <Arduino.h>
#include <RadioLib.h>
#include "Point.h"

// PIN configuration
#define NSS_PIN   5
#define DIO1_PIN  21
#define NRST_PIN  15
#define BUSY_PIN  2

// Module ID
#define ID_MASTER           -1
#define ID_SLAVE_A           0
#define ID_SLAVE_B           1
#define ID_SLAVE_C           2

#define ID                  -1
#define ID_TO_CHAR(x)       (char)('A' + x)

// Addressing
#define BASE_ADDRESS        0x01234567
#define ADDRESS_SLAVE       (BASE_ADDRESS + ID)
#define ADDRESS_MASTER(x)   (BASE_ADDRESS + x)

// RadioLib SX1280 module
SX1280 radio = new Module(NSS_PIN, DIO1_PIN, NRST_PIN, BUSY_PIN);


void setup() {
    Serial.begin(9600);
    delay(500);

    int state = radio.begin();

    if (state == RADIOLIB_ERR_NONE) {
        Serial.printf("[SX1280] Inicializacija uspesna!\n");
    } else {
        Serial.printf("[SX1280] Napaka pri inicializaciji, koda %d\n", state);
        while (true);
    }
}


#if ID == -1    // master

Point A {  0.00f,  0.00f};
Point B {128.00f,  0.00f};
Point C {128.00f, 89.40f};

DataManipulator dm;
DataPoint average{0.0f};
DataPoint deviation{0.0f};

unsigned int counter = 0;
const unsigned int printFreq = 10;

void loop() {
    static union {
        DataPoint ranges{0.0f};
        float ra, rb, rc;
    };

    average = dm.getAverage();
    deviation = dm.getStandardDeviation();

    for(int i = 0; i < 3; i++) {
        bool master = true;
        uint32_t address = ADDRESS_MASTER(i);

        int state = radio.range(master, address);

        if (state == RADIOLIB_ERR_NONE) {
            ranges[i] = radio.getRangingResult();
            if(counter % printFreq == 0)
                Serial.printf("Razdalja %c:  \t%.3f m\t\t%.3f m\t\t%.3f m\n", ID_TO_CHAR(i), ranges[i], average[i], deviation[i]);
        } else if (state == RADIOLIB_ERR_RANGING_TIMEOUT) {
            if(counter % printFreq == 0)
                Serial.printf("Razdalja %c:  \t   /\t\t%.3f m\t\t%.3f m\n", ID_TO_CHAR(i), average[i], deviation[i]);
        } else {
            Serial.printf("Razdalja %c: napaka, koda %d\n", ID_TO_CHAR(i), state);
        }

        // reset the module
        radio.reset();
        state = radio.begin();

        if (state != RADIOLIB_ERR_NONE) {
            Serial.printf("Napaka pri ponovnem zagonu, koda %d\n", state);
            while (true);
        }

        delay(500);
    }

    // save data
    dm.push(ranges);

    // trilaterate
    Point p = trilaterate(A, B, C, ra, rb, rc);
    Point pavg = trilaterate(A, B, C, average[0], average[1], average[2]);
    if(counter % printFreq == 0) {
        Serial.printf("Lokacija(%d): \t\t%s\n", counter, p.toString());
        Serial.printf("Avg. Lokacija: \t\t%s\n\n", pavg.toString());
    }

    counter++;
}

#else   // slave
void loop() {
    bool master = false;
    uint32_t address = ADDRESS_SLAVE;
    // Serial.println(address);

    int state = radio.range(master, address);

    if (state == RADIOLIB_ERR_NONE) {
        // Serial.println(F("uspeh!"));
    } else if (state == RADIOLIB_ERR_RANGING_TIMEOUT) {
        // Serial.println(F("timed out!"));
    } else {
        // Serial.print(F("napaka, koda "));
        // Serial.println(state);
    }

    delay(200);
}
#endif
