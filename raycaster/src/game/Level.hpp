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
    const Vector2 initialPos;
    uint16_t* const floor_tex;
    const uint16_t fog;
    int (*map)[MAP_WIDTH];
    void (*const startCallback)();

    Level(Level *const next, const Vector2 init, const uint16_t fog, int (*newMap)[MAP_WIDTH], uint16_t* floor_tex, void (*startCallback)()): 
        initialPos{init},
        fog{fog},  
        startCallback{startCallback},
        floor_tex{floor_tex},
        next{next} {
            map = newMap;
    }

    void start() {
        camera = Camera();
        EntityManager::clear();

        startCallback();

        gameMap = map;
        fog_color = fog;
        floor_texture = floor_tex;
        player.pos = initialPos;
    }
};

Level WIN(nullptr, {}, 0, nullptr, nullptr, []{});

Level SECOND_LEVEL(&WIN, {20.f,20.f}, 0x9165, map2, floor_cobble, []{
    EntityManager::addEntity(new Trasporter(1,1,6,5));
    EntityManager::addEntity(new Healer(29, 27, 5));
    EntityManager::addEntity(new Key(29, 26, 8));

    Barrel* barrels[] = {
        new Barrel(27, 27),
        new Barrel(29, 29),
        new Barrel(17, 6),
        new Barrel(5, 20),
        new Barrel(23, 7),
        new Barrel(29, 9),
        new Barrel(14, 6),
        new Barrel(1, 2),
        new Barrel(28, 2),
        new Barrel(13, 9),
        new Barrel(10, 6),
        new Barrel(2, 2),
        new Barrel(30, 7),
        new Barrel(10, 9),
        new Barrel(7, 14),
        new Barrel(5, 21),
        new Barrel(8, 17),
        new Barrel(9, 20),
        new Barrel(16, 4),
        new Barrel(2, 3),
    };

    for(int i=0; i<sizeof(barrels)/sizeof(Barrel*); ++i) {
        EntityManager::addEntity(barrels[i]);
    }
});

Level INITIAL_LEVEL(&SECOND_LEVEL, {30.f,30.f}, 0xbe9a, map1, floor_grass, []{
    EntityManager::addEntity(new Trasporter(1,1,6,5));

    for(int i=0; i<2; ++i) {
        for(int j=0; j<3; ++j) {
            EntityManager::addEntity(new Enemy(19+i, 25+j));
        }
    }

    EntityManager::addEntity(new Healer(29, 27, 5));
    EntityManager::addEntity(new Key(29, 26, 9));

    Barrel* barrels[] = {
        new Barrel(27, 27),
        new Barrel(29, 29),
        new Barrel(17, 6),
        new Barrel(5, 20),
        new Barrel(23, 7),
        new Barrel(29, 9),
        new Barrel(14, 6),
        new Barrel(1, 2),
        new Barrel(28, 2),
        new Barrel(13, 9),
        new Barrel(10, 6),
        new Barrel(2, 2),
        new Barrel(30, 7),
        new Barrel(10, 9),
        new Barrel(7, 14),
        new Barrel(5, 21),
        new Barrel(8, 17),
        new Barrel(9, 20),
        new Barrel(16, 4),
        new Barrel(2, 3),
    };

    for(int i=0; i<sizeof(barrels)/sizeof(Barrel*); ++i) {
        EntityManager::addEntity(barrels[i]);
    }
});

Level *level = &INITIAL_LEVEL;