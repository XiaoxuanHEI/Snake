#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unistd.h>

#define QUEUE_SIZE 400
#define E  1
#define NE 2
#define N  3
#define NW 4
#define W  5
#define SW 6
#define S  7
#define SE 8

#define MAX_X 49
#define MAX_Y 29
#define TILE_SIZE 16

char dir;
char m_dir;
char eaten;
char eatenghost;
char mat[MAX_X+1][MAX_Y+1];

int a = 0;
int b = 0;
int m = 0;
int level = 1;

bool paused;

int delay = 30;
int accelerer = 0;
int counter = 0;
int counter1 = 0; 
int fruitcounter = 0;
int missile_exist = 0;

typedef struct tag_node {
    int x;
    int y;
} node;

node missile;
node body;
node head;
node tail;
node taill;
node fruit;
node mine;
node ghost;
node wall[20];
node position[QUEUE_SIZE];
bool hit;
bool hit_m;

typedef struct tag_queue {
    int first;
    int last;
    int len;
    node elems[QUEUE_SIZE];
} queue;

queue snake;

SDL_Window* window =NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface*  field_surface = NULL;
SDL_Surface*  fruit1_surface = NULL;
SDL_Surface*  fruit2_surface = NULL;
SDL_Surface*  fruit3_surface = NULL;
SDL_Surface*  fruit4_surface = NULL;
SDL_Surface*  mine_surface = NULL;
SDL_Surface*  ghost_surface = NULL;
SDL_Surface*  wall_surface = NULL;
SDL_Surface*  missile_surface = NULL;
SDL_Surface*  sheade_surface = NULL;
SDL_Surface*  sheadse_surface = NULL;
SDL_Surface*  sheads_surface = NULL;
SDL_Surface*  sheadsw_surface = NULL;
SDL_Surface*  sheadw_surface = NULL;
SDL_Surface*  sheadnw_surface = NULL;
SDL_Surface*  sheadn_surface = NULL;
SDL_Surface*  sheadne_surface = NULL;
SDL_Surface*  snake_surface = NULL;

SDL_Texture*  field_texture = NULL;
SDL_Texture*  fruit1_texture = NULL;
SDL_Texture*  fruit2_texture = NULL;
SDL_Texture*  fruit3_texture = NULL;
SDL_Texture*  fruit4_texture = NULL;
SDL_Texture*  mine_texture = NULL;
SDL_Texture*  ghost_texture = NULL;
SDL_Texture*  wall_texture = NULL;
SDL_Texture*  missile_texture = NULL;
SDL_Texture*  sheade_texture = NULL;
SDL_Texture*  sheadse_texture = NULL;
SDL_Texture*  sheads_texture = NULL;
SDL_Texture*  sheadsw_texture = NULL;
SDL_Texture*  sheadw_texture = NULL;
SDL_Texture*  sheadnw_texture = NULL;
SDL_Texture*  sheadn_texture = NULL;
SDL_Texture*  sheadne_texture = NULL;
SDL_Texture*  snake_texture = NULL;

Mix_Music* sound_eat = NULL;
Mix_Music* sound_mine = NULL;
Mix_Music* sound_hit = NULL;
Mix_Music* sound_ghost = NULL;
Mix_Music* sound_missile = NULL;
Mix_Music* sound_hit_m = NULL;
Mix_Music* sound_over = NULL;

void init(void);
void input(void);
int  update(void);
void render(void);
void pop_tail(void);
void push_head(void);
void push_missile(void);
void gameover(void);
void draw_body(void);
void draw_head(void);
void draw_fruit(void);
void draw_mine(void);
void draw_ghost(void);
void draw_wall(void);
void draw_missile(void);
void clear_mine(void);
void clear_tail(void);
void clear_taill(void);
void clear_missile(void);
void next_fruit(void);
void next_mine(void);
void next_ghost(void);
void fire(void);
void eat_tail(void);
void cut_tail(void);

