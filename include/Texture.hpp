#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;

class Texture
{

    private : 
        int texW, texH;
        unsigned char *tex = NULL;
    
    public : 
        vec3 getPixel(int x, int y);
        vec3 texture(vec2 uv);
        void readTexture(std::string filePath);
};


#endif