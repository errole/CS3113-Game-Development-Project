//
//  SheetSprite.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/29/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//
#pragma once

#include "Entity.h"
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class SheetSprite {
public:
    SheetSprite();
    SheetSprite(ShaderProgram* program, unsigned int textureID, int spriteCountX, int spriteCountY, int index, float size): program(program), textureID(textureID), spriteCountX(spriteCountX), spriteCountY(spriteCountY), index(index), size(size){
        u = (float)(((int)index) % spriteCountX) / (float) spriteCountX;
        v = (float)(((int)index) / spriteCountX) / (float) spriteCountY;
        spriteWidth = 1.0/(float)spriteCountX;
        spriteHeight = 1.0/(float)spriteCountY;
    };
    void Draw(float vertices[]);
    float size;
    unsigned int textureID;
    float aspect;
    int index;
    int spriteCountX;
    int spriteCountY;
    float u;
    float v;
    float spriteWidth;
    float spriteHeight;
    ShaderProgram* program;
    Matrix modelMatrix;
};