int main(void)
{
    init();
    render();
    SDL_Event event;
    
    for (;;) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT){
                exit(0);
            }else if(event.type ==SDL_MOUSEBUTTONDOWN){
                if(event.button.button == SDL_BUTTON_LEFT) {
                    paused = true;
                }else if(event.button.button == SDL_BUTTON_RIGHT) {
                    paused = false;
                }
            }
        }
        if(counter1 == 3){
            counter++;
            if(!paused){
                input();
                if (update()) {
                    gameover();
                }
                if(counter == 20){
                    clear_mine();
                    next_mine();
                    draw_mine();
                    counter = 0;
                }
                render();
            }
            counter1 = 0;
        } else{
            counter1 ++;
            if(missile_exist){
                push_missile();
            }
        }
        
        if(accelerer == 0){
            SDL_Delay(delay * 2);
        }else{
            SDL_Delay(delay * 1);
        }
    }
    return 0;
}

void init(void)
{
    int i, j;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    }
    atexit(SDL_Quit);
    SDL_CreateWindowAndRenderer(800, 480, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    sound_eat = Mix_LoadMUS("eat.wav");
    sound_mine = Mix_LoadMUS("mine.wav");
    sound_hit = Mix_LoadMUS("hit.wav");
    sound_ghost = Mix_LoadMUS("ghost.mp3");
    sound_missile = Mix_LoadMUS("missile.mp3");
    sound_hit_m = Mix_LoadMUS("hit_m.mp3");
    sound_over = Mix_LoadMUS("over.mp3");
    
    fruit.x = 5;
    fruit.y = 5;
    mine.x = 10;
    mine.y = 10;
    ghost.x = 5;
    ghost.y = 25;
    head = fruit;
    if (head.x < (MAX_X / 2)) {
        dir = E;
    } else {
        dir = W;
    }
    fruit1_surface = SDL_LoadBMP("fruit1.bmp");
    fruit2_surface = SDL_LoadBMP("fruit2.bmp");
    fruit3_surface = SDL_LoadBMP("fruit3.bmp");
    fruit4_surface = SDL_LoadBMP("fruit4.bmp");
    mine_surface = SDL_LoadBMP("mine.bmp");
    ghost_surface = SDL_LoadBMP("ghost.bmp");
    wall_surface = SDL_LoadBMP("wall.bmp");
    missile_surface = SDL_LoadBMP("missile.bmp");
    sheade_surface = SDL_LoadBMP("heade.bmp");
    sheadse_surface = SDL_LoadBMP("headse.bmp");
    sheads_surface = SDL_LoadBMP("heads.bmp");
    sheadsw_surface = SDL_LoadBMP("headsw.bmp");
    sheadw_surface = SDL_LoadBMP("headw.bmp");
    sheadnw_surface = SDL_LoadBMP("headnw.bmp");
    sheadn_surface = SDL_LoadBMP("headn.bmp");
    sheadne_surface = SDL_LoadBMP("headne.bmp");
    snake_surface = SDL_LoadBMP("body.bmp");
    field_surface = SDL_LoadBMP("field.bmp");
    fruit1_texture = SDL_CreateTextureFromSurface(renderer, fruit1_surface);
    fruit2_texture = SDL_CreateTextureFromSurface(renderer, fruit2_surface);
    fruit3_texture = SDL_CreateTextureFromSurface(renderer, fruit3_surface);
    fruit4_texture = SDL_CreateTextureFromSurface(renderer, fruit4_surface);
    mine_texture = SDL_CreateTextureFromSurface(renderer, mine_surface);
    ghost_texture = SDL_CreateTextureFromSurface(renderer, ghost_surface);
    wall_texture = SDL_CreateTextureFromSurface(renderer, wall_surface);
    missile_texture = SDL_CreateTextureFromSurface(renderer, missile_surface);
    sheade_texture = SDL_CreateTextureFromSurface(renderer, sheade_surface);
    sheadse_texture = SDL_CreateTextureFromSurface(renderer, sheadse_surface);
    sheads_texture = SDL_CreateTextureFromSurface(renderer, sheads_surface);
    sheadsw_texture = SDL_CreateTextureFromSurface(renderer, sheadsw_surface);
    sheadw_texture = SDL_CreateTextureFromSurface(renderer, sheadw_surface);
    sheadnw_texture = SDL_CreateTextureFromSurface(renderer, sheadnw_surface);
    sheadn_texture = SDL_CreateTextureFromSurface(renderer, sheadn_surface);
    sheadne_texture = SDL_CreateTextureFromSurface(renderer, sheadne_surface);
    snake_texture = SDL_CreateTextureFromSurface(renderer, snake_surface);
    field_texture = SDL_CreateTextureFromSurface(renderer, field_surface);
    for (i = 0; i <= MAX_X; i++) {
        for (j = 0; j <= MAX_Y; j++) {
            tail.x = i;
            tail.y = j;
            clear_tail();
        }
    }
    snake.first = 0;
    snake.last = 0;
    snake.len = 0;
    
    push_head();
    next_fruit();
    draw_mine();
    draw_wall();
    draw_ghost();
    
    eaten = 1;
    printf("Level 1\n");
}

