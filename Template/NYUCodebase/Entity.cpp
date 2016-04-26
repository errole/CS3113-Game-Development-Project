//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//

#include "Entity.h"

void SheetSprite::DrawPlayer(Matrix &modelMatrix, Entity &player) {
    program->setModelMatrix(modelMatrix);
    modelMatrix.identity();
    modelMatrix.Translate(player.x, player.y, 0);
    u = (float)(((int)index) % spriteCountX) / (float) spriteCountX;
    v = (float)(((int)index) / spriteCountX) / (float) spriteCountY;

    spriteWidth = 1.0/(float)spriteCountX;
    spriteHeight = 1.0/(float)spriteCountY;
    GLfloat texCoords[] = {
        u, v+spriteHeight,
        u+spriteWidth, v,
        u, v,
        u+spriteWidth, v,
        u, v+spriteHeight,
        u+spriteWidth, v+spriteHeight
    };
    float aspect = player.width / player.height;
    float vertices[] = {
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, 0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, -0.5f * size};
    /*
    float vertices1[] = {
        -0.4f * 0.3f, -0.4f * 0.3f,
        0.4f * 0.3f, 0.4f * 0.3f,
        -0.4f * 0.3f, 0.4f * 0.3f,
        0.4f * 0.3f, 0.4f * 0.3f,
        -0.4f * 0.3f, -0.4f * 0.3f ,
        0.4f * 0.3f, -0.4f * 0.3f};
    */
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    program->setModelMatrix(modelMatrix);
    modelMatrix.identity();
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

float Entity::lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

/*
void Entity::Render() {
    float vertices[] = {
        x-width, y-height,
        x+width, y-height,
        x+width, y+height,
        x-width, y-height,
        x+width, y+height,
        x-width, y+height};
    sprite->Draw(vertices);
}
 */   
bool Entity::collidesWith(Entity *other){
    if(y-height > other->y+other->height || y+height < other->y-other->height ||
        x-width > other->x+other->width || x+width < other->x-other->width){
        return 0;
    }else{
        return 1;
    }
}

void Entity::UpdateX(float FIXED_TIMESTEP){
    velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction_x);
    velocity_x += acceleration_x * FIXED_TIMESTEP;
    x += velocity_x * FIXED_TIMESTEP;
}

void Entity::UpdateY(float FIXED_TIMESTEP){
    velocity_y = gravity_y * FIXED_TIMESTEP;
    velocity_y += lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);
    velocity_y += acceleration_y * FIXED_TIMESTEP;
    y += velocity_y * FIXED_TIMESTEP;
}