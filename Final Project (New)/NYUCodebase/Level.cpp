#include "Level.h"

void Level::buildLevel() {
    ifstream infile("untitled.txt");
    string line;
    while (getline(infile, line)) {
        
        if (line == "[header]") {
            if (!readHeader(infile)) {
                return;
            }
        }
        else if (line == "[layer]") {
            //cout << "calling read layer" << endl;
            readLayerData(infile);
        }
        else if (line == "[Object Layer 1]") {
            readEntityData(infile);
        }
    }
}

bool Level::readHeader(ifstream& infile) {
    string line;
    mapWidth = -1;
    mapHeight = -1;
    while (getline(infile, line)) {
        if (line == "") {
            break;
        }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "width") {
            mapWidth = atoi(value.c_str());
        }
        else if (key == "height") {
            mapHeight = atoi(value.c_str());
        }
    }
    if (mapWidth == -1 || mapHeight == -1) {
        return false;
    }
    else { // allocate our map data
        levelData = new unsigned char*[mapHeight];
        for (int i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned char[mapWidth];
        }
        return true;
    }
}

bool Level::readLayerData(ifstream& infile) {
    string line;
    while (getline(infile, line)) {
        if(line == "") {
            break;
        }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "data") {
            for(int y=0; y < mapHeight; y++) {
                getline(infile, line);
                istringstream lineStream(line);
                string tile;
                for (int x=0; x < mapWidth; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val =  (unsigned char)atoi(tile.c_str());
                    if (val > 0) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        levelData[y][x] = val-1;
                    }
                    else {
                        levelData[y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}

bool Level::readEntityData(ifstream& infile) {
    string line;
    string type;
    while (getline(infile, line)) {
        if (line == "") {
            break;
        }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "type") {
            type = value;
        }
        else if (key == "location") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            float placeX = atof(xPosition.c_str())/16*TILE_SIZE;
            float placeY = atof(yPosition.c_str())/16*-TILE_SIZE;
            placeEntity(type, placeX, placeY);
        }
    }
    return true;
}

void Level::placeEntity(string& type, float placeX, float placeY) {
    if (type == "Start") {
        //player.x = placeX/TILE_SIZE/2;
        //player.y = -placeY/TILE_SIZE/2;
        player.x = placeX/TILE_SIZE;
        player.y = -placeY/TILE_SIZE;
    }
}