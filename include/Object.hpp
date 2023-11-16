#ifndef OBJECT_HPP
#define OBJECT_HPP

#define NO_INTERSECTION 10E9

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using namespace glm;

struct rayContact
{
    float t = NO_INTERSECTION;
    vec3 position;
    vec3 color;
    vec3 normal;
};

class Object
{
    public : 
        vec3 color = vec3(1);
        virtual rayContact trace(vec3 ray, vec3 origin);
};

class Sphere : public Object
{
    private :
        float radiusSquared;
        float radius;

    public :
        vec3 center;

        void setRadius(float r);
        virtual rayContact trace(vec3 ray, vec3 origin);

};

class Quad : public Object
{
    private : 
        vec3 normal;
        vec3 points[4] = {vec3(1, 0, 1), vec3(-1, 0, -1), vec3(-1, 0, 1), vec3(1, 0, -1)};

        vec3 min, max;

    public : 
        bool invertNormal = false;
        void genNormalMinMax();

        Quad();
        Quad(vec3 p1, vec3 p2, vec3 p3, vec3 p4);

        vec3 position;

        vec3 operator[](int i);

        void setPoints(vec3 p1, vec3 p2, vec3 p3, vec3 p4);
        virtual rayContact trace(vec3 ray, vec3 origin);
};

class Triangle : public Object
{
    private : 
        float iArea = 1.f;
        vec3 normal;

        vec3 position[3];
        vec3 normals[3];
        vec2 uvs[3];

    public : 
        void setPoints(vec3 p1, vec3 p2, vec3 p3);
        void setNormals(vec3 n1, vec3 n2, vec3 n3);
        void setUVs(vec2 uv1, vec2 uv2, vec2 uv3);
        rayContact trace(vec3 ray, vec3 origin);
};

class Mesh : public Object
{
    private : 
        std::vector<Triangle> triangles;

    public : 
        void readOBJ(std::string filePath);
        rayContact trace(vec3 ray, vec3 origin);
};


#endif
