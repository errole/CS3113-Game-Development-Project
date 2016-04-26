//
//  Map.cpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/29/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Map.h"

bool Map::readHeader(std::ifstream &stream) {
    string line;
    mapWidth = -1;
    mapHeight = -1;
    
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "width") {
            mapWidth = atoi(value.c_str());
        } else if(key == "height"){
            mapHeight = atoi(value.c_str());
        }
    }
    if(mapWidth == -1 || mapHeight == -1) {
        return false;
    } else { // allocate our map data
        levelData = new unsigned char*[mapHeight];
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned char[mapWidth];
        }
        return true;
    }
}

bool Map::readLayerData(std::ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "data") {
            for(int y=0; y < mapHeight; y++) {
                getline(stream, line);
                istringstream lineStream(line);
                string tile;
                for(int x=0; x < mapWidth; x++) {
                    getline(lineStream, tile, ',');
                    //Data from file is limited to an index number of 255 (unsigned char)
                    unsigned char val =  (unsigned char)atoi(tile.c_str());
                    if(val > 0) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        levelData[y][x] = val-1;
                        //cout << " " << (int)val<<" ";
                    } else {
                        levelData[y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}

bool Map::readEntityData(std::ifstream &stream, Entity &player) {
    string line;
    string type;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "type") {
            type = value;
        } else if(key == "location") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            float placeX = atoi(xPosition.c_str())/16*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())/16*-TILE_SIZE;
            placeEntity(type, placeX, placeY, player);
        }
    }
    return true;
}

void Map::placeEntity(string type,float placeX,float &placeY, Entity &player){
    if (type == "ENTITY_PLAYER") {
        player.x = placeX/TILE_SIZE/2;
        player.y = -placeY/TILE_SIZE/2;
    }
}

void Map::renderLevel(ShaderProgram *program, GLuint mapTexture, Matrix &modelMatrix){
    program->setModelMatrix(modelMatrix);
    modelMatrix.identity();
    modelMatrix.Translate(-1, 5.0, 0);
    
    vertexData.clear();
    texCoordData.clear();
    
    for(int y=0; y < mapHeight; y++) {
        for(int x=0; x < mapWidth; x++) {
            if(levelData[y][x]!=0){

            float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
            float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
            
            float spriteWidth= 1.0f/(float)SPRITE_COUNT_X;
            float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
            //cout << " "<<u<<" "<<v<<"/n";
            vertexData.insert(vertexData.end(), {
                TILE_SIZE * x, -TILE_SIZE * y,
                TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
                (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                TILE_SIZE * x, -TILE_SIZE * y,
                (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
            });
                
            texCoordData.insert(texCoordData.end(), {
                u, v,
                u, v+(spriteHeight),
                u+spriteWidth, v+(spriteHeight),
                u, v,
                u+spriteWidth, v+(spriteHeight),
                u+spriteWidth, v
            });
                
            }
        }
    }
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glDrawArrays(GL_TRIANGLES, 0, vertexData.size()/2);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Map::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}
void Map::tileToWorldCoordinates(int gridX, int gridY, float *worldX, float *worldY) {
    *worldX = (float)(gridX * TILE_SIZE);
    *worldY = (float)(-gridY * TILE_SIZE);
}

bool Map::bottomCollision(Entity *player){
    int gridX;
    int gridY;
    worldToTileCoordinates(player->x, (player->y-(player->sprite->size/2)), &gridX, &gridY);
    for(int i=0;i<solids.size();i++){
        if((int)(levelData[gridY][gridX])==solids[i]){
            return true;
        }
    }
    return false;
}
bool Map::RightCollision(Entity *player){
    int gridX;
    int gridY;
    worldToTileCoordinates(player->x+(player->sprite->size/2), player->y, &gridX, &gridY);
    for(int i=0;i<solids.size();i++){
        if((int)(levelData[gridY][gridX])==solids[i]){
            return true;
        }
    }
    return false;
}
bool Map::LeftCollision(Entity *player){
    int gridX;
    int gridY;
    worldToTileCoordinates(player->x-(player->sprite->size/2), player->y, &gridX, &gridY);
    for(int i=0;i<solids.size();i++){
        if((int)(levelData[gridY][gridX])==solids[i]){
            return true;
        }
    }
    return false;
}

