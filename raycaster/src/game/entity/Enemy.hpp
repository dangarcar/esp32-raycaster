#pragma once 

#include "Entity.hpp"
#include "../render/enemy_sprites.h"

const float COS_PI_8 = cosf(M_PI/8.f);
const float COS_3PI_8 = cosf(3.f*M_PI/8.f);

enum class EnemyState: char {
    IDLING, CHASING, SHOOTING, DEAD
};

class Enemy: public Entity {
    static constexpr float DAMAGE = 3;
    static constexpr float SEE_DISTANCE = 5.f;
    static constexpr float SHOT_DISTANCE = 3.f;
    static constexpr float ANIM_TIME = 0.15f;
    static constexpr float DIR_TIME = 1.5f;

    int deadCount = 0;
    float _lastTimeAnim = 0.f;
    float _lastTimeDir = 3764827623.f;

    int _animCount = 0;
    int _shootCount = 0;
    int _nextShot = 16;

    float _vel = 1.5f;

    EnemyState state = EnemyState::IDLING;
    
    bool see = false;
    float distToPlayerLength;
public:
    int health = 4;
    Vector2 dir;

    Enemy(float x, float y): Entity{x, y, 16.f, 1.3f}, dir{1, 0} {
        this->r = 0.3f;
        this->object = false;
    }

    void damage(int amount) override {
        health -= amount;
        ++deadCount;

        if(health <= 0) {
            state = EnemyState::DEAD;
        }
    }

    void update(float dt) override {
        if(state == EnemyState::DEAD) 
            return;

        see = seePlayer();
        Vector2 distToPlayer = {player.pos.x-sprite.x, player.pos.y-sprite.y};
        distToPlayerLength = length(distToPlayer);

        if(_shootCount == 0 && see && distToPlayerLength<SHOT_DISTANCE) {
            _shootCount++;
            _nextShot = 12 + rand()%15;

            state = EnemyState::SHOOTING;
            dir = normalized(distToPlayer);
        }
        else if(state == EnemyState::SHOOTING ) {
            if(_shootCount == 3 && see && distToPlayerLength<SHOT_DISTANCE) {
                player.damage(DAMAGE);
            }
            else if(_shootCount == 10) {
                state = EnemyState::CHASING;
            }
        }
        else {
            _lastTimeAnim += dt;
            _lastTimeDir += dt;

            if(_lastTimeDir >= DIR_TIME) {
                _lastTimeDir = 0.f;

                if(state == EnemyState::IDLING) {
                    float angle = float(rand())/float(RAND_MAX);
                    dir = Vector2(cosf(angle), sinf(angle));
                } else {
                    dir = normalized(distToPlayer);
                }
            }

            if(distToPlayerLength > 1.f) {
                Vector2 pos(sprite.x, sprite.y);
                if(move(&pos, dir, dt*_vel, this->r)) {
                    if(rand()%2 == 0)
                        dir = rotated90Deg(dir);
                    else 
                        dir = rotatedMinus90Deg(dir);
                }
                sprite.x = pos.x; 
                sprite.y = pos.y;

                if(_lastTimeAnim >= ANIM_TIME) { //Animate character
                    _lastTimeAnim = 0.f;
                    _animCount = ++_animCount % 4;   
                }
            }
        }
    }

    void chooseSprite() override {
        switch (state){
        case EnemyState::DEAD:
            if(deadCount == 1)
                sprite.texture = enemy_front_dying0;
            else if(deadCount < 5)
                sprite.texture = enemy_dyings[deadCount-1];
            else 
                sprite.texture = enemy_death;
        break;
        
        case EnemyState::SHOOTING:
            if(_shootCount == 3)
                sprite.texture = enemy_front_shoot1;
            else
                sprite.texture = enemy_front_shoot0;
        break;

        default:
            float d = dot(dir, player.dir);
            bool right = dir.x * player.dir.y - player.dir.x * dir.y > 0.f;

            if(d > COS_PI_8) {
                sprite.texture = enemy_backs[_animCount];
            } else if(d > COS_3PI_8) {
                sprite.texture = right? enemy_back_rights[_animCount]:enemy_back_lefts[_animCount];
            } else if(d > -COS_3PI_8) {
                sprite.texture = right? enemy_rights[_animCount]:enemy_lefts[_animCount];
            } else if(d > -COS_PI_8) {
                sprite.texture = right? enemy_front_rights[_animCount]:enemy_front_lefts[_animCount];
            } else {
                sprite.texture = enemy_fronts[_animCount];
            }
        break;
        }

        if(deadCount != 0) {
            ++deadCount;
            if(state != EnemyState::DEAD) {
                sprite.texture = enemy_front_dying0;
                deadCount %= 3;
            }
        }
        if(_shootCount != 0)
            _shootCount = ++_shootCount % _nextShot;

        SpriteManager::addSprite(sprite);
    }

    void alert() override {
        if(state == EnemyState::DEAD) 
            return;
        state = EnemyState::CHASING;
        _lastTimeDir = 3764827623.f;
    }

    bool isDead() const override {
        return state == EnemyState::DEAD;
    }

    bool seePlayer() {
        float dx = player.pos.x - sprite.x;
        float dy = player.pos.y - sprite.y;
        float step = fmaxf(abs(dx),abs(dy));

        if(step > SEE_DISTANCE)
            return false;

        dx = dx / step;
        dy = dy / step;
        float x = sprite.x;
        float y = sprite.y;

        bool see = true;
        for(int i=0; i <= step; ++i) {
            x += dx;
            y += dy;
            if(gameMap[(int)y][(int)x] > 0){
                see = false;
                break;
            }
        }

        return see;
    }

    virtual ~Enemy() {}
};