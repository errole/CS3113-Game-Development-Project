//
//  Particle.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 5/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//
#pragma once
#ifndef Particle_hpp
#define Particle_hpp
#include <vector>
#include <stdio.h>
#include "ShaderProgram.h"

#endif /* Particle_hpp */
using namespace std;

class Particle {
public:
    Particle(float x, float y, float size, GLuint& texture);
    void Render(ShaderProgram *program, Matrix &modelMatrix);
    
    float x;
    float y;
    GLuint& texture;
    float lifetime=0;
    float TILE_SIZE=.3;
    float size=1;
    float aspect=1.0;
    
};

/*class ParticleEmitter {
public:
    ParticleEmitter(unsigned int particleCount);
    ParticleEmitter();
    ~ParticleEmitter();
    
    void Update(float elapsed);
    
    vector<float> gravity;
    vector<Particle> particles;
    float startSize;
    float endSize;
    float maxLifetime = 1;
};
*/