//
//  mesh.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 5/12/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//
#pragma once
#ifndef mesh_hpp
#include <vector>
#include "ShaderProgram.h"
#define mesh_hpp

#include <stdio.h>

#endif /* mesh_hpp */

class Mesh {
public:
    void Render(ShaderProgram *program);
    void loadOBJ(const char *fileName);
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<float> normals;
};