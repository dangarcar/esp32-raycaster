#pragma once

#include "Entity.hpp"
#include "../Player.hpp"

class Healer: public Entity {
    const int healAmount;
    bool exists = true;
public:
    Healer(float x, float y, int healAmount) : Entity{x, y}, healAmount{healAmount} {
        this->r = 0.3f;
        sprite.texture = healAmount>3? spr_MEDIA0 : spr_STIMA0;
        this->object = true;
    }

    //UNUSED
    void damage(int a) override {}

    void update(float dt) override {
        if(!exists)
            return;

        float dx = abs(player.pos.x - sprite.x);
        float dy = abs(player.pos.y - sprite.y);

        if(dx*dx + dy*dy <= this->r) {
            player.heal(healAmount);
            exists = false;
        } 
    }

    void chooseSprite() override {
        if(exists) {
            SpriteManager::addSprite(this->sprite);
        }
    }

    bool isDead() const override {
        return exists;
    }

    virtual ~Healer() {}
};