void input(void)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    SDL_PumpEvents();
    if (state[SDL_SCANCODE_UP]) {
        accelerer = 1;
    } else if (state[SDL_SCANCODE_DOWN]) {
        accelerer = 0;
    } else if (state[SDL_SCANCODE_LEFT]) {
        switch (dir) {
            case E:
                dir = NE;
                break;
            case NE:
                dir = N;
                break;
            case N:
                dir = NW;
                break;
            case NW:
                dir = W;
                break;
            case W:
                dir = SW;
                break;
            case SW:
                dir = S;
                break;
            case S:
                dir = SE;
                break;
            case SE:
                dir = E;
                break;
        }
    } else if (state[SDL_SCANCODE_RIGHT]) {
        switch(dir){
            case E:
                dir = SE;
                break;
            case SE:
                dir = S;
                break;
            case S:
                dir = SW;
                break;
            case SW:
                dir = W;
                break;
            case W:
                dir = NW;
                break;
            case NW:
                dir = N;
                break;
            case N:
                dir = NE;
                break;
            case NE:
                dir = E;
                break;
        }
    } else if (state[SDL_SCANCODE_ESCAPE]) {
        exit(0);
    } else if (state[SDL_SCANCODE_A]) {
        fire();
        missile_exist = 1;
    }  else if(state[SDL_SCANCODE_Q]){
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    else if(state[SDL_SCANCODE_W]){
        SDL_SetWindowFullscreen(window, 0);
    }
}

