#pragma once

#include "sprites.h"
#include "Screen.hpp"
#include "../math.hpp"

class Sprite {
public:
    const uint16_t* texture = defaultS;
    float x, y, z, scale;
    int entity_idx;
    float dst;

    Sprite() = default;
    Sprite(float x, float y, float z, float scale):
        x{x}, y{y}, z{z}, scale{scale} {}
};

namespace SpriteManager {
    const int MAX_SPRITES = 64;
    Sprite sprites[MAX_SPRITES];
    int sprite_num = 0;

    //It is bad performance-wise but i don't care
    //It is not that bad because you can only reset the number, and it is simpler
    void clear() {
        sprite_num = 0;
    }

    int addSprite(Sprite s) {
        if(sprite_num >= MAX_SPRITES)
            return -1;

        sprites[sprite_num] = s;
        ++sprite_num;

        return 0;
    }

    void sortSprites(Vector2 center) {
        for(int i=0; i<sprite_num; ++i) {
            sprites[i].dst = (center.x - sprites[i].x)*(center.x - sprites[i].x) + (center.y-sprites[i].y)*(center.y-sprites[i].y);
        }

        qsort(sprites, sprite_num, sizeof(Sprite), [](const void* e1, const void* e2){
            Sprite* a = (Sprite*) e1;
            Sprite* b = (Sprite*) e2;

            return (a->dst < b->dst) - (a->dst > b->dst);
        });
    }
};