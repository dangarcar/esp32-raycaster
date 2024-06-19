#pragma once

#include <stdint.h>
#include <stddef.h>
#include "sprites.h"
#include "../map.h"

#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)

struct Color {
    int r,g,b;

    Color(uint16_t bit) {
        r = (bit & 0xf800) >> 8;
        g = (bit & 0x7e0) >> 3;
        b = (bit & 0x1f) << 3;
    }

    uint16_t to16Bit() {
        return uint16_t((r << 8) & 0xf800) | uint16_t((g << 3) & 0x7e0) | uint16_t((b >> 3) & 0x1f);
    }
};

Color operator*(Color c, float f) {
        c.r *= f;
        c.g *= f;
        c.b *= f;

        return c;
}

Color operator+(Color a, Color b) {
    a.r = __min(255, b.r+a.r);
    a.g = __min(255, b.g+a.g);
    a.b = __min(255, b.b+a.b);

    return a;
}

namespace Screen {
    static const size_t SCREEN_WIDTH = 240;
    static const size_t SCREEN_HEIGHT = 160;
    static const size_t SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

    static uint16_t _screen[SCREEN_SIZE];

    inline void clear() {
        for(int i=0; i<SCREEN_SIZE; ++i)
            _screen[i] = fogColor;
    }

    //Also renders the image
    inline void fillScreen(const uint16_t* image) {
        memcpy(_screen, image, SCREEN_SIZE*sizeof(uint16_t));
    }

    inline void fillRect(int x, int y, int w, int h, uint16_t color){
        for(int i=0; i<h; ++i) {
            for(int j=0; j<w; ++j) {
                _screen[(y+i)*SCREEN_WIDTH + j+x] = color;
            }
        }
    }

    inline void drawTexture(int x, int y, SpriteTexture tex){
        for(int i=0; i<tex.h; ++i) {
            for(int j=0; j<tex.w; ++j) {
                uint16_t c = tex.tex[i*tex.w + j];
                if(c != TRANSP)
                    _screen[(y+i)*SCREEN_WIDTH + j+x] = c;
            }
        }
    }

    inline void drawFastVLine(int x, int y, int len, uint16_t color) {
        for(int i=0; i<len; ++i) {
            _screen[(y+i)*SCREEN_WIDTH + x] = color;
        }
    }

    inline void drawFastHLine(int x, int y, int len, uint16_t color) {
        for(int i=0; i<len; ++i) {
            _screen[y*SCREEN_WIDTH + x+i] = color;
        }
    }

    inline void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
        bool steep = abs(y1 - y0) > abs(x1 - x0);
        if (steep) {
            SWAP(int, x0, y0);
            SWAP(int, x1, y1);
        }

        if (x0 > x1) {
            SWAP(int, x0, x1);
            SWAP(int, y0, y1);
        }

        int dx = x1 - x0, dy = abs(y1 - y0);

        int err = dx/2, ystep = -1, xs = x0, dlen = 0;

        if (y0 < y1) ystep = 1;

        if (steep) {
            for (; x0 <= x1; x0++) {
                dlen++;
                err -= dy;
                if (err < 0) {
                    if (dlen == 1) _screen[xs*SCREEN_WIDTH + y0] = color;
                    else drawFastVLine(y0, xs, dlen, color);
                    dlen = 0;
                    y0 += ystep; xs = x0 + 1;
                    err += dx;
                }
            }
            if (dlen) 
                drawFastVLine(y0, xs, dlen, color);
        }
        else {
            for (; x0 <= x1; x0++) {
                dlen++;
                err -= dy;
                if (err < 0) {
                    if (dlen == 1) _screen[y0*SCREEN_WIDTH + xs] = color;
                    else drawFastHLine(xs, y0, dlen, color);
                    dlen = 0;
                    y0 += ystep; xs = x0 + 1;
                    err += dx;
                }
            }
            if (dlen) 
                drawFastHLine(xs, y0, dlen, color);
        }
    }

    inline void drawPixel(int x, int y, uint16_t color) {
        _screen[y*SCREEN_WIDTH + x] = color;
    }

    inline void greenify() {
        for(int i=0; i<Screen::SCREEN_SIZE; ++i) {
            _screen[i] |= (_screen[i] << 1) & 0x7E0;
        }
    }

    inline void redify() {
        for(int i=0; i<Screen::SCREEN_SIZE; ++i) {
            _screen[i] |= (_screen[i] << 1) & 0xF800;
        }
    }

    inline void blueify() {
        for(int i=0; i<Screen::SCREEN_SIZE; ++i) {
            _screen[i] |= (_screen[i] << 1) & 0x1F;
        }
    }
};