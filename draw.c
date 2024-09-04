//TODO
//make the w and h flexible
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <stdio.h>
#include <math.h>

#define w_i 25
#define h_i 25

enum state {
    COLORS,
    NUMBERS
};

SDL_Color White = {255, 255, 255};

SDL_Rect Message_rect;

SDL_Surface* surfaceMessage;
SDL_Texture* Message;

enum state STATE;
char img[h_i][w_i] = {0};

int selected_num = 0;

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

void fillArray(char img[h_i][w_i]){
    for(int i = 0;i<h_i;i++){
        for(int j = 0;j<w_i;j++){
            img[i][j] = 0;
        }
    }
    
}

void save_img(int i){
    char name[50];
    sprintf(name, "tagged_img/test%i_%i.pbm",i,selected_num);
    FILE *file = fopen(name, "w");


    char header[15];
    sprintf(header,"P1\n%i %i\n",w_i,h_i);
    //why 6? I don't know
    char num[6];

    fwrite(header,  11, 1, file);

    for(int i = 0;i<h_i;i++){
        //has to be relative to w/h
        for(int j = 0;j<w_i;j++){
            sprintf(num, "%i", img[i][j]);
            fwrite(num, sizeof num[0], 1, file);
        }
        sprintf(num, "\n");
        fwrite(num, sizeof num[0], 1, file);
    }
}

void convert_to_bitmap(int i){
    char name[1000];
    sprintf(name, "tagged_img/test%i_%i.img",i,selected_num);
    FILE *file = fopen(name, "w");
    char num[6];
    for(int i = 0;i<h_i;i++){
        for(int j = 0;j<w_i;j++){
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
#define NBR_NUMBER 10

int color_pos_pos[NBR_NUMBER][2] = {{0}};

int check_collision(int x, int y, SDL_Renderer *renderer){
    TTF_Font* Sans = TTF_OpenFont("./NotoSans-Regular.ttf",100);
    int it = STATE == COLORS ? NBR_COLORS: NBR_NUMBER;
    for(int i=0;i<it;i++){
        if(x >= color_pos_pos[i][0] - 20 && x <= color_pos_pos[i][0] + 20){
            if(y >= color_pos_pos[i][1] - 15 && y <= color_pos_pos[i][1] + 15){
                selected_num = i;
                return i; 
            }
        }
    }
    return -1;
}


void draw_colors(SDL_Renderer *renderer){
    TTF_Font* Sans = TTF_OpenFont("./NotoSans-Regular.ttf",100);

    int it = STATE == COLORS ? NBR_COLORS: NBR_NUMBER;
    for(short i=0;i<it;i++){
        SDL_SetRenderDrawColor(renderer, color_pos[i][0], color_pos[i][1], color_pos[i][2], 255);
        if(STATE == COLORS){
            drawCircle(renderer, 30, 50*(i+1), 2);
            color_pos_pos[i][0] = 30;
            color_pos_pos[i][1] = 50*(i+1);
        }else if(STATE == NUMBERS){
            Message_rect.x = 20;
            Message_rect.y = 30*i;
            Message_rect.w = 30;
            Message_rect.h = 30;
            char itoc[7];
            sprintf(itoc, "%i", i);
            surfaceMessage = TTF_RenderText_Solid(Sans, itoc, White); 
            Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
            color_pos_pos[i][0] = 20;
            color_pos_pos[i][1] = 15+30*(i);
        }
            
    }

    /*SDL_FreeSurface(surfaceMessage);*/
    SDL_DestroyTexture(Message);
    TTF_CloseFont(Sans);
}



int main(int argc, char *argv[]) {
    fillArray(img);
    TTF_Init();
    if(argc <= 1){
        STATE = COLORS;
    }else{
        STATE = NUMBERS;
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Draw Circle Around Mouse Position", 
                                       SDL_WINDOWPOS_CENTERED, 
                                       SDL_WINDOWPOS_CENTERED, 
                                       w_i, h_i, 
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
    int radius = 1; 
                
    const int targetFPS = 50;
    const int frameDelay = 1000 / targetFPS;  

    Uint32 frameStart;
    int frameTime;

    int color = -1;
    
    /*draw_colors(renderer);*/

    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);

    int save_i = 0;
    while (!quit) {
        frameStart = SDL_GetTicks();  

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q) {
                printf("actually quit i guess\n");
                quit = 1;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_s) {
                printf("saving");
                /*save_img(save_i);    */
                convert_to_bitmap(save_i);
                fillArray(img);
                save_i++;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                /*draw_colors(renderer);*/
                SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
            } else if (e.type == SDL_MOUSEMOTION && (SDL_GetMouseState(&mouseX,&mouseY) & SDL_BUTTON_LMASK)) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
                if(mouseX<(w_i) && mouseX>0 && mouseY<h_i && mouseY>0){
                    drawCircle(renderer, mouseX, mouseY, radius);
                }
            }else if(SDL_GetMouseState(&mouseX,&mouseY) & SDL_BUTTON_LMASK){
                /*if((color = check_collision(mouseX,mouseY, renderer)) != -1){*/
                    /*SDL_SetRenderDrawColor(renderer, color_pos[color][0],color_pos[color][1],color_pos[color][2],255);*/
                /*}*/
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