int update(void)
{
    body = head;
    
    if((snake.len >= 0)&&(snake.len <= 10)){
        delay = 30;
        if(level != 1){
            level =1;
            printf("Level 1 !\n");
        }
    }else if ((snake.len > 10)&&(snake.len <= 20)){
        delay = 20;
        if(level != 2){
            level = 2;
            printf("Level 2 !\n");
        }
    }else if ((snake.len > 20)&&(snake.len <= 30)){
        delay = 15;
        if(level != 3){
            level =3;
            printf("Level 3 !\n");
        }
    }else if ((snake.len > 30)&&(snake.len <= 40)){
        delay = 10;
        if(level != 4){
            level = 4;
            printf("Level 4 !\n");
        }
    }else{
        delay = 6;
        if(level != 5){
            level = 5;
            printf("Level 5 !\n");
        }
    }
    
    switch (dir) {
        case E:
            head.x = head.x + 1;
            break;
        case NE:
            head.y = head.y - 1;
            head.x = head.x + 1;
            break;
        case N:
            head.y = head.y - 1;
            break;
        case NW:
            head.x = head.x - 1;
            head.y = head.y - 1;
            break;
        case W:
            head.x = head.x - 1;
            break;
        case SW:
            head.x = head.x - 1;
            head.y = head.y + 1;
            break;
        case S:
            head.y = head.y + 1;
            break;
        case SE:
            head.x = head.x + 1;
            head.y = head.y + 1;
            break;
    }
    if (head.x > MAX_X)	{
        head.x = 0;
    }
    if (head.x < 0)	{
        head.x = MAX_X;
    }
    if (head.y > MAX_Y)	{
        head.y = 0;
    }
    if (head.y < 0)	{
        head.y = MAX_Y;
    }
    if (mat[head.x][head.y]) {
        eat_tail();
        return 0;
    }
    
    hit = false;
    if((head.x>=wall[0].x-1)&&(head.x<=wall[19].x+1)&&(head.y>=wall[0].y-1)&&(head.y<=wall[0].y+1)){
        if((head.x>=wall[0].x-1)&&(head.x<=wall[19].x+1)&&(head.y==wall[0].y-1||head.y==wall[0].y+1)&&(dir==E||dir==W))
            hit = false;
        else if(((head.x==wall[0].x-1)||(head.x==wall[19].x+1))&&((head.y>=wall[0].y-1)&&(head.y<=wall[0].y+1))&&(dir==N||dir==S))
            hit = false;
        else {
            hit = true;
            Mix_PlayMusic(sound_hit,1);
        }
    }
    
    
    if (head.x == fruit.x && head.y == fruit.y) {
        Mix_PlayMusic(sound_eat, 1);
        next_fruit();
        eaten = 1;
    }
    else if(hit){
        switch (dir) {
            case E:
                if(snake.len != 1){
                    return 1;
                }else{
                    dir = W;
                }
                break;
            case NE:
                if((head.x == wall[0].x-1)&&(head.y == wall[0].y-1)){
                    
                } else if ((head.x == wall[0].x-1)&&(head.y == wall[0].y)){
                    dir = NW;
                } else if ((head.x == wall[0].x-1)&&(head.y == wall[0].y+1)){
                    if(snake.len != 1){
                        return 1;
                    }else{
                        dir = SW;
                    }
                } else if ((head.x == wall[19].x+1)&&(head.y == wall[19].y+1)){
                    
                } else{
                    dir = SE;
                }
                break;
            case N:
                if(snake.len != 1){
                    return 1;
                }else{
                    dir = S;
                }
                break;
            case NW:
                if((head.x == wall[0].x-1)&&(head.y == wall[0].y+1)){
                    
                } else if ((head.x == wall[19].x+1)&&(head.y == wall[19].y+1)){
                    if(snake.len != 1){
                        return 1;
                    }else{
                        dir = SE;
                    }
                } else if ((head.x == wall[19].x+1)&&(head.y == wall[19].y)){
                    dir = NE;
                } else if ((head.x == wall[19].x+1)&&(head.y == wall[19].y-1)){
                    
                }else{
                    dir = SW;
                }
                break;
            case W:
                if(snake.len != 1){
                    return 1;
                }else{
                    dir = E;
                }
                break;
            case SW:
                if((head.x == wall[0].x-1)&&(head.y == wall[0].y-1)){
                    
                } else if ((head.x == wall[19].x+1)&&(head.y == wall[19].y-1)){
                    if(snake.len != 1){
                        return 1;
                    }else{
                        dir = NE;
                    }
                } else if ((head.x == wall[19].x+1)&&(head.y == wall[19].y)){
                    dir = SE;
                } else if ((head.x == wall[19].x+1)&&(head.y == wall[19].y+1)){
                    
                } else {
                    dir = NW;
                }
                break;
            case S:
                if(snake.len != 1){
                    return 1;
                }else{
                    dir = N;
                }
                break;
            case SE:
                if((head.x == wall[19].x+1)&&(head.y == wall[19].y-1)){
                    
                } else if ((head.x == wall[0].x-1)&&(head.y == wall[0].y-1)){
                    if(snake.len != 1){
                        return 1;
                    }else{
                        dir = NW;
                    }
                } else if ((head.x == wall[0].x-1)&&(head.y == wall[0].y)){
                    dir = SW;
                } else if ((head.x == wall[0].x-1)&&(head.y == wall[0].y+1)){
                    
                } else {
                    dir = NE;
                }
                break;
        }
        pop_tail();
    }
    else if (head.x == mine.x && head.y == mine.y) {
        Mix_PlayMusic(sound_mine,1);
        return 1;
    }   else if (head.x == ghost.x && head.y == ghost.y) {
        Mix_PlayMusic(sound_ghost,1);
        next_ghost();
        eatenghost = 1;
        pop_tail();
        pop_tail();
    } 	else {
        pop_tail();
        eatenghost = 0;
        eaten = 0;
    }
    
    push_head();
    return 0;
}

void render(void)
{
    if (snake.len > 1) {
        draw_body();
    }
    if (eaten) {
        draw_fruit();
    } else {
        clear_tail();
    }
    if (eatenghost){
        clear_taill();
        clear_tail();
        draw_ghost();
        
    } else{
        clear_tail();
    }
    draw_head();
    SDL_RenderPresent(renderer);
}



void fire(void)
{
    missile.x = head.x;
    missile.y = head.y;
    m_dir = dir;
    Mix_PlayMusic(sound_missile,1);
    push_missile();
}

void eat_tail(void)
{
    for(int i = b; i < a; i++){
        if((position[i].x == head.x)&&(position[i].y == head.y)){
            m = i;
        }
    }
    cut_tail();
}

