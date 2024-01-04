#include <Arduino.h>
#include <PS2MouseHandler.h>
#include <Wire.h>

#define MOUSE_DATA 5
#define MOUSE_CLOCK 6

#define LEFT_BUTTON 0
#define MIDDLE_BUTTON 1
#define RIGHT_BUTTON 2

#define DATA1_PIN 9
#define DATA2_PIN 8
#define DATA3_PIN 7
#define CLOCK_PIN 13

#define COL_NUM 3
#define ROW_NUM 4

PS2MouseHandler mouse(MOUSE_CLOCK, MOUSE_DATA, PS2_MOUSE_REMOTE);

constexpr int rows[ROW_NUM] = { 28, 27, 26, 25 };
constexpr int columns[COL_NUM] = { 24, 23, 22 };

const char keys[ROW_NUM][COL_NUM] = {
    '1', '2', '3',
    '4', '5', '6',
    '7', '8', '9',
    '*', '0', '#',
};

volatile bool interrupt = false;
uint32_t dataBuffer;

struct Data {
    int16_t x;
    int8_t y;
    uint8_t flags;
};
constexpr int UP = 5;
constexpr int DOWN = 4;
constexpr int LEFT = 3;
constexpr int RIGHT = 2;

void send(uint32_t value);
uint32_t getMouseData();
void readKeys(Data& data);

void setup() {
    Serial.begin(9600);

    for(auto c: columns) {
        pinMode(c, OUTPUT);
        digitalWrite(c, HIGH);
    }

    for(auto r: rows) {
        pinMode(r, INPUT_PULLUP);
    }

    pinMode(DATA1_PIN, OUTPUT);
    digitalWrite(DATA1_PIN, LOW);
    pinMode(DATA2_PIN, OUTPUT);
    digitalWrite(DATA2_PIN, LOW);
    pinMode(DATA3_PIN, OUTPUT);
    digitalWrite(DATA3_PIN, LOW);

    pinMode(CLOCK_PIN, OUTPUT);
    digitalWrite(CLOCK_PIN, LOW);

    attachInterrupt(digitalPinToInterrupt(2), [](){
        interrupt = true;
    }, FALLING);

    if(mouse.initialise()){
        Serial.println("Mouse error");
    }

    Serial.println(mouse.device_id());
    dataBuffer = getMouseData();
}

void loop() {
    if(interrupt) {
        send(dataBuffer);
        interrupt = false;
        dataBuffer = getMouseData();
    }
}

uint32_t getMouseData() {
    mouse.get_data();

    Data data;
    data.x = mouse.x_movement();
    data.y = mouse.y_movement();

    bool left = mouse.clicked(LEFT_BUTTON);
    bool right = mouse.clicked(RIGHT_BUTTON);

    data.flags = (left << 7) | (right << 6);

    readKeys(data);

    uint32_t buffer;
    memcpy(&buffer, &data, sizeof(Data));

    return buffer;
}

void send(uint32_t value) {
    constexpr int N = 11;
    constexpr uint64_t msk = 1ll << (N*3-1);

    uint64_t val = (uint64_t) value;

    delay(1);
    for (int i = 0; i < N; i++)  {
        digitalWrite(DATA1_PIN, !!(val & msk));
        val <<= 1;

        digitalWrite(DATA2_PIN, !!(val & msk));
        val <<= 1;

        digitalWrite(DATA3_PIN, !!(val & msk));
        val <<= 1;

        delayMicroseconds(300);
        digitalWrite(CLOCK_PIN, HIGH);
        delayMicroseconds(500);
        digitalWrite(CLOCK_PIN, LOW);
    }

    digitalWrite(DATA1_PIN, LOW);
    digitalWrite(DATA2_PIN, LOW);
    digitalWrite(DATA3_PIN, LOW);
}

void readKeys(Data& data) {
    for(int i=0; i<COL_NUM; ++i) {
        digitalWrite(columns[i], LOW);

        for(int j=0; j<ROW_NUM; ++j) {
            if(digitalRead(rows[j]) == LOW) {
                switch (keys[j][i]) {
                case '2': 
                    data.flags |= 1 << UP;
                    break;
                case '4': 
                    data.flags |= 1 << LEFT;
                    break;
                case '5': 
                    data.flags |= 1 << DOWN;
                    break;
                case '6': 
                    data.flags |= 1 << RIGHT;
                    break;
                default: 
                    break;
                }
            }
        }

        digitalWrite(columns[i], HIGH);
    }
}