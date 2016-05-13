//
//  Particle.cpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 5/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//
#include "Particle.hpp"


/*void ParticleEmitter::Update(float elapsed){
    for(int i=0; i<particles.size();i++){
        
    }
}*/

Particle::Particle(float x, float y, float size, GLuint& texture): texture(texture), size(size), x(x), y(y) {}

void Particle::Update(){}

void Particle::Render( ShaderProgram *program, Matrix &modelMatrix ){
    program->setModelMatrix(modelMatrix);
    modelMatrix.identity();
    
    float u = 1;
    float v = 1;
    float spriteWidth= 1;
    float spriteHeight = 1;
    float vertices[] ={
        TILE_SIZE * x, -TILE_SIZE * y,
        TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
        (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
        TILE_SIZE * x, -TILE_SIZE * y,
        (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
        (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
    };
    float texCoords[] = {
        u, v,
        u, v+(spriteHeight),
        u+spriteWidth, v+(spriteHeight),
        u, v,
        u+spriteWidth, v+(spriteHeight),
        u+spriteWidth, v
    };
    
    glUseProgram(program->programID);
    glBindTexture(GL_TEXTURE_2D, texture);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertices);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    //glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, colors);
    glEnableVertexAttribArray(program->positionAttribute);
    glEnableVertexAttribArray(program->texCoordAttribute);
    //glEnableVertexAttribArray(colorAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}