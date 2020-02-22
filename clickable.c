#define WIDTH 100
#define HEIGHT 100
#define CELL_SIZE 7 

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
bool readMouse = true;

#define DELAY 10

typedef enum { INP_START, INP_WAIT, INP_ESCAPE } inputs;

void init_graphics(void) {
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow("Conway's game of life", SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);
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

void draw_grid() {
    SDL_SetRenderDrawColor(gRenderer, 0xF0, 0xF0, 0xF0, 0xFF);
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            SDL_Rect rect = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_RenderDrawRect(gRenderer, &rect);
        }
    }
    SDL_RenderPresent(gRenderer);
}

void draw(const bool arr[WIDTH][HEIGHT], const bool showGrid) {
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
    if(showGrid) draw_grid();
    else SDL_RenderPresent(gRenderer);
}

inputs update(bool field[WIDTH][HEIGHT]) {
    SDL_Event e;
    SDL_PollEvent(&e);
    switch(e.type) {
        case SDL_KEYDOWN:
            switch(e.key.keysym.sym) {
                case SDLK_SPACE:
                    return INP_START;
                case SDLK_ESCAPE:
                    return INP_ESCAPE;
                default: break;
            }
        case SDL_QUIT: return INP_ESCAPE;
        default: break;
    }
    bool temp[WIDTH][HEIGHT];
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            int nNeighbors = count_neighbors(field, i, j);
            temp[i][j] = (nNeighbors == 3) ? true : (nNeighbors == 2) ? field[i][j] : false;
        }
    }
    clear_array(field);
    copy_array(field, temp);
    draw(field, false);
    return true;
}

inputs edit(bool field[WIDTH][HEIGHT]) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type) {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
                case SDLK_SPACE: return INP_START;
                case SDLK_ESCAPE: return INP_ESCAPE;
                case SDLK_DELETE:
                    clear_array(field);
                    draw(field, true);
                    return INP_WAIT;
                default: return INP_WAIT;
            }
        case SDL_MOUSEBUTTONDOWN:
            if(readMouse) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                field[x / CELL_SIZE][y / CELL_SIZE] = !field[x / CELL_SIZE][y / CELL_SIZE];
                draw(field, true);
                readMouse = false;
            }
            return INP_WAIT;
        case SDL_MOUSEBUTTONUP:
            readMouse = true;
            return INP_WAIT;
        case SDL_QUIT: return INP_ESCAPE;
        default: return INP_WAIT;
    }
}

int main(void) {
    init_graphics();
    bool field[WIDTH][HEIGHT];
    clear_array(field);
    inputs inp;
    while(true) {
        draw(field, true);
        do {
            inp = edit(field);
            SDL_Delay(1);
        } while(inp == INP_WAIT);
        if(inp == INP_START) {
            do {
                inp = update(field);
                SDL_Delay(DELAY);
            } while(inp == INP_WAIT);
        }
        else break;
    }
    quit_graphics();
    return 0;
}
