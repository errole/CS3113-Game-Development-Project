//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//
#pragma once

#ifndef Entity_h
#define Entity_h

#include <stdio.h>
#include "ShaderProgram.h"
#include <math.h>

#endif /* Entity_h*/

class Entity;

class SheetSprite {
public:
    SheetSprite(ShaderProgram* program, unsigned int textureID, int spriteCountX, int spriteCountY, int index, float size): program(program), textureID(textureID), spriteCountX(spriteCountX), spriteCountY(spriteCountY), index(index), size(size){};
    void DrawPlayer(Matrix &modelMatrix, Entity &player);
    float size;
    unsigned int textureID;
    float aspect;
    int index = 20;
    int spriteCountX = 30;
    int spriteCountY = 30;
    float u;
    float v;
    float spriteWidth;
    float spriteHeight;
    ShaderProgram* program;
    Matrix modelMatrix;
};

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_COIN};

class Entity {
public:
    Entity();
    Entity(float x, float y, float width, float height): x(x), y(y), width(width), height(height) {};
    
    void UpdateX(float FIXED_TIMESTEP);
    void UpdateY(float FIXED_TIMESTEP);
    void Render();
    bool collidesWith(Entity *entity);
    float lerp(float v0, float v1, float t);
    
    float x;
    float y;
    
    float width;
    float height;
    
    float velocity_x = 0;
    float velocity_y = 0;
    
    float acceleration_x = 0;
    float acceleration_y = 0;
    
    float friction_x=2;
    float friction_y=2;
    
    float gravity_x=0;
    float gravity_y=0;
    
    bool isStatic;
    EntityType entityType;
    SheetSprite *sprite;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
};