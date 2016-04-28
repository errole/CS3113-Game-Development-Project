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
int gridX;
int gridY;
float posX=0;
float posY=0;
float zoom=.1;

void Setup(ShaderProgram &program){
    //Load Map File
    string levelFileOne = "/Users/errolelbasan/Documents/Codes/Game-Development-Project/Project v0.1/resources/gamemap1.txt";
    ifstream infile(levelFileOne);
    string line;
    while (getline(infile, line)) {
        if(line == "[header]") {
            level.readHeader(infile);
        }
        else if(line == "[layer]") {
            level.readLayerData(infile);
        }
        else if(line == "[layer2]") {
            level.readLayerData2(infile);
        }
        else if(line == "[ObjectsLayer]") {
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
    //Scrolling
    viewMatrix.identity();
    viewMatrix.Scale(zoom, zoom, 0);
    viewMatrix.Translate(posX,posY,0);
    //viewMatrix.Rotate(.3);
    program.setViewMatrix(viewMatrix);

}

void UpdateGameLevel(ShaderProgram &program){
    float screenMovement=0.1;
    float zoomRes=.01;
    
    if(keys[SDL_SCANCODE_UP]){
        posY-=screenMovement;
    }
    
    if(keys[SDL_SCANCODE_DOWN]){
        posY+=screenMovement;
    }
    
    if(keys[SDL_SCANCODE_RIGHT]) {
        posX-=screenMovement;
    }
    
    if(keys[SDL_SCANCODE_LEFT]){
        posX+=screenMovement;
    }
    
    if(keys[SDL_SCANCODE_O]){
        zoom+=zoomRes;
    }
    if(keys[SDL_SCANCODE_L]){
        zoom-=zoomRes;
    }
    //cout << zoom;
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
    
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    Mix_Music *music;
    music = Mix_LoadMUS( "War Theme  America the Beautiful.mp3");
    Mix_PlayMusic(music, -1);
    
    mapTexture = LoadTexture("RPGpack_sheet.png");
    SheetSprite mySprite(program, mapTexture, 30, 30, 19, .3);
    
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



