
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <stdlib.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <map>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Utilities.h"
#include "Map.h"
#include "Mesh.hpp"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// 60 FPS (1.0f/60.0f)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
float ticks;
//Timer
float lastFrameTicks = 0.0f;
float lastElapsed = 0.0;
float elapsed = 0.0;
//Global Setup Parameters
SDL_Window* displayWindow;
SDL_Event event;
unsigned char** levelData;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
ShaderProgram* program;
bool done = false;
//
GLuint mapTexture;
GLuint unitTexture;
GLuint fire1Texture;
GLuint fire2Texture;
GLuint fontSheet;
GLuint tankSkin;
SheetSprite *mapSprite;
SheetSprite *fire1Sprite;
SheetSprite *fire2Sprite;
SheetSprite *unitSprite;
string levelFileOne = "gamemap1.txt";
string levelFileTwo = "gamemap2.txt";
string levelFileThree = "gamemap3.txt";
Mesh tank;
//Game States
enum GameState {STATE_MAIN_MENU, STATE_GAME_LEVEL_1, STATE_GAME_LEVEL_2, STATE_GAME_LEVEL_3, STATE_OVER};
int state = STATE_MAIN_MENU;
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
Entity *unitSelected;
Map level;
vector<Entity> allUnits;
string message="";
int redPlayerUnits = 0;
int bluePlayerUnits = 0;
int death = 0;
int playerTurn = 1;

