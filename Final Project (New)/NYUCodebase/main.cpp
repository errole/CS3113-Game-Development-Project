#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "Matrix.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "SheetSprite.h"
#include "Level.h"
#include <vector>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// Fixed Timestep
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

// Global Variables
SDL_Window* displayWindow;

// Matrices
Matrix modelMatrix;
Matrix projectionMatrix;
Matrix viewMatrix;

// Sheets
ShaderProgram* program;
GLuint spriteSheet;

// Level
Level demo;

// Keyboard input
const Uint8 *keys = SDL_GetKeyboardState(NULL);

void setUp() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    program->setModelMatrix(modelMatrix);
    program->setProjectionMatrix(projectionMatrix);
    program->setViewMatrix(viewMatrix);
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    glUseProgram(program->programID);
}

void drawLevel(Level demo) {
    program->setModelMatrix(modelMatrix);
    //modelMatrix.identity();
    for (int y=0; y < 25; y++) {
        for (int x=0; x < 23; x++) {
            if(demo.levelData[y][x] != 0) {
                float u = (float)(((int)demo.levelData[y][x]) % 30) / (float) 30;
                float v = (float)(((int)demo.levelData[y][x]) / 30) / (float) 30;
                float spriteWidth = 1.0f/(float)30;
                float spriteHeight = 1.0f/(float)30;
                demo.vertexData.insert(demo.vertexData.end(), {
                    0.15f * x, -0.15f * y,
                    0.15f * x, (-0.15f * y)-0.15f,
                    (0.15f * x)+0.15f, (-0.15f * y)-0.15f,
                    0.15f * x, -0.15f * y,
                    (0.15f * x)+0.15f, (-0.15f * y)-0.15f,
                    (0.15f * x)+0.15f, -0.15f * y
                });
                demo.texCoordData.insert(demo.texCoordData.end(), {
                    u, v,
                    u, v+(spriteHeight),
                    u+spriteWidth, v+(spriteHeight),
                    u, v,
                    u+spriteWidth, v+(spriteHeight),
                    u+spriteWidth, v
                });
            }
        }
    }
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, demo.vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, demo.texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, spriteSheet);
    glDrawArrays(GL_TRIANGLES, 0, demo.vertexData.size() / 2);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SDL_FreeSurface(surface);
    return textureID;
}

void update(Level demo) {
    drawLevel(demo);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Platformer Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
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
    
    float counterx = 0;
    float countery = 0;
    setUp();
    
    demo.buildLevel();

    spriteSheet = LoadTexture("sheet.png");

    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }

        glClearColor(0.4f, 0.2f, 0.4f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if (keys[SDL_SCANCODE_RIGHT]) {
            modelMatrix.Translate(counterx-=0.0005, 0, 0);
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            modelMatrix.Translate(-(counterx-=0.0005), 0, 0);
        }
        if (keys[SDL_SCANCODE_UP]) {
            modelMatrix.Translate(0,countery-=0.0005, 0);
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            modelMatrix.Translate(0, -(countery-=0.0005), 0);
        }
        
        
        update(demo);

        SDL_GL_SwapWindow(displayWindow);
    }
    SDL_Quit();
    return 0;
}