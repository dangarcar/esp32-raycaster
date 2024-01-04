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

struct Data {
    int16_t x;
    int16_t y;
    uint16_t flags;
};
constexpr int UP = 5;
constexpr int DOWN = 4;
constexpr int LEFT = 3;
constexpr int RIGHT = 2;

void send(Data value);
Data getMouseData();
void readKeys(Data& data);

Data dataBuffer;

void requestEvent() {
    Wire.write((const uint8_t*) &dataBuffer, sizeof(Data));
    dataBuffer = Data();
}

void setup() {
    Wire.begin(8);                // join I2C bus with address #8
    Wire.onRequest(requestEvent);
    Serial.begin(9600);

    for(auto c: columns) {
        pinMode(c, OUTPUT);
        digitalWrite(c, HIGH);
    }

    for(auto r: rows) {
        pinMode(r, INPUT_PULLUP);
    }

    pinMode(13, OUTPUT);
    if(mouse.initialise()){
        Serial.println("Mouse error");
        digitalWrite(13, HIGH);
    }
    else {
        digitalWrite(13, LOW);
    }

    Serial.println(mouse.device_id());
    dataBuffer = getMouseData();
}

void loop() {
    Data newData = getMouseData();
    dataBuffer.x += newData.x;
    dataBuffer.y += newData.y;
    dataBuffer.flags |= newData.flags;
}

Data getMouseData() {
    mouse.get_data();

    Data data;
    data.x = mouse.x_movement();
    data.y = mouse.y_movement();

    bool left = mouse.clicked(LEFT_BUTTON);
    bool right = mouse.clicked(RIGHT_BUTTON);

    data.flags = (left << 7) | (right << 6);

    readKeys(data);

    return data;
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