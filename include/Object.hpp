#ifndef OBJECT_HPP
#define OBJECT_HPP

#define NO_INTERSECTION 10E9

#include <glm/glm.hpp>
#include <iostream>

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



#endif
