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
    SheetSprite();
    SheetSprite(ShaderProgram* program, unsigned int textureID, int SPRITE_COUNT_X, int SPRITE_COUNT_Y, float TILE_SIZE): program(program), textureID(textureID), SPRITE_COUNT_X(SPRITE_COUNT_X), SPRITE_COUNT_Y(SPRITE_COUNT_Y), TILE_SIZE(TILE_SIZE){};
    
    void renderGraphics(Matrix &modelMatrix, Entity &player);
    
    ShaderProgram* program;
    Matrix* modelMatrix;
    
    float TILE_SIZE=.3;
    unsigned int textureID;
    int SPRITE_COUNT_X = 26;
    int SPRITE_COUNT_Y = 10;
};

enum Fraction {PLAYER1, PLAYER2};
enum UnitKind {BUILDING, UNIT};
enum UnitType {Inf,ATInf,APC,LTank,MTank,HTank,Selection};

class Entity {
public:
    Entity();
    Entity(float x, float y, UnitType unitType, SheetSprite &sprite): x(x), y(y), sprite(&sprite), unitType(unitType) {
        if(unitType==Inf){index=0;}
        else if(unitType==ATInf){index=1;}
        else if(unitType==APC){index=2;}
        else if(unitType==LTank){index=4;}
        else if(unitType==MTank){index=5;}
        else if(unitType==HTank){index=6;}
        else if(unitType==Selection){index=15;}
    };
    
    void Render(Matrix &modelMatrix);
    
    float x;
    float y;
    int index = 0;
    SheetSprite *sprite;
    
    float baseHealth=10;
    float baseDamage=10;
    float baseMovement=3;
    float Experience;
    Fraction entityType;
    UnitKind unitKind;
    UnitType unitType;
};