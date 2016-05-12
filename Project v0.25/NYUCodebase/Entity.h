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
#include "SDL_mixer.h"
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
    void renderGraphics(Matrix &modelMatrix,int gridX, int gridY, int index);
    
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
    Entity(float x, float y, UnitType unitType, Fraction fraction, SheetSprite &sprite);
    
    void Render(Matrix &modelMatrix);
    bool checkOccupation(vector<Entity> &units);
    float distance(Entity *other);
    void rechargeMovement();
    void attack(Entity *other);
    void playAttackMusic(UnitType unitType);
    
    float x;
    float y;
    int index;
    SheetSprite *sprite;
    bool canAttack;
    
    float baseHealth=10;
    float baseDamage=5;
    float baseMovement=0;
    float Experience;
    Fraction fraction;
    UnitKind unitKind;
    UnitType unitType;
    
    Mix_Chunk *bullet=Mix_LoadWAV("bullet.wav");
    Mix_Chunk *explosion=Mix_LoadWAV("explosion.wav");
    Mix_Chunk *boom=Mix_LoadWAV("boom.wav");
    
    Mix_Chunk *attackArray[1][2]={
        bullet, boom
    };
    Mix_Chunk *deathArray[1][1]={
        explosion
    };
    
};

