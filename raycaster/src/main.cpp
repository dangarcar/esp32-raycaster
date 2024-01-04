#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>

#include "game/game.hpp"

#define WIDTH TFT_HEIGHT
#define HEIGHT TFT_WIDTH

TFT_eSPI tft;

#define pushPixel(color) \
    GPIO.out_w1tc = GPIO_OUT_CLR_MASK; GPIO.out_w1ts = tft.xset_mask[(uint8_t) ((color) >> 8)]; WR_H; \
    GPIO.out_w1tc = GPIO_OUT_CLR_MASK; GPIO.out_w1ts = tft.xset_mask[(uint8_t) (color)]; WR_H;

void render() {
    tft.setWindow(0, 0, WIDTH-1, HEIGHT-1);

    for (int i=0; i<Screen::SCREEN_HEIGHT; ++i) {
        for (int j=0; j<Screen::SCREEN_WIDTH; ++j) {
            auto b = Screen::_screen[i*Screen::SCREEN_WIDTH + j];
            pushPixel(b); 
            pushPixel(b);
        }
        for (int j=0; j<Screen::SCREEN_WIDTH; ++j) {
            auto b = Screen::_screen[i*Screen::SCREEN_WIDTH + j];
            pushPixel(b); 
            pushPixel(b);
        }
    }
}

InputData receiveData() {
    InputData data;
    Wire.requestFrom(8, 6);    // request 4 bytes from slave device #8

    if(Wire.available()) { // slave may send less than requested
        uint8_t buffer[6];
        Wire.readBytes(buffer, 6);

        memcpy(&data, buffer, 6);
        data.correct = true;
    }
    else {
        data.correct = false;
    }

    return data;
}

void setup() {
    Wire.begin(21, 22);  
    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);

    Screen::fillScreen(screen_start);
    render();
    Game::setup();
    delay(1000);
}

float delta;
bool over = false;

void loop() {
    static float delta;
    auto t = getTime();

    InputData input = receiveData();

    if(input.rightClick() || input.leftClick()) {
        if(over)
            Game::reset(&input);
        over = false;
    }

    if(!over) over = Game::loop(delta, input);

    render();

    delta = float(getTime() - t)/1000000.0f;
    Serial.println(delta*1000000);
}