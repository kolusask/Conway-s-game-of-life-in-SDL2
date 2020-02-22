#define WIDTH 950
#define HEIGHT 600
#define CELL_SIZE 1 

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

#define NPOINTS 3
const int gPoints[NPOINTS][2] = {{2, 2}, {3, 3}, {2, 4}};

#define DELAY 9

#define RANDOM

void init_graphics(void) {
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow("Conway's game of life", SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
}

void quit_graphics(void) {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

void clear_array(bool arr[WIDTH][HEIGHT]) {
    for(int i = 0; i < WIDTH; i++) 
        for(int j = 0; j < HEIGHT; j++) 
            arr[i][j] = false;
}

void place_points(bool arr[WIDTH][HEIGHT]) {
    #ifndef RANDOM
    for(int i = 0; i < NPOINTS; i++) 
        arr[gPoints[i][0]][gPoints[i][1]] = true;
    #else
    for(int i = 0; i < WIDTH; i++) 
        for(int j = 0; j < HEIGHT; j++)
            arr[i][j] = rand() % 2;
    #endif
}

bool get_alive(const bool field[WIDTH][HEIGHT], int x, int y) {
    if(x == -1)
        x = WIDTH - 1;
    else if(x == WIDTH)
        x = 0;
    if(y == -1)
        y = HEIGHT - 1;
    else if(y == HEIGHT)
        y = 0;
    return field[x][y];
}

int count_neighbors(const bool field[WIDTH][HEIGHT], const int x, const int y) {
    int result = 0;
    for(int i = -1; i <= 1; i++)
        for(int j = -1; j <= 1; j++)
            if(i || j)
                result += get_alive(field, x + i, y + j);
    return result;
}

void copy_array(bool dst[WIDTH][HEIGHT], const bool src[WIDTH][HEIGHT]) {
    for(int i = 0; i < WIDTH; i++)
        for(int j = 0; j < HEIGHT; j++)
            dst[i][j] = src[i][j];
}

void draw(const bool arr[WIDTH][HEIGHT]) {
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            if(arr[i][j]) {
                SDL_Rect pointRect = {i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(gRenderer, &pointRect);
            }
        }
    }
    SDL_RenderPresent(gRenderer);
}

bool update(bool field[WIDTH][HEIGHT]) {
    SDL_Event e;
    SDL_PollEvent(&e);
    if(e.type == SDL_QUIT) return false;
    bool temp[WIDTH][HEIGHT];
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            int nNeighbors = count_neighbors(field, i, j);
            temp[i][j] = (nNeighbors == 3) ? true : (nNeighbors == 2) ? field[i][j] : false;
        }
    }
    clear_array(field);
    copy_array(field, temp);
    draw(field);
    return true;
}

int main(void) {
    init_graphics();
    srand(time(NULL));
    bool field[WIDTH][HEIGHT];
    clear_array(field);
    place_points(field);
    draw(field);
    while(update(field))
        SDL_Delay(DELAY);
    quit_graphics();
    return 0;
}
