#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <stdio.h>
#include <math.h>

#define w 400
#define h 300

char img[h][w] = {0};

void putPixel(SDL_Renderer *renderer, int x, int y) {
    SDL_RenderDrawPoint(renderer, x, y);
}

void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius) {
    for (int angle = 0; angle < 360; angle++) {
        for(int rad = 0;rad < radius; rad++){
            int x = centerX + rad * cos(angle * M_PI / 180);
            int y = centerY + rad * sin(angle * M_PI / 180);
            putPixel(renderer, x, y);
            if(img[y][x] == 1)
                continue;

            img[y][x] = 1;
        }
    }
}

void fillArray(char img[h][w]){
    for(int i = 0;i<h;i++){
        for(int j = 0;j<w;j++){
            img[i][j] = 0;
        }
    }
    
}

void save_img(){
    FILE *file = fopen("./test.pbm", "w");

    char *header = "P1\n300 300\n";
    //why 6? I don't know
    char num[6];

    fwrite(header,  11, 1, file);

    for(int i = 0;i<h;i++){
        for(int j = 100;j<w;j++){
            sprintf(num, "%i", img[i][j]);
            fwrite(num, sizeof num[0], 1, file);
        }
        sprintf(num, "\n");
        fwrite(num, sizeof num[0], 1, file);
    }
}

void convert_to_bitmap(){
    FILE *file = fopen("./test.img", "w");
    char num[6];
    for(int i = 0;i<h;i++){
        for(int j = 100;j<w;j++){
            sprintf(num, "%i", img[i][j]);
            fwrite(num, sizeof num[0], 1, file);
        }
    }
}

#define NBR_COLORS 5
int color_pos[NBR_COLORS][3] = {
    {0,200,200},
    {0,200,0},
    {0,0,200},
    {200,0,200},
    {200,0,0},
};

int color_pos_pos[NBR_COLORS][2] = {{0}};

int check_collision(int x, int y){
    for(int i=0;i<NBR_COLORS;i++){
        if(x >= color_pos_pos[i][0] - 20 && x <= color_pos_pos[i][0] + 20){
            if(y >= color_pos_pos[i][1] - 20 && y <= color_pos_pos[i][1] + 20){
                return i; 
            }
        }
    }
    return -1;
}

void draw_colors(SDL_Renderer *renderer){
    for(int i=0;i<NBR_COLORS;i++){
        SDL_SetRenderDrawColor(renderer, color_pos[i][0], color_pos[i][1], color_pos[i][2], 255);
        drawCircle(renderer, 30, 50*(i+1), 20);
        color_pos_pos[i][0] =  30;
        color_pos_pos[i][1] =  50*(i+1) ;
    }
}

int main(int argc, char *argv[]) {
    fillArray(img);
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Draw Circle Around Mouse Position", 
                                       SDL_WINDOWPOS_CENTERED, 
                                       SDL_WINDOWPOS_CENTERED, 
                                       w, h, 
                                       SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color red = {255, 0, 0, 255};

    SDL_Event e;
    int quit = 0;
    int mouseX, mouseY;
    int radius = 5; 
                
    const int targetFPS = 50;
    const int frameDelay = 1000 / targetFPS;  
    int color = -1;

    Uint32 frameStart;
    int frameTime;

    draw_colors(renderer);

    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);

    while (!quit) {
        frameStart = SDL_GetTicks();  

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q) {
                printf("actually quit i guess\n");
                quit = 1;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_s) {
                printf("saving");
                save_img();    
                convert_to_bitmap();
            } else if (e.type == SDL_MOUSEMOTION && (SDL_GetMouseState(&mouseX,&mouseY) & SDL_BUTTON_LMASK)) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
                if(mouseX<(w) && mouseX>100 && mouseY<h && mouseY>0){
                    drawCircle(renderer, mouseX, mouseY, radius);
                }
            }else if(SDL_GetMouseState(&mouseX,&mouseY) & SDL_BUTTON_LMASK){
                if((color = check_collision(mouseX,mouseY)) != -1){
                    SDL_SetRenderDrawColor(renderer, color_pos[color][0],color_pos[color][1],color_pos[color][2],255);
                }
            }
        }
        frameTime = SDL_GetTicks() - frameStart;
        SDL_RenderPresent(renderer);
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

