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


// {
//     rayContact result;
//     vec3 v = origin - center;

//     float a = dot(ray, ray);
//     float b = 2 * dot(ray, v);
//     float c = dot(v, v) - radius * radius;

//     float delta = b * b - 4 * a * c;
//     if (delta < 0)
//     {
//         return result;
//     }

//     float t1 = (-b - sqrt(delta)) / (2 * a);
//     float t2 = (-b + sqrt(delta)) / (2 * a);

//     if (t1 < 0.0001f && t2 < 0.0001f)
//     {
//         return result;
//     }

//     if (t1 < 0.0001f)
//     {
//         result.t = t2;
//     }
//     else if (t2 < 0.0001f)
//     {
//         result.t = t1;
//     }
//     else
//     {
//         result.t = min(t1, t2);
//     }

//     result.position = origin + ray * result.t;

//     result.normal = normalize(result.position - center);

//     return result;
// }




// {
//     rayContact result;

//     vec3 AC = center - origin;
//     float norm = dot(ray, AC) / length(AC);

//     /*
//         We check if the potential intersection point is
//         behind or in front of the origin

//         dot(Cbis-origin, ray) < 0 
//         can be replace with 
//         norm < 0

//         because :
//             dot(Cbis-origin, ray) = dot(ray*norm, ray) = norm*||ray||²
//             if ray < 0 then dot(ray*norm, ray) < 0
//     */
//     // if(norm < 0)
//     //     return result;

//     // sphere center projection on the ray line
//     vec3 Cbis = origin + ray*norm;

//     // cosine of the intersection point 
//     float res = length(Cbis - center);

//     // if(res > radius)
//     //     return result;

//     result.t = sin(acos(res));
//     // result.position = Cbis + ray*result.t;
//     result.position = Cbis;
//     result.normal = normalize(center-result.position);

//     return result;
// }

#endif
