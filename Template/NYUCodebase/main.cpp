#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <stdlib.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Utilities.h"
#include "Map.h"
using namespace std;
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
// 60 FPS (1.0f/60.0f)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
//Global Setup Parameters
SDL_Window* displayWindow;
unsigned char** levelData;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
ShaderProgram* program;
GLuint mapTexture=0;
//Game States
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_WIN, STATE_LOSE};
int state = STATE_MAIN_MENU;
//Timer
float lastFrameTicks = 0.0f;
float elapsed = 0.0;
//Matrices
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;
//Entity & Map Data
Map level;
vector<Entity> mapEntities;
vector<Entity> enemies;
Entity player(.5,-5.2,1,1);
int gridX;
int gridY;

void Setup(ShaderProgram &program){
    //Load Map File
    string levelFile = "/Users/errolelbasan/Documents/Codes/CS3113/CS3113-Homework/Homework/HW5/PlatformerMap.txt";
    ifstream infile(levelFile);
    string line;
    while (getline(infile, line)) {
        if(line == "[header]") {
            level.readHeader(infile);
        }
        else if(line == "[layer]") {
            level.readLayerData(infile);
        }
        else if(line == "[ObjectsLayer]") {
            level.readEntityData(infile, player);
        }
    }
    /*
    mapTexture = LoadTexture("mapTexture.png");
    SheetSprite mySprite(program, mapTexture, 30, 30, 20, .3);
    player.sprite = &mySprite;
     */
}

void RenderGameLevel(ShaderProgram &program){
    //Rendering
    level.renderLevel(&program, mapTexture, modelMatrix);
    player.sprite->DrawPlayer(modelMatrix, player);
    //Scrolling
    viewMatrix.identity();
    viewMatrix.Translate(-player.x, -player.y, 0);
    program.setViewMatrix(viewMatrix);
}

void UpdateGameLevel(ShaderProgram &program){
    player.collidedBottom = false;
    player.collidedBottom = level.bottomCollision(&player);
    
    if (player.collidedBottom == false) {
        player.velocity_y = 2.2;
        player.acceleration_y += -4.81;
        player.velocity_y = player.lerp(player.velocity_y, 0.0f, FIXED_TIMESTEP * player.friction_y);
        player.velocity_y += player.acceleration_y * FIXED_TIMESTEP;
        player.y += player.velocity_y * FIXED_TIMESTEP;
    }else{
        level.worldToTileCoordinates(player.x, player.y, &gridX, &gridY);
        player.y += ((-level.TILE_SIZE*(gridY))-player.y-(player.sprite->size/2)+.00000001);
        player.collidedBottom = true;
    }
    if(keys[SDL_SCANCODE_SPACE]){
        if(player.collidedBottom == true){
            player.acceleration_y = 4.81;
            player.velocity_y = 2.2;
            player.velocity_y += player.lerp(player.velocity_y, 0.0f, FIXED_TIMESTEP * player.friction_y);
            player.velocity_y += player.acceleration_y * FIXED_TIMESTEP;
            player.y += player.velocity_y * FIXED_TIMESTEP;
            player.collidedBottom = false;
        }
    }
    
    if(keys[SDL_SCANCODE_RIGHT]) {
        player.collidedRight = false;
        player.collidedRight = level.RightCollision(&player);
        if(player.collidedRight == false){
            player.acceleration_x = 1.2;
            player.velocity_x = player.lerp(player.velocity_x, 0.0f, FIXED_TIMESTEP * player.friction_x);
            player.velocity_x += player.acceleration_x * FIXED_TIMESTEP;
            player.x += player.velocity_x * FIXED_TIMESTEP;
        }else{
            player.x += 0;
            player.acceleration_x = 0;
            player.velocity_x = 0;
            level.worldToTileCoordinates(player.x, player.y, &gridX, &gridY);
            player.x -= ((player.x+(player.sprite->size/2))-(level.TILE_SIZE*(gridX+1))+.00000001);
            player.collidedBottom = true;
        }
    }
    
    else if(keys[SDL_SCANCODE_LEFT]){
        player.collidedLeft = false;
        player.collidedLeft = level.LeftCollision(&player);
        if (player.collidedLeft == false) {
            player.acceleration_x = -1.2;
            player.velocity_x = player.lerp(player.velocity_x, 0.0f, FIXED_TIMESTEP * player.friction_x);
            player.velocity_x += player.acceleration_x * FIXED_TIMESTEP;
            player.x += player.velocity_x * FIXED_TIMESTEP;
        }else{
            player.x += 0;
            player.acceleration_x = 0;
            player.velocity_x = 0;
            level.worldToTileCoordinates(player.x, player.y, &gridX, &gridY);
            player.x += ((level.TILE_SIZE*(gridX))-(player.x-(player.sprite->size/2))+.00000001);
            player.collidedBottom = true;
        }
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 640, 360);
    
    SDL_Event event;
    bool done = false;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    program->setModelMatrix(modelMatrix);
    program->setProjectionMatrix(projectionMatrix);
    program->setViewMatrix(viewMatrix);
    projectionMatrix.setOrthoProjection(-5.55, 5.55, -3.0f, 3.0f, -1.0f, 1.0f);
    
    Setup(*program);
    
    mapTexture = LoadTexture("mapTexture.png");
    SheetSprite mySprite(program, mapTexture, 30, 30, 19, .3);
    player.sprite = &mySprite;
    
    while (!done) {
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }else if(event.type == SDL_KEYDOWN) {
            }
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP ) {
            fixedElapsed -= FIXED_TIMESTEP;
        }
        //Background color
        glClearColor(0.53f, 0.808f, 0.98f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        UpdateGameLevel(*program);
        RenderGameLevel(*program);
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}



