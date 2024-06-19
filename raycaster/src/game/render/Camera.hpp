#pragma once

#include "../Player.hpp"
#include "Screen.hpp"
#include "textures.h"
#include "Sprite.hpp"
#include "../map.h"

//constexpr uint16_t AO_Levels[4] = {0xffff, 0x8410, 0x4a69, 0x18c3};
constexpr float AO_Levels[4] = {1.0, 0.5, 0.3, 0.1};

#define FOG_ACTIVE 1

inline uint16_t blend(uint16_t c1, uint16_t c2, uint8_t alpha) {
#ifndef FOG_ACTIVE
    return c1;
#else
    alpha >>= 3;

    uint32_t c1_32 = uint32_t(c1);
    uint32_t c1_spa = c1_32 & 0x1f;
    c1_spa |= (c1_32 & 0x7e0) << 5;
    c1_spa |= (c1_32 & 0xf800) << 11;

    uint32_t c2_32 = uint32_t(c2);
    uint32_t c2_spa = c2_32 & 0x1f;
    c2_spa |= (c2_32 & 0x7e0) << 5;
    c2_spa |= (c2_32 & 0xf800) << 11;

    uint32_t ans_spa = (c1_spa * alpha) + (c2_spa * (32 - alpha));

    return uint16_t( 
        ((ans_spa >> 5) & 0x1f) + 
        ((ans_spa >> 10) & 0x7e0) + 
        ((ans_spa >> 16) & 0xf800) 
    );
#endif
}

inline uint16_t smoothDarken(uint16_t c, float alpha) {
    float r = (c >> 11) & 0x1f;
    float g = (c >> 5) & 0x3f;
    float b = c & 0x1f;

    r = r*alpha;
    g = g*alpha;
    b = b*alpha;

    return uint16_t( (int(r) << 11) | (int(g) << 5) | int(b) );
}

inline float interpolate(float c[4], float x, float y) {
    return c[0]*(1.0-x)*(1.0-y) + c[1]*x*(1.0-y) + c[2]*(1.0-x)*y + c[3]*x*y;
}

class Camera {
private:
    static constexpr int H = Screen::SCREEN_HEIGHT;
    static constexpr int W = Screen::SCREEN_WIDTH;
    static constexpr int H_2 = H/2;
    static constexpr int W_2 = W/2;

    Vector2 pos;
    Vector2 dir;
    Vector2 plane;
public:
    float Zbuffer[W];

    void update(const Player& player) {
        pos = player.pos;
        dir = player.dir;
        plane = Vector2(dir.y, -dir.x);
    }

    void draw() {
        drawWalls();
        drawSprites();
    }

private:
    void drawWalls();
    void drawSprites();

    float getAlpha(int y, float perpDistance, float floorXWall, float floorYWall);
};

Camera camera;

void Camera::drawSprites() {
    player.spriteAtCenter = -1;
    SpriteManager::sortSprites(pos);

    for (int i=0; i<SpriteManager::sprite_num; ++i) {
        Sprite sprite = SpriteManager::sprites[i];

        float spriteX = sprite.x - pos.x;
        float spriteY = sprite.y - pos.y;

        float invDet = 1.f / (plane.x * dir.y - dir.x * plane.y);

        float transformX = invDet * (dir.y * spriteX - dir.x * spriteY);
        float transformY = invDet * (-plane.y * spriteX + plane.x * spriteY); // this is actually the depth inside the screen, that what Z is in 3D

        int spriteHW = abs(int(H / transformY)) / sprite.scale;

        if(transformY > 0 && spriteHW > 0) { //In screen
            int spriteScreenX = int(W_2 * (1.f + transformX / transformY));
            int spriteScreenZ = int(sprite.z / transformY);

            int drawStartY = -spriteHW/2 + H_2 + spriteScreenZ;
            if (drawStartY < 0) drawStartY = 0;
            int drawEndY = spriteHW/2 + H_2 + spriteScreenZ;
            if (drawEndY >= H) drawEndY = H;

            int drawStartX = -spriteHW / 2 + spriteScreenX;
            if (drawStartX < 0) drawStartX = 0;
            int drawEndX = spriteHW / 2 + spriteScreenX;
            if (drawEndX >= W) drawEndX = W - 1;

            for (int x=drawStartX; x<=drawEndX; ++x) {
                int texX = int(256 * (x - (-spriteHW / 2 + spriteScreenX)) * SPR_WIDTH / spriteHW) / 256;

                if (x >= 0 && x < W && transformY < Zbuffer[x]) {
                    uint8_t blendDist = 255/fmaxf(1.f, transformY);

                    for(int y=drawStartY; y<drawEndY; ++y) {
                        int d = (y-spriteScreenZ) * 256 - H * 128 + spriteHW * 128; // 256 and 128 factors to avoid floats
                        int texY = ((d * SPR_HEIGHT) / spriteHW) / 256;

                        uint16_t color = sprite.texture[SPR_WIDTH * texY + texX];
                        if (color != TRANSP) {
                            if(x == W_2) {
                                int idx = sprite.entity_idx;
                                if(EntityManager::entities[idx]->isDead() == false)
                                    player.spriteAtCenter = idx;
                            }
                            Screen::drawPixel(x, y, blend(color, fogColor, blendDist));
                        }
                    }
                }
            }
        }        
    }
}

