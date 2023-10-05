#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glm/glm.hpp>

using namespace glm;

class Object
{
    public : 
        virtual float trace(vec3 ray, vec3 origin);
};

class sphere : public Object
{
    private :

        float radius;
        vec3 position;

    public :

        float trace(vec3 ray, vec3 origin);

};

float sphere::trace(vec3 ray, vec3 origin)
{
    /*
    */
    vec3 AC = origin-position;
    float norm = dot(ray, AC) * length(AC);

    // vec3 Cbis()

    // https://moodle.umontpellier.fr/pluginfile.php/2358964/mod_resource/content/1/HAI719I-Rappel%20vecteurs.pdf
    // page 26
}


#endif
