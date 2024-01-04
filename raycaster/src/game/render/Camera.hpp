#pragma once

#include "../Player.hpp"
#include "Screen.hpp"
#include "textures.h"
#include "Sprite.hpp"
#include "../map.h"

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

class Camera {    
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

    void draw();

private:
    void drawFloor();
    void drawWalls();
    void drawSprites();
};

Camera camera;

void Camera::draw() {
    drawFloor();
    drawWalls();

    drawSprites();
}

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

        if(transformY > 0) { //In screen
            int spriteScreenX = int(W_2 * (1.f + transformX / transformY));
            int spriteScreenZ = int(sprite.z / transformY);

            int spriteHeight = abs(int(H / transformY)) / sprite.scale;
            int drawStartY = -spriteHeight/2 + H_2 + spriteScreenZ;
            if (drawStartY < 0) drawStartY = 0;
            int drawEndY = spriteHeight/2 + H_2 + spriteScreenZ;
            if (drawEndY >= H) drawEndY = H;

            int spriteWidth = abs(int(H / (transformY))) / sprite.scale;
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            if (drawStartX < 0) drawStartX = 0;
            int drawEndX = spriteWidth / 2 + spriteScreenX;
            if (drawEndX >= W) drawEndX = W - 1;

            for (int x=drawStartX; x<=drawEndX; ++x) {
                int texX = int(256 * (x - (-spriteWidth / 2 + spriteScreenX)) * SPR_WIDTH / spriteWidth) / 256;

                if (x >= 0 && x < W && transformY < Zbuffer[x]) {
                    uint8_t blendDist = 255/fmaxf(1.f, transformY);

                    for(int y=drawStartY; y<drawEndY; ++y) {
                        int d = (y-spriteScreenZ) * 256 - H * 128 + spriteHeight * 128; // 256 and 128 factors to avoid floats
                        int texY = ((d * SPR_HEIGHT) / spriteHeight) / 256;

                        uint16_t color = sprite.texture[SPR_WIDTH * texY + texX];
                        if (color != TRANSP) {
                            if(x == W_2) {
                                int idx = sprite.entity_idx;
                                if(EntityManager::entities[idx]->isDead() == false)
                                    player.spriteAtCenter = idx;
                            }
                            Screen::drawPixel(x, y, blend(color, fog_color, blendDist));
                        }
                    }
                }
            }
        }        
    }
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

        float step = float(TEX_HEIGHT) / float(lineHeight);
        float texPos = (drawStart - H_2 + lineHeight / 2) * step;

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

            Screen::drawPixel(x, y, blend(c, fog_color, blendDist));
        }

        Zbuffer[x] = perpDistance;
    }
}

void Camera::drawFloor() {
    for(int y=H_2; y<H; ++y) {
        float rayDirX0 = dir.x - plane.x;
        float rayDirY0 = dir.y - plane.y;
        float rayDirX1 = dir.x + plane.x;
        float rayDirY1 = dir.y + plane.y;

        float rowDistance = float(H_2) / (y - H_2);
        uint8_t blendDist = 255*(float(y)/float(H_2)-1);

        float floorStepX = (rowDistance * 2.0f * plane.x) / W;
        float floorStepY = (rowDistance * 2.0f * plane.y) / W;

        float floorX = pos.x + rowDistance * rayDirX0;
        float floorY = pos.y + rowDistance * rayDirY0;

        for(int x=0; x<W; ++x) {
            int cellX = (int)(floorX);
            int cellY = (int)(floorY);

            int tx = int(float(FLOOR_WIDTH) * (floorX - cellX)) & (FLOOR_WIDTH-1);
            int ty = int(float(FLOOR_HEIGHT) * (floorY - cellY)) & (FLOOR_HEIGHT-1);

            floorX += floorStepX;
            floorY += floorStepY;
            
            //Screen::drawPixel(x, H-y-1, blend(floor_wood[FLOOR_WIDTH*ty + tx], fog_color, blendDist));
            Screen::drawPixel(x, y, blend(floor_texture[FLOOR_WIDTH*ty + tx], fog_color, blendDist));
        }
    }
}