void pop_tail(void)
{
    taill = tail;
    tail = snake.elems[snake.first];
    position[b].x = 0;
    position[b].y = 0;
    b = (b + 1) % QUEUE_SIZE;
    snake.first = (snake.first + 1) % QUEUE_SIZE;
    snake.len--;
    if(snake.len < 0){
        gameover();
    }
    mat[tail.x][tail.y] = 0;
}

void push_head(void)
{
    position[a].x = head.x;
    position[a].y = head.y;
    a = (a + 1) % QUEUE_SIZE;
    snake.elems[snake.last] = head;
    snake.last = (snake.last + 1) % QUEUE_SIZE;
    snake.len++;
    mat[head.x][head.y] = 1;
}

void push_missile(void)
{
    clear_missile();
    
    if (missile.x < 0 || missile.x > MAX_X || missile.y < 0 || missile.y > MAX_Y) {
        missile_exist = 0;
    }
    
    switch (m_dir) {
        case E:
            missile.x = missile.x + 1;
            break;
        case NE:
            missile.y = missile.y - 1;
            missile.x = missile.x + 1;
            break;
        case N:
            missile.y = missile.y - 1;
            break;
        case NW:
            missile.x = missile.x - 1;
            missile.y = missile.y - 1;
            break;
        case W:
            missile.x = missile.x - 1;
            break;
        case SW:
            missile.x = missile.x - 1;
            missile.y = missile.y + 1;
            break;
        case S:
            missile.y = missile.y + 1;
            break;
        case SE:
            missile.x = missile.x + 1;
            missile.y = missile.y + 1;
            break;
    }
    
    hit_m = false;
    if((missile.x>=wall[0].x-1)&&(missile.x<=wall[19].x+1)&&(missile.y>=wall[0].y-1)&&(missile.y<=wall[0].y+1)){
        if((missile.x>=wall[0].x-1)&&(missile.x<=wall[19].x+1)&&(missile.y==wall[0].y-1||missile.y==wall[0].y+1)&&(dir==E||dir==W))
            hit_m = false;
        else if(((missile.x==wall[0].x-1)||(missile.x==wall[19].x+1))&&((missile.y>=wall[0].y-1)&&(missile.y<=wall[0].y+1))&&(dir==N||dir==S))
            hit_m = false;
        else {
            hit_m = true;
            Mix_PlayMusic(sound_hit_m,1);            
        }
    }
    
    if(hit_m){
        missile_exist = 0;
    }else if(missile.x == fruit.x && missile.y == fruit.y) {
        Mix_PlayMusic(sound_eat, 2);
        clear_missile();
        next_fruit();
        draw_fruit();
        push_head();
        push_head();
        push_head();
        missile_exist = 0;
    }else if(missile.x == ghost.x && missile.y == ghost.y){
        Mix_PlayMusic(sound_eat, 1);
        clear_missile();
        next_ghost();
        draw_ghost();
        push_head();
        missile_exist = 0;
    }else{
        draw_missile();
    }
}

void gameover(void)
{
    printf("Snake Length: %d\n", snake.len);
    printf("Game Over\n");
    sleep(1);
    Mix_PlayMusic(sound_over,1);
    sleep(3);
    exit(0);
}

void draw_body(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = body.x*TILE_SIZE;
    rect.y = body.y*TILE_SIZE;
    SDL_RenderCopy(renderer, snake_texture, NULL, &rect);
}

void draw_head(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = head.x*TILE_SIZE;
    rect.y = head.y*TILE_SIZE;
    switch(dir){
        case E:
            SDL_RenderCopy(renderer, sheade_texture, NULL, &rect);
            break;
            
        case SE:
            SDL_RenderCopy(renderer, sheadse_texture, NULL, &rect);
            break;
            
        case S:
            SDL_RenderCopy(renderer, sheads_texture, NULL, &rect);
            break;
            
        case SW:
            SDL_RenderCopy(renderer, sheadsw_texture, NULL, &rect);
            break;
            
        case W:
            SDL_RenderCopy(renderer, sheadw_texture, NULL, &rect);
            break;
            
        case NW:
            SDL_RenderCopy(renderer, sheadnw_texture, NULL, &rect);
            break;
            
        case N:
            SDL_RenderCopy(renderer, sheadn_texture, NULL, &rect);
            break;
            
        case NE:
            SDL_RenderCopy(renderer, sheadne_texture, NULL, &rect);
            break;
    }
}

