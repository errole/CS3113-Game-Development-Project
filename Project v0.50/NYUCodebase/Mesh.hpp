//
//  Mesh.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 5/12/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//
#pragma once

#include <vector>
#include "ShaderProgram.h"
#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>

#endif /* Mesh_hpp */

class Mesh {
public:
    void Render(ShaderProgram *program);
    void loadOBJ(const char *fileName);
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<float> normals;
};