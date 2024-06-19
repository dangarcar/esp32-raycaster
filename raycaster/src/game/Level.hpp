#pragma once

#include "entity/Entity.hpp"
#include "entity/Barrel.hpp"
#include "entity/Enemy.hpp"
#include "entity/Healer.hpp"
#include "entity/Key.hpp"
#include "entity/Transporter.hpp"

#include "render/Camera.hpp"
#include "map.h"

struct Level {
    Level *const next;
    const Vector2 initialPos, escapePos;
    uint16_t* const floorTex;
    const uint16_t fog;
    uint8_t (*map)[MAP_WIDTH];
    void (*const startCallback)();

    Level(Level *const next, const Vector2 init, const Vector2 end, const uint16_t fog, uint8_t (*newMap)[MAP_WIDTH], uint16_t* floorTex, void (*startCallback)()): 
        initialPos{init},
        escapePos{end},
        fog{fog},  
        startCallback{startCallback},
        floorTex{floorTex},
        next{next} {
            map = newMap;
    }

    void start() {
        camera = Camera();
        EntityManager::clear();

        startCallback();

        gameMap = map;
        fogColor = fog;
        floorTexture = floorTex;
        player.pos = initialPos;
    }
};

Level WIN(nullptr, {}, {}, 0, nullptr, nullptr, []{});

Level FINAL_LEVEL(&WIN, {30.f,62.f}, {2.5f,2.5f}, 0x0000, map3, floor_wood, []{
    EntityManager::addEntity(new Trasporter(2,2,1,1));
    EntityManager::addEntity(new Key(28.5, 60.5, 8));

    Healer* healers[] = {
        new Healer(10.5, 4.5, 8),
        new Healer(28.5, 2.5, 3),
        new Healer(28.5, 7.5, 3),
        new Healer(3.5, 7.5, 8),
        new Healer(8.5, 17.5, 3),
        new Healer(8.5, 21.5, 3),
        new Healer(6.5, 19.5, 3),
        new Healer(10.5, 19.5, 3),
        new Healer(17.5, 32.5, 3),
        new Healer(17.5, 36.5, 3),
        new Healer(10.5, 55.5, 3),
        new Healer(10.5, 61.5, 3),
    };
    
    for(int i=0; i<sizeof(healers)/sizeof(Healer*); ++i) {
        EntityManager::addEntity(healers[i]);
    }

    Barrel* barrels[] = {
        new Barrel(22.5, 9.5),
        new Barrel(20.5, 18.5),
        new Barrel(14.5, 22.5),
        new Barrel(4.5, 24.5),
        new Barrel(10.5, 28.5),
        new Barrel(22.5, 36.5),
        new Barrel(4.5, 35.5),
        new Barrel(12.5, 44.5),
        new Barrel(6.5, 48.5),
        new Barrel(18.5, 50.5),
        new Barrel(27.5, 49.5),
        new Barrel(29.5, 28.5),
    };

    for(int i=0; i<sizeof(barrels)/sizeof(Barrel*); ++i) {
        EntityManager::addEntity(barrels[i]);
    }

    Enemy* enemys[] = {
        new Enemy(7.5, 6.5),
        new Enemy(10.5, 44.5),
        new Enemy(18.5, 59.5),
        new Enemy(28.5, 2.5),
        new Enemy(20.5, 56.5),
        new Enemy(22.5, 26.5),
        new Enemy(14.5, 35.5),
        new Enemy(9.5, 26.5),
        new Enemy(6.5, 50.5),
        new Enemy(30.5, 20.5),
        new Enemy(14.5, 2.5),
        new Enemy(28.5, 7.5),
        new Enemy(30.5, 33.5),
        new Enemy(30.5, 23.5),
        new Enemy(10.5, 28.5),
        new Enemy(28.5, 42.5),
        new Enemy(12.5, 48.5),
        new Enemy(18.5, 47.5),
        new Enemy(28.5, 20.5),
        new Enemy(12.5, 24.5),
    };

    for(int i=0; i<sizeof(enemys)/sizeof(Enemy*); ++i) {
        EntityManager::addEntity(enemys[i]);
    }
});

Level SECOND_LEVEL(&FINAL_LEVEL, {30.f,62.f}, {8.5f,1.5f}, 0x9165, map2, floor_cobble, []{
    EntityManager::addEntity(new Trasporter(8,1,1,1));
    EntityManager::addEntity(new Key(13.5, 31.5, 8));

    EntityManager::addEntity(new Healer(19.5, 31.5, 6));
    EntityManager::addEntity(new Healer(6.5, 3.5, 8));
    EntityManager::addEntity(new Healer(9.5, 13.5, 3));
    EntityManager::addEntity(new Healer(16.5, 28.5, 3));
    EntityManager::addEntity(new Healer(16.5, 34.5, 3));

    Enemy* enemys[] = {
        new Enemy(30.5, 52.5),
        new Enemy(17.5, 62.5),
        new Enemy(11.5, 58.5),
        new Enemy(20.5, 43.5),
        new Enemy(8.5, 50.5),
        new Enemy(3.5, 26.5),
        new Enemy(3.5, 14.5),
        new Enemy(19.5, 2.5),
        new Enemy(26.5, 23.5),
        new Enemy(23.5, 14.5),
        new Enemy(27.5, 45.5),
        new Enemy(5.5, 3.5),
    };

    for(int i=0; i<sizeof(enemys)/sizeof(Enemy*); ++i) {
        EntityManager::addEntity(enemys[i]);
    }
});

Level INITIAL_LEVEL(&SECOND_LEVEL, {30.f,62.f}, {15.5f,2.5f}, 0xbe9a, map1, floor_grass, []{
    EntityManager::addEntity(new Trasporter(15,2,1,1));
    EntityManager::addEntity(new Key(20.5, 42.5, 9));

    for(int i=0; i<2; ++i) {
        for(int j=0; j<3; ++j) {
            EntityManager::addEntity(new Enemy(24.5+i, 47.5+j));
        }
    }

    EntityManager::addEntity(new Healer(15.5, 3.5, 8));
    EntityManager::addEntity(new Enemy(15.5, 4.5));
});

Level *level = &FINAL_LEVEL;