#pragma once

#include "math.hpp"
#include "InputData.hpp"
#include "render/Screen.hpp"
#include "entity/Entity.hpp"

#define MAX_HEALTH 16
#define INVINCIBLE false

class Player {
    static constexpr int NOISE_DST_SQR = 900;
    static constexpr int SHOT_DAMAGE = 4;
    static constexpr uint64_t HURT_TIME = 200000;
    static constexpr uint64_t SHOT_TIME = 400000;

    float _vel = 2.0f;

    bool _shoot = false, _open = false;
    bool _canShoot = true, _inRange = false;
    uint64_t _hurtStart = 0, _healStart = 0, _shotStart = 0, _pickStart = 0;

    Direction direction;
public:
    bool keyArray[10];
    float _yaw = 3*M_PI/2; //In radians

    int spriteAtCenter = -1;
    int blockCenterX, blockCenterY;
    int health = MAX_HEALTH;

    Vector2 pos;
    Vector2 dir;
    float r;

    Player(float x, float y, float r): pos{x,y} {
        memset(keyArray, false, sizeof(keyArray));
        this->r = r;
    }

    void update(InputData input, float dt);
    void damage(int amount);
    void heal(int amount);
    void giveKey(int door);
    void drawUI(Vector2 dst);
    void drawCompass(Vector2 dst);

    float getAngleRelativeTo(float theta) const {
        return _yaw - theta;
    }
};

Player player (30.0f, 30.0f, 0.3f);

void Player::update(InputData input, float dt) {
    if(!input.correct)
        return;

    _yaw -= input.x * dt * SENSITIVITY * DEG_TO_RAD;
    if(_yaw < 0.f) _yaw += M_2PI;
    else if(_yaw >= M_2PI) _yaw -= M_2PI;
    
    //This way intead of rotate dir for floating point precision
    dir = Vector2(cosf(_yaw), sinf(_yaw));

    direction = input.getDirection();
    if(direction != Direction::STOPPED) {
        Vector2 newDir;
        switch (direction) {
        case Direction::UP:       newDir = dir; break;
        case Direction::DOWN:     newDir = invert(dir); break;
        case Direction::LEFT:     newDir = rotatedMinus90Deg(dir); break;
        case Direction::RIGHT:    newDir = rotated90Deg(dir);  break;
        case Direction::UP_LEFT:  newDir = Vector2(dir.x-dir.y, dir.x+dir.y) * M_SQRT1_2; break;
        case Direction::UP_RIGHT: newDir = Vector2(dir.x+dir.y, -dir.x+dir.y) * M_SQRT1_2; break;
        default:
            break;
        }

        move(&pos, newDir, dt*_vel, r);
    }

    _canShoot = getTime()-_shotStart > SHOT_TIME;
    _inRange = spriteAtCenter != -1 && !EntityManager::entities[spriteAtCenter]->isDead();
    _shoot = input.leftClick() && _canShoot;
    if(_shoot) {
        _shotStart = getTime();

        if(_inRange){
            float x = EntityManager::entities[spriteAtCenter]->sprite.x - pos.x;
            float y = EntityManager::entities[spriteAtCenter]->sprite.y - pos.y;
            int dmg = max(0, SHOT_DAMAGE-(x*x+y*y)/16);
            EntityManager::entities[spriteAtCenter]->damage(dmg);
        }

        for(int i=0; i<EntityManager::entities_num; ++i) {
            float x = EntityManager::entities[i]->sprite.x - pos.x;
            float y = EntityManager::entities[i]->sprite.y - pos.y;
            if(x*x+y*y <= NOISE_DST_SQR) {
                EntityManager::entities[i]->alert();
            }
        }
    }

    _open = input.rightClick() && keyArray[ gameMap[blockCenterY][blockCenterX] ];
    if(_open) {
        keyArray[ gameMap[blockCenterY][blockCenterX] ] = false;
        gameMap[blockCenterY][blockCenterX] = 0;
    }

}

void Player::damage(int amount){
    if(INVINCIBLE) 
        return;

    health -= amount;
    _hurtStart = getTime();
}

void Player::heal(int amount){
    health += amount;
    _healStart = getTime();
}

void Player::giveKey(int door) {
    keyArray[door] = true;
    _pickStart = getTime();
}

void Player::drawCompass(Vector2 dst) {
    Screen::drawTexture(210, 130, compass_base);

    Vector2 diff = normalized(Vector2(dst.x-pos.x, dst.y-pos.y));
    float needle = dot(diff, dir);
    bool z = diff.x * dir.y - dir.x * diff.y > 0;
    int dx = (z? 1:-1) * int(ceilf(10*sqrtf(1.f-needle*needle)));
    int dy = ceilf(10*needle);

    Screen::drawLine(222, 142, 222 + dx, 142 - dy, 0xf800);
}

void Player::drawUI(Vector2 dst) {
    auto time = getTime();

    if(_canShoot)
        Screen::drawTexture(85, Screen::SCREEN_HEIGHT-pistol0.h, pistol0);
    else 
        Screen::drawTexture(85, Screen::SCREEN_HEIGHT-pistol_down.h, pistol_down);
    if(_shoot)
        Screen::drawTexture(85+14, Screen::SCREEN_HEIGHT-pistol1.h-33, pistol1);

    if(time - _hurtStart < HURT_TIME) {
        Screen::redify();
    } else if(time - _healStart < HURT_TIME) {
        Screen::greenify();
    } else if(time - _pickStart < HURT_TIME) {
        Screen::blueify();
    }

    if(keyArray[9])
        Screen::drawTexture(200, 1, small_key);
    if(keyArray[8])
        Screen::drawTexture(200, 1, green_key);

    int h = health;
    for(int i=0; i*2<MAX_HEALTH; ++i) {
        if(h == 1)
            Screen::drawTexture(1+i*16, 1, heart_half);
        else if(h > 1)
            Screen::drawTexture(1+i*16, 1, heart_full);
        else
            Screen::drawTexture(1+i*16, 1, heart_empty);

        h -= 2;
    }

    Screen::drawTexture(114, 75, _inRange? crosshair2:crosshair1);

    drawCompass(dst);
}