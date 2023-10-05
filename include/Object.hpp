#ifndef OBJECT_HPP
#define OBJECT_HPP

#define NO_INTERSECTION 10e9

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;

class Object
{
    public : 
        virtual float trace(vec3 ray, vec3 origin);
};

class Sphere : public Object
{
    private :


    public :
        float radius;
        vec3 position;

        float trace(vec3 ray, vec3 origin);

};

float Sphere::trace(vec3 ray, vec3 origin)
{
    vec3 AC = normalize(position - origin);
    float norm = dot(ray, AC) / length(AC);

    // on calcul la projection du centre de la sphère sur la droite 
    vec3 Cbis = origin + ray*norm;

    float res = length(Cbis - position);

    // std::cout << res << "\n";

    if(res <= radius)
        return res;
    else
        return NO_INTERSECTION;

    return res;
}


#endif
