#pragma once

#include "../math.hpp"
#include "../Player.hpp"
#include "Entity.hpp"

class Barrel: public Entity {
    static constexpr float EXPLOSION_RADIUS = 1.5f;
    static constexpr int EXPLOSION_DAMAGE = 16;
    static constexpr float DMG_PER_UNIT = EXPLOSION_DAMAGE/EXPLOSION_RADIUS;
    static constexpr float KNOCKBACK_SPEED = 0.8f;

    int deadCount = 0;
public:
    Barrel(float x, float y): Entity{x, y, 10.f, 1.2f} {
        this->r = 0.3f;
        this->object = true;
    }

    void damage(int amount) override {
        deadCount++;        
    }

    void update(float dt) override {
        if(deadCount == 2) {
            //EXPLODE THE PLAYER
            float dstX = sprite.x - player.pos.x, dstY = sprite.y - player.pos.y;
            float dst = fmaxf(abs(dstX), abs(dstY));
            if(dst <= EXPLOSION_RADIUS) {
                int dmg = EXPLOSION_DAMAGE - DMG_PER_UNIT*dst;
                player.damage(dmg);
            }

            //EXPLODE OTHER ENTITIES
            for(int i=0; i<EntityManager::entities_num; ++i) {
                if(i != this->sprite.entity_idx) {
                    Entity* e = EntityManager::entities[i];
                    float dstX = sprite.x - e->sprite.x, dstY = sprite.y - e->sprite.y;
                    float dst = fmaxf(abs(dstX), abs(dstY));
                    if(dst <= EXPLOSION_RADIUS && !e->isDead()) {
                        int dmg = EXPLOSION_DAMAGE - DMG_PER_UNIT*dst;
                        e->damage(dmg);
                    }
                }
            }
        }
    }

    void chooseSprite() override {
        if(deadCount>=4)
            return;

        switch (deadCount){
        case 1: sprite.texture = spr_BEXPC0; deadCount++; break;
        case 2: sprite.texture = spr_BEXPD0; deadCount++; break;
        case 3: sprite.texture = spr_BEXPE0; deadCount++; break;
        case 0: sprite.texture = spr_barrel; break;
        }

        SpriteManager::addSprite(sprite);
    }

    bool isDead() const override {
        return deadCount != 0;
    }

    virtual ~Barrel() {}
};