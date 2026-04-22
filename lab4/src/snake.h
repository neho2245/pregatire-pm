#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>

#define CELL_SIZE    8
#define GRID_COLS    (ST7735_WIDTH  / CELL_SIZE)   // 16
#define GRID_ROWS    (ST7735_HEIGHT / CELL_SIZE)    // 20
#define MAX_SNAKE    (GRID_COLS * GRID_ROWS)        // 320

typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
} direction_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} point_t;

typedef struct {
    point_t     body[MAX_SNAKE];
    uint16_t    length;
    direction_t dir;
    direction_t next_dir;
    point_t     food;
    uint8_t     game_over;
    uint16_t    score;
} snake_game_t;

void snake_init(snake_game_t *g);
void snake_tick(snake_game_t *g);
void snake_set_dir(snake_game_t *g, direction_t d);
void snake_draw(const snake_game_t *g);
void snake_draw_cell(uint8_t gx, uint8_t gy, uint16_t color);
void snake_game_over(uint16_t score);

#endif
