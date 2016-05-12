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
#include <vector>
using namespace std;
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

enum Fraction {None, Red, Blue, Green};
enum UnitKind {NotUnit, BUILDING, UNIT};
enum UnitType {NotType, Inf,ATInf,APC,LTank,MTank,HTank,Selection,Highlight};

class Entity {
public:
    Entity();
    Entity(float x, float y, UnitType unitType, Fraction fraction, SheetSprite &sprite): x(x), y(y), fraction(fraction),sprite(&sprite), unitType(unitType) {
        if(unitType==Inf){index=0+sprite.SPRITE_COUNT_X*(fraction-1);}
        else if(unitType==ATInf){index=1+sprite.SPRITE_COUNT_X*(fraction-1);}
        else if(unitType==APC){index=2+sprite.SPRITE_COUNT_X*(fraction-1);}
        else if(unitType==LTank){index=4+sprite.SPRITE_COUNT_X*(fraction-1);}
        else if(unitType==MTank){index=5+sprite.SPRITE_COUNT_X*(fraction-1);}
        else if(unitType==HTank){index=6+sprite.SPRITE_COUNT_X*(fraction-1);}
        else if(unitType==Selection){index=15;}
    };
    
    void Render(Matrix &modelMatrix);
    bool checkOccupation(vector<Entity> &units);
    float movementDistance(Entity *other);
    //void rechargeMovements(vector<Entity> &units, Fraction player);
    
    float x;
    float y;
    int index;
    SheetSprite *sprite;
    
    float baseHealth=10;
    float baseDamage=10;
    float baseMovement=3;
    float Experience;
    Fraction fraction;
    UnitKind unitKind;
    UnitType unitType;
};