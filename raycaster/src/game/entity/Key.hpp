#pragma once

#include "Entity.hpp"
#include "../Player.hpp"

class Key: public Entity {
    const int door;
    bool exists = true;
public:
    Key(float x, float y, int door) : Entity{x, y}, door{door} {
        this->r = 0.3f;
        sprite.texture = door==9? spr_key0 : spr_key1;
        sprite.scale = 3;
        sprite.z = 30;
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
            player.giveKey(door);
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

    virtual ~Key() {}
};