#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define SPRITE_COUNT_X 25
#define SPRITE_COUNT_Y 23
#define TILE_SIZE 0.15f
#define LEVEL_WIDTH 64
#define LEVEL_HEIGHT 64

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Matrix.h"

using namespace std;

class Level {
public:
    Level() {};
    ~Level(){};
    
    unsigned char** levelData;
    int mapWidth;
    int mapHeight;
    GLuint spriteSheet;
    
    Entity player;
    
    vector<float> texCoordData;
    vector<float> vertexData;
    
    void buildLevel();
    bool readHeader(ifstream& infile);
    bool readLayerData(ifstream& infile);
    bool readEntityData(ifstream& infile);
    void placeEntity(string& type, float placeX, float placeY);
};