//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//

#include "Entity.h"

float damMod[6][6]={
    1,   .8,    1.5,   1,  1,   1,
    1.5,  1,    1.5,   1,  1,   1,
    .8,   1.5,    1,   1,  1,   1,
    .5,   1.2,   .5,   1,  1,   1,
    .3,   1,     .3,   1,  1,   1,
    .2,   .8,    .2,   1,  1,   1,
};

void SheetSprite::renderGraphics(Matrix &modelMatrix, Entity &player) {
    modelMatrix.identity();

                
    float u = (float)(((int)player.index) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
    float v = (float)(((int)player.index) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
    float spriteWidth= 1.0f/(float)SPRITE_COUNT_X;
    float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
    //cout << " "<<u<<" "<<v<<"/n";
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

void Entity::Render(Matrix &modelMatrix){
    sprite->renderGraphics(modelMatrix,*this);
}