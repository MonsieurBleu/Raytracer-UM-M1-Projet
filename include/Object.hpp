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
        rayContact trace(vec3 ray, vec3 origin);

};

class Rectangle : public Object
{
    private : 
        vec3 normal;

    public : 
        vec3 halfSize;
        vec3 position;

        void setNormale(vec3 n);      
        rayContact trace(vec3 ray, vec3 origin);
};

void Sphere::setRadius(float r)
{
    radius = r;
    radiusSquared = r*r;
}

rayContact Sphere::trace(vec3 ray, vec3 origin)
{
    rayContact result;
    vec3 oc = origin-center;
    float a = ray.x*ray.x + ray.y*ray.y + ray.z*ray.z;
    float b = ray.x*oc.x + ray.y*oc.y + ray.z*oc.z;
    float c = oc.x*oc.x + oc.y*oc.y + oc.z*oc.z - radiusSquared;
    
    float sqrtDelta = 2.f*sqrtf(b*b - a*c);
    float x1 = 0.5f*(-b-sqrtDelta)/a;
    float x2 = 0.5f*(-b+sqrtDelta)/a;

    x1 = x1 < 0.f ? NO_INTERSECTION : x1;
    x2 = x2 < 0.f ? NO_INTERSECTION : x2;
    result.t = x1 < x2 ? x1 : x2;
        
    result.position = origin + ray*result.t;
    result.normal = normalize(result.position - center);

    return result;
}

void Rectangle::setNormale(vec3 n)
{
    normal = normalize(n);
}

rayContact Rectangle::trace(vec3 ray, vec3 origin)
{
    rayContact result;
    result.t = (dot(position, normal)-dot(origin, normal))/dot(ray, normal);

    result.t = result.t < 0.0 || result.t == NAN || result.t == INFINITY ? NO_INTERSECTION : result.t;

    result.position = origin + ray*result.t;
    result.normal = normal;

    // Use the halfsize to check if the point on the plane is inside the rectangle 
    result.t = length(position-result.position) > 1.1 ? NO_INTERSECTION : result.t;

    return result;
}


#endif
