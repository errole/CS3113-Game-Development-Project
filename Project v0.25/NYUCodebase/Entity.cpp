//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//

#include "Entity.h"
#include <vector>
#include <math.h>
#include <SDL_mixer.h>
using namespace std;

const int width = 7;
const int height = 7;
float damageModifier[height][width]={
    0,      Inf,    ATInf,  APC,    LTank,  MTank,  HTank, // <- ATTACKING UNIT
    Inf,    1,      .8,     1.5,    1,      1,      1,
    ATInf,  1.5,    1,      1.5,    1,      1,      1,
    APC,    .8,     1.5,    1,      1,      1,      1,
    LTank,  .5,     1.2,    .5,     1,      1,      1,
    MTank,  .3,     1,      .3,     1,      1,      1,
    HTank,  .2,     .8,     .2,     1,      1,      1,
};
int movementModifier[2][6]={
    Inf,    ATInf,  APC,    LTank,  MTank,  HTank,
    2,      2,      4,      4,      3,      2,
};

void SheetSprite::renderGraphics(Matrix &modelMatrix, Entity &player) {
    modelMatrix.identity();

                
    float u = (float)(((int)player.index) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
    float v = (float)(((int)player.index) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
    float spriteWidth= 1.0f/(float)SPRITE_COUNT_X;
    float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
    float vertexData[] ={
        TILE_SIZE * player.x, -TILE_SIZE * player.y,
        TILE_SIZE * player.x, (-TILE_SIZE * player.y)-TILE_SIZE,
        (TILE_SIZE * player.x)+TILE_SIZE, (-TILE_SIZE * player.y)-TILE_SIZE,
        TILE_SIZE * player.x, -TILE_SIZE * player.y,
        (TILE_SIZE * player.x)+TILE_SIZE, (-TILE_SIZE * player.y)-TILE_SIZE,
        (TILE_SIZE * player.x)+TILE_SIZE, -TILE_SIZE * player.y
    };
    float texCoordData[] = {
        u, v,
        u, v+(spriteHeight),
        u+spriteWidth, v+(spriteHeight),
        u, v,
        u+spriteWidth, v+(spriteHeight),
        u+spriteWidth, v
    };
    
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData);
    glEnableVertexAttribArray(program->texCoordAttribute);
    program->setModelMatrix(modelMatrix);
    modelMatrix.identity();
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

}
void SheetSprite::renderGraphics(Matrix &modelMatrix, int gridX, int gridY, int index) {
    modelMatrix.identity();
    
    
    float u = (float)(((int)index) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
    float v = (float)(((int)index) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
    float spriteWidth= 1.0f/(float)SPRITE_COUNT_X;
    float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
    float vertexData[] ={
        TILE_SIZE * gridX, -TILE_SIZE * gridY,
        TILE_SIZE * gridX, (-TILE_SIZE * gridY)-TILE_SIZE,
        (TILE_SIZE * gridX)+TILE_SIZE, (-TILE_SIZE * gridY)-TILE_SIZE,
        TILE_SIZE * gridX, -TILE_SIZE * gridY,
        (TILE_SIZE * gridX)+TILE_SIZE, (-TILE_SIZE * gridY)-TILE_SIZE,
        (TILE_SIZE * gridX)+TILE_SIZE, -TILE_SIZE * gridY
    };
    float texCoordData[] = {
        u, v,
        u, v+(spriteHeight),
        u+spriteWidth, v+(spriteHeight),
        u, v,
        u+spriteWidth, v+(spriteHeight),
        u+spriteWidth, v
    };
    
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData);
    glEnableVertexAttribArray(program->texCoordAttribute);
    program->setModelMatrix(modelMatrix);
    modelMatrix.identity();
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}

void Entity::Render(Matrix &modelMatrix){
    sprite->renderGraphics(modelMatrix,*this);
}

Entity::Entity(){};

Entity::Entity(float x, float y, UnitType unitType, Fraction fraction, SheetSprite &sprite): x(x), y(y), fraction(fraction),sprite(&sprite), unitType(unitType) {
    //Modify all attributes of the class depending on the unit types
    for(int x=0; x < width-1; x++){
        if(unitType==movementModifier[0][x]){
            baseMovement= movementModifier[1][x];
        }
    }
    if(unitType==Inf){index=0+sprite.SPRITE_COUNT_X*(fraction-1);}
    else if(unitType==ATInf){index=1+sprite.SPRITE_COUNT_X*(fraction-1);}
    else if(unitType==APC){index=2+sprite.SPRITE_COUNT_X*(fraction-1);}
    else if(unitType==LTank){index=4+sprite.SPRITE_COUNT_X*(fraction-1);}
    else if(unitType==MTank){index=5+sprite.SPRITE_COUNT_X*(fraction-1);}
    else if(unitType==HTank){index=6+sprite.SPRITE_COUNT_X*(fraction-1);}
    else if(unitType==Selection){index=15;}
}

bool Entity::checkOccupation(vector<Entity> &units){
    for(int i=0;i<units.size();i++){
        if(units[i].x == this->x && units[i].y==this->y){
            return false;
        }
    }
    return true;
}

float Entity::distance(Entity *other){
    int num = fabs(this->x-other->x)+fabs(this->y-other->y);
    return num;
}

void Entity::attack(Entity *other){
    int i= 0;
    int j= 0;
    for(i = 0; i<width;i++){
        if(this->unitType==damageModifier[0][i]){
            break;
        }
    }
    for(j =0; j<height;j++){
        if(this->unitType==damageModifier[j][0]){
            break;
        }
    }
    other->baseHealth= other->baseHealth-(this->baseDamage*damageModifier[j][i]);
    this->baseMovement=0;
}

void Entity::rechargeMovement(){
    for(int x=0; x < width-1; x++){
        if(unitType==movementModifier[0][x]){
            baseMovement= movementModifier[1][x];
        }
    }
}