void Setup (ShaderProgram &program, string file) {
    //Load Map File
    ifstream infile(file);
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
    }
}
void loadUnits(){
    allUnits.clear();
    //Add Units
    Entity unit(0, 20, Inf, Red, *unitSprite);
    Entity unit1(0, 21, APC, Red, *unitSprite);
    Entity unit2(0, 22, ATInf, Red, *unitSprite);
    Entity unit3(0, 23, LTank, Red, *unitSprite);
    Entity unit4(20, 1, Inf, Blue, *unitSprite);
    Entity unit5(20, 2, APC, Blue, *unitSprite);
    Entity unit6(20, 3, ATInf, Blue, *unitSprite);
    Entity unit7(20, 4, LTank, Blue, *unitSprite);
    
    allUnits.push_back(unit);
    allUnits.push_back(unit1);
    allUnits.push_back(unit2);
    allUnits.push_back(unit3);
    allUnits.push_back(unit4);
    allUnits.push_back(unit5);
    allUnits.push_back(unit6);
    allUnits.push_back(unit7);
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
    program.setViewMatrix(viewMatrix);

}
void UpdateGameLevel(ShaderProgram &program) {
    float screenMovement = 0.1;
    float zoomRes = 0.01;
    
    while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE || keys[SDL_SCANCODE_ESCAPE]) {
        done = true;
    }else if (event.type == SDL_KEYDOWN) {
        //Adjust Selection Window
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
        //Allow Player to Select Own Units and then 2nd time is movement
        if (keys[SDL_SCANCODE_X]){
            if ( moveWindowOn == true){
                if (selectionWindow->checkOccupation(allUnits) && unitSelected->distance(selectionWindow) <= unitSelected->baseMovement && !level.mapCollision(selectionWindow)) {
                    unitSelected->baseMovement -= unitSelected->distance(selectionWindow);
                    unitSelected->x = selectionWindow->x;
                    unitSelected->y = selectionWindow->y;
                    moveWindowOn = false;
                }
            }else {
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
                            
                            allUnits[i].playAttackMusic(allUnits[i].unitType);
                            unitSelected->attack(&allUnits[i]);
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
    //Map Controls
    if (keys[SDL_SCANCODE_Q]) {
        state = STATE_GAME_LEVEL_1;
        Setup(program, levelFileOne);
    }
    if (keys[SDL_SCANCODE_E]) {
        state = STATE_GAME_LEVEL_2;
        Setup(program, levelFileTwo);
    }
    if (keys[SDL_SCANCODE_R]) {
        state = STATE_GAME_LEVEL_3;
        Setup(program, levelFileThree);
    }
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
    
    //Check for Dead Units
    for(int i = 0;i<allUnits.size(); i++) {
        if(allUnits[i].baseHealth <= 0) {
                allUnits.erase(allUnits.begin() + i);
        }
    }
    //Count Units Still Alive
    redPlayerUnits=0;
    bluePlayerUnits=0;
    for(int i=0;i<allUnits.size();i++){
        if (allUnits[i].fraction==Blue) {
            bluePlayerUnits+=1;
        }else if(allUnits[i].fraction==Red){
            redPlayerUnits+=1;
        }
    }

    //Check Win Condition
    if(bluePlayerUnits==0){
        message="Blue Player Wins";
        state=STATE_MAIN_MENU;
    }else if(redPlayerUnits==0){
        message="Red Player Wins";
        state=STATE_MAIN_MENU;
    }
}

void RenderMenu(){
    DrawText(program, fontSheet, "Primitve Wars", 0.5f, -0.25f, -1.5, -1.2);
    DrawText(program, fontSheet, message, 0.5f, -0.25f, -1.9, -1.9);
    modelMatrix.identity();
    modelMatrix.Translate(0, -1, 0);
    modelMatrix.Yaw(ticks/15);
    program->setModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D,tankSkin);
    viewMatrix.identity();
    viewMatrix.Scale(0.3, 0.3, 0);
    program->setViewMatrix(viewMatrix);
    tank.Render(program);
}
void UpdateMenu(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE || keys[SDL_SCANCODE_ESCAPE]) {
            done = true;
        }else{
            if (keys[SDL_SCANCODE_Q]) {
                state = STATE_GAME_LEVEL_1;
                Setup(*program, levelFileOne);
                loadUnits();
            }
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
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    program->setModelMatrix(modelMatrix);
    program->setProjectionMatrix(projectionMatrix);
    program->setViewMatrix(viewMatrix);
    projectionMatrix.setOrthoProjection(-5.55, 5.55, -3.0f, 3.0f, -1.0f, 1.0f);
    //MUSIC
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    Mix_Music *music;
    music = Mix_LoadMUS("ImperialMusic.mp3");
    Mix_PlayMusic(music, -1);
    //
    tank.loadOBJ("T-90.obj");
    mapTexture = LoadTexture("RPGpack_sheet.png");
    unitTexture = LoadTexture("Map_units.png");
    fire1Texture = LoadTexture("fire.png");
    fire2Texture = LoadTexture("fire2.png");
    fontSheet = LoadTexture("font1.png");
    tankSkin = LoadTexture("purple.png");
    mapSprite = new SheetSprite(program, mapTexture, 20, 13, .3);
    fire1Sprite = new SheetSprite(program, fire1Texture, 1, 1, .3);
    fire2Sprite = new SheetSprite(program, fire2Texture, 1, 1, .3);
    unitSprite = new SheetSprite(program, unitTexture, 26, 10, .3);
    selectionWindow=new Entity(0, 0, NotType, None, *mapSprite);
    selectionWindow->index = 15;
    moveWindow=new Entity(0, 0, NotType, None, *mapSprite);
    moveWindow->index = 17;
    warWindow=new Entity(0, 0, NotType, None, *mapSprite);
    warWindow->index = 16;
    unitSelected = new Entity(0, 0, NotType, None, *mapSprite);
    loadUnits();
    
    
    while (!done) {
        
        //Background color
        glClearColor(0.53f, 0.808f, 0.98f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        switch(state) {
            case STATE_OVER: {
                state = STATE_MAIN_MENU;
            }
            case STATE_MAIN_MENU: {
                RenderMenu();
                UpdateMenu();
                break;
            }
            case STATE_GAME_LEVEL_1:
                RenderGameLevel(*program, elapsed);
                UpdateGameLevel(*program);
                break;
            case STATE_GAME_LEVEL_2:
                RenderGameLevel(*program, elapsed);
                UpdateGameLevel(*program);
                break;
            case STATE_GAME_LEVEL_3:
                RenderGameLevel(*program, elapsed);
                UpdateGameLevel(*program);
                break;
        }
        
        ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP ) {
            fixedElapsed -= FIXED_TIMESTEP;
        }
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}