void draw_fruit(void)
{
    
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = fruit.x*TILE_SIZE;
    rect.y = fruit.y*TILE_SIZE;
    switch(fruitcounter){
        case 0:
            SDL_RenderCopy(renderer, fruit1_texture, NULL, &rect);
            break;
        case 1:
            SDL_RenderCopy(renderer, fruit2_texture, NULL, &rect);
            break;
        case 2:
            SDL_RenderCopy(renderer, fruit3_texture, NULL, &rect);
            break;
        case 3:
            SDL_RenderCopy(renderer, fruit4_texture, NULL, &rect);
            fruitcounter = 0;
            break;
    }
    fruitcounter ++;
}

void draw_mine(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = mine.x * TILE_SIZE;
    rect.y = mine.y * TILE_SIZE;
    SDL_RenderCopy(renderer, mine_texture, NULL, &rect);
}

void draw_ghost(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = ghost.x * TILE_SIZE;
    rect.y = ghost.y * TILE_SIZE;
    SDL_RenderCopy(renderer, ghost_texture, NULL, &rect);
}

void draw_missile(void)
{
    
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = missile.x * TILE_SIZE;
    rect.y = missile.y * TILE_SIZE;
    SDL_RenderCopy(renderer, missile_texture, NULL, &rect);
}

void draw_wall(void)
{
    for(int i=0;i<sizeof(wall)/sizeof(wall[0]);i++) {
        SDL_Rect rect;
        rect.h = TILE_SIZE;
        rect.w = TILE_SIZE;
        rect.x = (15+i) * TILE_SIZE;
        rect.y = 14 * TILE_SIZE;
        wall[i].x = 15+i;
        wall[i].y = 14;
        SDL_RenderCopy(renderer, wall_texture, NULL, &rect);
    }
}

void clear_missile(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = missile.x * TILE_SIZE;
    rect.y = missile.y * TILE_SIZE;
    SDL_RenderCopy(renderer, field_texture, NULL, &rect);
}

void clear_mine(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = mine.x * TILE_SIZE;
    rect.y = mine.y * TILE_SIZE;
    SDL_RenderCopy(renderer, field_texture, NULL, &rect);
}

void clear_tail(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = tail.x * TILE_SIZE;
    rect.y = tail.y * TILE_SIZE;
    SDL_RenderCopy(renderer, field_texture, NULL, &rect);
}

void clear_taill(void)
{
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = taill.x * TILE_SIZE;
    rect.y = taill.y * TILE_SIZE;
    SDL_RenderCopy(renderer, field_texture, NULL, &rect);
}

void cut_tail(void)
{
    snake.len = snake.len - (m - b);
    snake.first = (snake.first + (m - b)) % QUEUE_SIZE;
    
    for(int i = b; i < m; i++){
        SDL_Rect rect;
        rect.h = TILE_SIZE;
        rect.w = TILE_SIZE;
        rect.x = position[i].x * TILE_SIZE;
        rect.y = position[i].y * TILE_SIZE;
        SDL_RenderCopy(renderer, field_texture, NULL, &rect);
        mat[position[i].x][position[i].y] = 0;
    }
    b = (b + (m - b)) % QUEUE_SIZE;
}

void next_fruit(void)
{
    do {
        fruit.x = (fruit.x * 6 + 1) % (MAX_X + 1);
        fruit.y = (fruit.y * 16 + 1) % (MAX_Y + 1);
    } while (mat[fruit.x][fruit.y]||((mine.x == fruit.x)&&(mine.y == fruit.y))||((fruit.x == ghost.x)&&(fruit.y == ghost.y)));
}

void next_mine(void)
{
    do {
        mine.x = (mine.x * 6 + 1) % (MAX_X + 1);
        mine.y = (mine.y * 16 + 1) % (MAX_Y + 1);
    } while (mat[mine.x][mine.y]||((mine.x == fruit.x)&&(mine.y == fruit.y))||((mine.x == ghost.x)&&(mine.y == ghost.y)));
}

void next_ghost(void)
{
    do {
        ghost.x = (ghost.x * 6 + 1) % (MAX_X + 1);
        ghost.y = (ghost.y * 16 + 1) % (MAX_Y + 1);
    } while (mat[ghost.x][ghost.y]||((ghost.x == fruit.x)&&(ghost.y == fruit.y))||((mine.x == ghost.x)&&(mine.y == ghost.y)));
}
