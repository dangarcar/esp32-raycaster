#pragma once

#include "../render/Sprite.hpp"
#include "../map.h"

class Entity {
public:
    bool object = true;
    Sprite sprite;
    float r;

    Entity(float x, float y, float z=0.0f, float scale=1.0f): sprite(x, y, z, scale) {}

    virtual void update(float dt) = 0;
    virtual void chooseSprite() = 0;
    virtual void damage(int amount) = 0;
    virtual bool isDead() const = 0;
    virtual void alert() {};
    virtual ~Entity() {}
};

namespace EntityManager {
    const int MAX_ENTITIES = SpriteManager::MAX_SPRITES; //It has to be equal so every entity has its own sprite
    Entity* entities[MAX_ENTITIES];
    int entities_num = 0;

    int addEntity(Entity* e) {
        if(entities_num >= MAX_ENTITIES)
            return -1;

        e->sprite.entity_idx = entities_num;
        entities[entities_num] = e;
        ++entities_num;

        return 0;
    }

    void updateEntities(float dt) {
        for(int i=0; i<entities_num; ++i) {
            entities[i]->update(dt);
        }
    }

    void updateSprites() {
        for(int i=0; i<entities_num; ++i) {
            entities[i]->chooseSprite();
        }
    }

    void clear() {
        for(int i=0; i<entities_num; ++i) {
            delete(entities[i]);
            entities[i] = nullptr;
        }

        entities_num = 0;
    }
};

//Returns the new position of the entity after movement with collision checks
bool move(Vector2* pos, Vector2 dir, float v, float r) {
    Vector2 mov = dir * v;

    float dx = 0.0f, dy = 0.0f;

    float boundX = pos->x + (mov.x>=0? r:-r);
    float boundY = pos->y + (mov.y>=0? r:-r);

    bool stuck = false;

    if(gameMap[int(boundY)][int(boundX+mov.x)] == 0)
        dx = mov.x;
    else 
        stuck = true;
    if(gameMap[int(boundY+mov.y)][int(boundX)] == 0)
        dy = mov.y;
    else
        stuck = true;

    *pos = Vector2(pos->x + dx, pos->y + dy);

    return stuck;
}