float Camera::getAlpha(int y, float perpDistance, float floorXWall, float floorYWall) {
    float currentDist = H / (2.0 * y - H);
    float weight = currentDist / perpDistance;

    float currentFloorX = weight * floorXWall + (1.0 - weight) * pos.x;
    float currentFloorY = weight * floorYWall + (1.0 - weight) * pos.y;

    int tx = int(float(FLOOR_WIDTH) * currentFloorX) & (FLOOR_WIDTH-1);
    int ty = int(float(FLOOR_HEIGHT) * currentFloorY) & (FLOOR_HEIGHT-1);

    int floorX = int(currentFloorX), floorY = int(currentFloorY);
    int aoVertex[4] = {
        !!(gameMap[floorY-1][floorX-1]) + !!(gameMap[floorY-1][floorX]) + !!(gameMap[floorY][floorX-1]), //Top Left
        !!(gameMap[floorY-1][floorX]) + !!(gameMap[floorY-1][floorX+1]) + !!(gameMap[floorY][floorX+1]), //Top Right
        !!(gameMap[floorY][floorX-1]) + !!(gameMap[floorY+1][floorX-1]) + !!(gameMap[floorY+1][floorX]), //Bottom Left
        !!(gameMap[floorY][floorX+1]) + !!(gameMap[floorY+1][floorX+1]) + !!(gameMap[floorY+1][floorX]), //Bottom Right
    };

    float aoColors[4];
    for(int i=0; i<4; ++i)
        aoColors[i] = AO_Levels[aoVertex[i]];

    float xPer = float(tx)/(FLOOR_WIDTH-1), yPer = float(ty)/(FLOOR_HEIGHT-1);
    return interpolate(aoColors, xPer, yPer);
}

