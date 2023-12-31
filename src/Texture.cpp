#include <Texture.hpp>
#include <stb/stb_image.h>
#include <Utils.hpp>

void Texture::readTexture(std::string filePath)
{
    int n, fileStatus;
    fileStatus = stbi_info(filePath.c_str(), &texW, &texH, &n);

    if(!fileStatus)
    {
        std::cerr 
        << TERMINAL_ERROR << "Texture2D::loadFromFile : stb error, can't load image " 
        << TERMINAL_FILENAME << filePath 
        << TERMINAL_ERROR << ". This file either don't exist or the format is not supported.\n"
        << TERMINAL_RESET; 
    }

    tex = stbi_load(filePath.c_str(), &texW, &texH, &n, 3);

    if(!tex)
    {
        std::cerr 
        << TERMINAL_ERROR << "Texture2D::loadFromFile : stb error, can load info but can't load pixels of image " 
        << TERMINAL_FILENAME << filePath << "\n"
        << TERMINAL_RESET; 
    }
};

vec3 Texture::getPixel(int x, int y)
{
    x = clamp(x, 0, texH-1);
    y = clamp(y, 0, texW-1);
    int c = 3*(texW*x + y);
    return vec3(
        tex[c],
        tex[c + 1],
        tex[c + 2]
    )/vec3(255);
}

vec3 Texture::texture(vec2 uv)
{
    if(!tex) return vec3(0);

    uv = vec2(uv.y, uv.x);

    float x = texH*uv.x;
    float y = texW*uv.y;

    vec3 c1 = getPixel(floor(x), floor(y));
    vec3 c2 = getPixel(ceil(x), floor(y));
    vec3 c3 = getPixel(floor(x), ceil(y));
    vec3 c4 = getPixel(ceil(x), ceil(y));

    vec3 r = (c1+c2+c3+c4)*vec3(1/4.0);

    return r;
}