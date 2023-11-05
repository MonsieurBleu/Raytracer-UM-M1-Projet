#ifndef PHONGLIGHTS_HPP
#define PHONGLIGHTS_HPP

// #include <glm/glm.hpp>
// #include <iostream>


#include <Object.hpp>
using namespace glm;

// BlinnPhong point Light
class PhongLight
{
    public : 
        vec3 position;
        vec3 color = vec3(1.0);
        float intensity = 1.0;
        float radius = 1.0;

        vec3 getLighting(const rayContact& rc, vec3 ray) const;
};

#endif