void Camera::drawWalls() {
    for(int x=0; x<W; ++x) {
        float camX = 2.0f * x / float(W) - 1.0f; // -1 to 1 being 0 the center of the screen
        float rayDirX = dir.x + plane.x*camX;
        float rayDirY = dir.y + plane.y*camX;

        int mapX = int(pos.x);
        int mapY = int(pos.y);

        float deltaDistX = abs(1.0f/rayDirX);
        float deltaDistY = abs(1.0f/rayDirY);

        float sideDistX, sideDistY;
        int stepX, stepY;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (pos.x - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - pos.x) * deltaDistX;
        }

        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (pos.y - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - pos.y) * deltaDistY;
        }

        int side;
        bool hit = false;
        while(!hit) {
            if(sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
 
            hit = gameMap[mapY][mapX] > 0;
        }

        //Draw line code
        float perpDistance = side==0? (sideDistX - deltaDistX):(sideDistY - deltaDistY); //Perpendicular distance, not euclidean
        uint8_t blendDist = 255/fmaxf(1.f, perpDistance);

        int lineHeight = int(H / perpDistance);
        int drawStart = -lineHeight / 2 + H_2;
        if(drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + H_2;
        if(drawEnd >= H) drawEnd = H - 1;

        float wallX = side==0? (pos.y + perpDistance * rayDirY) : (pos.x + perpDistance * rayDirX);
        wallX -= floor(wallX); //From 0 to 1;

        int texX = int(wallX * float(TEX_WIDTH)) % TEX_WIDTH;
        if(side == 0 && rayDirX > 0.0f) texX = TEX_WIDTH - texX - 1;
        if(side == 1 && rayDirY < 0.0f) texX = TEX_WIDTH - texX - 1;

        float step = float(TEX_HEIGHT) / float(lineHeight+1);
        float texPos = (drawStart - H_2 + lineHeight / 2) * step;

        //DRAW FLOOR
        float floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall
        if(side == 0 && rayDirX > 0) {
            floorXWall = mapX;
            floorYWall = mapY + wallX;
        } else if(side == 0 && rayDirX < 0) {
            floorXWall = mapX + 1.0;
            floorYWall = mapY + wallX;
        } else if(side == 1 && rayDirY > 0) {
            floorXWall = mapX + wallX;
            floorYWall = mapY;
        } else {
            floorXWall = mapX + wallX;
            floorYWall = mapY + 1.0;
        }

        //Brightness stuff
        float aoAlpha = getAlpha(drawEnd, perpDistance, floorXWall, floorYWall);

        //DRAW WALLS
        for(int y=drawStart; y<=drawEnd; ++y) {
            int texY = int(texPos) & (TEX_HEIGHT-1); //Mask to not overflow
            texPos += step;
            uint16_t c = textures[gameMap[mapY][mapX]][texY*TEX_HEIGHT + texX];

            if(side) 
                c = DARKEN_COLOUR(c);

            if(x == W_2) {
                player.blockCenterX = mapX;
                player.blockCenterY = mapY;
            }

            auto yAlpha = float(texY)/(TEX_HEIGHT-1);
            float alpha = 1.0 + (aoAlpha - 1.0) * yAlpha;
            c = smoothDarken(c, SDL_clamp(alpha, 0.0, 1.0));

            Screen::drawPixel(x, y, blend(c, fogColor, blendDist));
        }

        Zbuffer[x] = perpDistance;

        //DRAW FLOOR
        if(drawEnd < 0) 
            drawEnd = H; 
        for(int y=drawEnd+1; y<H; ++y) {
            float currentDist = H / (2.0 * y - H);
            float weight = currentDist / perpDistance;

            float currentFloorX = weight * floorXWall + (1.0 - weight) * pos.x;
            float currentFloorY = weight * floorYWall + (1.0 - weight) * pos.y;

            int tx = int(float(FLOOR_WIDTH) * currentFloorX) & (FLOOR_WIDTH-1);
            int ty = int(float(FLOOR_HEIGHT) * currentFloorY) & (FLOOR_HEIGHT-1);

            uint16_t c = floorTexture[FLOOR_WIDTH*ty + tx];

            int floorX = int(currentFloorX), floorY = int(currentFloorY);
            int aoVertex[4] = {
                !!(gameMap[floorY-1][floorX-1]) + !!(gameMap[floorY-1][floorX]) + !!(gameMap[floorY][floorX-1]), //Top Left
                !!(gameMap[floorY-1][floorX]) + !!(gameMap[floorY-1][floorX+1]) + !!(gameMap[floorY][floorX+1]), //Top Right
                !!(gameMap[floorY][floorX-1]) + !!(gameMap[floorY+1][floorX-1]) + !!(gameMap[floorY+1][floorX]), //Bottom Left
                !!(gameMap[floorY][floorX+1]) + !!(gameMap[floorY+1][floorX+1]) + !!(gameMap[floorY+1][floorX]), //Bottom Right
            };

            float aoColors[4];
            for(int i=0; i<4; ++i)
                aoColors[i] = AO_Levels[aoVertex[i]];

            float xPer = float(tx)/(FLOOR_WIDTH-1), yPer = float(ty)/(FLOOR_HEIGHT-1);
            auto alpha = interpolate(aoColors, xPer, yPer);
            c = smoothDarken(c, SDL_clamp(alpha, 0.0, 1.0));
            aoAlpha = __min(aoAlpha, alpha);

            float floorFogDist = 255/fmaxf(currentDist, 1.0);
            Screen::drawPixel(x, y, blend(c, fogColor, floorFogDist));
        }

        //DRAW SKY
        for(int y=0; y<drawStart; ++y) {
            Screen::drawPixel(x, y, fogColor);
        }
    }
}