#pragma once

#include "game/Player.hpp"
#include "game/render/Camera.hpp"
#include "game/render/screens.h"
#include "Level.hpp"

namespace Game {
    void setup() {
        for(int i=0; i<FLOOR_HEIGHT*FLOOR_WIDTH; ++i) {
            floor_wood[i] = DARKEN_COLOUR(floor_wood[i]);
            floor_cobble[i] = DARKEN_COLOUR(floor_cobble[i]);
        }

        level->start();
    }

    bool loop(float delta, InputData input) {
        SpriteManager::clear();
        Screen::clear();

        player.update(input, delta);
        camera.update(player);

        EntityManager::updateEntities(delta);
        EntityManager::updateSprites();

        camera.draw();
        player.drawUI();

        if(transport) {
            transport = false;
            level = level->next;
            if(level) {
                level->start();
            }
            else
                return false;
        }

        if(player.health <= 0) {
            for(int t=0; t<5; ++t) Screen::redify();
            Screen::drawTexture(0, 0, {screen_death, 240, 160});
            player.drawUI(); //To make the hearts look empty
        } else if(level == &WIN) {
            level = &INITIAL_LEVEL;

            Screen::redify(); Screen::blueify(); Screen::greenify(); //Whitify
            Screen::redify(); Screen::blueify(); Screen::greenify(); //Whitify x2
            Screen::drawTexture(0, 0, {screen_win, 240, 160});
        } else 
            return false;
        return true;
    }

    void reset(InputData* input) {
        *input = InputData{};
        player = Player(0.0f, 0.0f, 0.3f);
        level->start();
        camera = Camera();
    }
}