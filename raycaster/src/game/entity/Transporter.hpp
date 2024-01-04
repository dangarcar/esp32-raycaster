#pragma once

#include "Entity.hpp"
#include "../Player.hpp"

bool transport = false;

class Trasporter: public Entity {
    int w,h;
public:

    Trasporter(float x, float y, int w, int h) : Entity{x, y} {
        this->object = true;
        this->w = w; 
        this->h = h;
    }

    //UNUSED
    void damage(int a) override {}
    void chooseSprite() override {}
    bool isDead() const override { return true; }

    void update(float dt) override {
        transport = player.pos.x >= sprite.x && player.pos.x < sprite.x+w
        && player.pos.y >= sprite.y && player.pos.y < sprite.y+h;
    }

    virtual ~Trasporter() {}
};