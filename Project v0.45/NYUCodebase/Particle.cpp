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

Particle::Particle(float x, float y, float size, GLuint& texture): texture(texture), size(size), x(x), y(y) {
    vertices.insert(vertices.end(), {
        TILE_SIZE * x, -TILE_SIZE * y,
        TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
        (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
        TILE_SIZE * x, -TILE_SIZE * y,
        (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
        (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
    });
}

void Particle::Update(){
    vertices.clear();
    float cosTheta = cosf(xRotation);
    float sinTheta = sinf(xRotation);
    float TL_x = cosTheta * -size - sinTheta * size;
    float TL_y = sinTheta * -size + cosTheta * size;
    float BL_x = cosTheta * -size - sinTheta * -size;
    float BL_y = sinTheta * -size + cosTheta * -size;
    float BR_x = cosTheta * size - sinTheta * -size;
    float BR_y = sinTheta * size + cosTheta * -size;
    float TR_x = cosTheta * size - sinTheta * size;
    float TR_y = sinTheta * size + cosTheta * size;
    vertices.insert(vertices.end(), {
        x + TL_x, y + TL_y,
        x + BL_x, y + BL_y,
        x + TR_x, y + TR_y,
        x + TR_x, y + TR_y,
        x + BL_x, y + BL_y,
        x + BR_x, y + BR_y
    });
}

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