
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
#include <map>

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
GLuint mapTexture = 0;
GLuint unitTexture = 0;

//Game States
enum GameState {
    STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_WIN, STATE_LOSE
};
int state = STATE_MAIN_MENU;

//Timer
float lastFrameTicks = 0.0f;
float elapsed = 0.0;

//Matrices
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;

//Camera Position/Variables
float posX=0;
float posY=0;
float zoom=.1;

//Entity & Map Data
bool moveWindowOn = false;
bool warWindowOn = false;
Entity *selectionWindow;
Entity *moveWindow;
Entity *warWindow;
Map level;
vector<Entity> allUnits;

void Setup (ShaderProgram &program) {
    //Load Map File
    string levelFileOne = "/Users/Kevin/Desktop/Stuff/NYU-Poly/CS3113/Final/CS3113-Game-Development-Project/Project v0.25/resources/gamemap1.txt";
    ifstream infile(levelFileOne);
    string line;
    while (getline(infile, line)) {
        if (line == "[header]") {
            level.readHeader(infile);
        }
        else if (line == "[layer]") {
            level.readLayerData(infile);
        }
        else if (line == "[layer2]") {
            level.readLayerData2(infile);
        }
        else if (line == "[ObjectsLayer]") {
        }
    }
}

void RenderGameLevel(ShaderProgram &program, float elapsed) {
    //Rendering
    level.renderLevel(&program, mapTexture, modelMatrix);
    for (int i = 0; i<allUnits.size(); i++) {
        allUnits[i].Render(modelMatrix);
    }
    selectionWindow->Render(modelMatrix);
    if (moveWindowOn) {
        moveWindow->Render(modelMatrix);
    }
    if (warWindowOn) {
        warWindow->Render(modelMatrix);
    }
    
    //Scrolling
    viewMatrix.identity();
    viewMatrix.Scale(zoom, zoom, 0);
    viewMatrix.Translate(posX,posY,0);
    //viewMatrix.Rotate(.3);
    viewMatrix.Pitch(elapsed/2);
    viewMatrix.Yaw(elapsed);
    //cout << elapsed << endl;
    program.setViewMatrix(viewMatrix);

}

void UpdateGameLevel(ShaderProgram &program) {
    float screenMovement = 0.1;
    float zoomRes = 0.01;
    //Zoom and Scroll Controls
    if (keys[SDL_SCANCODE_UP]) {
        posY -= screenMovement;
    }
    
    if (keys[SDL_SCANCODE_DOWN]) {
        posY += screenMovement;
    }
    
    if (keys[SDL_SCANCODE_RIGHT]) {
        posX -= screenMovement;
    }
    
    if (keys[SDL_SCANCODE_LEFT]) {
        posX += screenMovement;
    }
    
    if (keys[SDL_SCANCODE_O]) {
        zoom += zoomRes;
    }
    
    if (keys[SDL_SCANCODE_L]) {
        zoom -= zoomRes;
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
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    Mix_Music *music;
    music = Mix_LoadMUS("War Theme  America the Beautiful.mp3");
    Mix_PlayMusic(music, -1);
    mapTexture = LoadTexture("RPGpack_sheet.png");
    SheetSprite mapSprite(program, mapTexture, 20, 13, .3);
    unitTexture = LoadTexture("Map_units.png");
    SheetSprite unitSprite(program, unitTexture, 26, 10, .3);
    selectionWindow=new Entity(0, 0, NotType, None, mapSprite);
    selectionWindow->index = 15;
    moveWindow=new Entity(0, 0, NotType, None, mapSprite);
    moveWindow->index = 17;
    warWindow=new Entity(0, 0, NotType, None, mapSprite);
    warWindow->index = 16;
    Entity *unitSelected = new Entity(0, 0, NotType, None, mapSprite);
    
    //Add Units
    Entity unit(0, 0, Inf, Red, unitSprite);
    Entity unit2(1, 1, APC, Blue, unitSprite);
    allUnits.push_back(unit);
    allUnits.push_back(unit2);
    int playerTurn = 1;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE || keys[SDL_SCANCODE_ESCAPE]) {
                done = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                //Allow Player to Select Own Units and then 2nd time is movement
                if (keys[SDL_SCANCODE_X]){
                    if ( moveWindowOn == true){
                        if (selectionWindow->checkOccupation(allUnits) && unitSelected->distance(selectionWindow) <= unitSelected->baseMovement) {
                            unitSelected->baseMovement -= unitSelected->distance(selectionWindow);
                            unitSelected->x = selectionWindow->x;
                            unitSelected->y = selectionWindow->y;
                            moveWindowOn = false;
                        }
                    }
                    else {
                        for(int i = 0; i < allUnits.size(); i++) {
                            if (allUnits[i].x == selectionWindow->x && allUnits[i].y == selectionWindow->y && allUnits[i].fraction == playerTurn && allUnits[i].baseMovement > 0) {
                                moveWindowOn = true;
                                unitSelected = &allUnits[i];
                                cout << unitSelected->baseMovement;
                                moveWindow->x = unitSelected->x;
                                moveWindow->y = unitSelected->y;
                            }
                        }
                    }
                }
                //Shadow Box Movement Controls
                if (keys[SDL_SCANCODE_W] && selectionWindow->y > 0) {
                    selectionWindow->y -= 1;
                }
                if (keys[SDL_SCANCODE_S] && selectionWindow->y < level.mapHeight - 1) {
                    selectionWindow->y += 1;
                }
                if (keys[SDL_SCANCODE_D] && selectionWindow->x < level.mapWidth - 1) {
                    selectionWindow->x += 1;
                }
                if (keys[SDL_SCANCODE_A] && selectionWindow->x > 0) {
                    selectionWindow->x -= 1;
                }
                if (keys[SDL_SCANCODE_RETURN]) {
                    cout << "Enter";
                    //Alternate Players
                    playerTurn += 1;
                    if (playerTurn > 2) {
                        playerTurn = 1;
                    }
                    //Recharge all Movement
                    for(int i = 0; i < allUnits.size(); i++) {
                        if (playerTurn == allUnits[i].fraction) {
                            allUnits[i].rechargeMovement();
                        }
                    }
                }
                //Enter Attack Mode
                if(keys[SDL_SCANCODE_Z]) {
                    if(warWindowOn == true) {
                        if(unitSelected->distance(selectionWindow) == 1) {
                            for(int i = 0;i<allUnits.size(); i++) {
                                if(allUnits[i].x == selectionWindow->x && allUnits[i].y == selectionWindow->y && allUnits[i].fraction != playerTurn) {
                                    unitSelected->attack(&allUnits[i]);
                                    if(allUnits[i].baseHealth <= 0) {
                                        allUnits.erase(allUnits.begin() + i);
                                    }
                                }
                            }
                            warWindowOn = false;
                        }
                    }
                    else {
                        for(int i = 0; i < allUnits.size(); i++) {
                            if(allUnits[i].x == selectionWindow->x && allUnits[i].y == selectionWindow->y && allUnits[i].fraction == playerTurn) {
                                warWindowOn = true;
                                unitSelected = &allUnits[i];
                                warWindow->x = unitSelected->x;
                                warWindow->y = unitSelected->y;
                            }
                        }
                    }
                }
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
        RenderGameLevel(*program, ticks);
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}



