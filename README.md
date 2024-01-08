# Arduino Raycaster

This is a PlatformIO project with esp32 that is a raycaster able to run on esp32.

It is made with a Lilygo T3 v1.6 LoRa 32 as the main processor, a 3.5 inch TFT9846 screen as the display, an Arduino Mega 2560 as the peripheral receiver with one keypad and a PS2 mouse. The two controller communicate between them with I2C. The mouse and the keypad are not attached directly to the esp32 because there aren't enough pins to connect them.

As the code is made with C++, I managed to separate almost all the game logic and I made it platform independent, so I've made a port of the game that runs with SDL2 in my Windows machine. It is controlled with the WASD keys and the mouse instead of the 2456 of the keypad and the old mouse.

1. Clone the repository: ```git clone https://github.com/dangarcar/esp32-raycaster```
2. Go to the test folder in raycaster: ```cd esp32-raycaster/raycaster/test```
3. Run make: ```make```
4. Play! (And don't forget Esc is the key to exit): 
![Here should have been a gameplay screenshot in SDL2's port]()