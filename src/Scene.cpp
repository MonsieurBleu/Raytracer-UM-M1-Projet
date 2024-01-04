#include <Scene.hpp>
#include <Utils.hpp>

Scene& Scene::add(SceneObject object)
{
    objects.push_back(object);
    return *this;
}

Scene& Scene::add(PhongLight light)
{
    lights.push_back(light);
    return *this;
}

rayContact Scene::trace(vec3 ray, vec3 origin)
{
    rayContact firstContact;
    

    for(auto i : objects)
    {
        rayContact tmp = i->trace(ray, origin);
        firstContact = tmp.t < firstContact.t ? tmp : firstContact;
    }

    return firstContact;
}

vec2 getPhiTheta(vec3 v)
{
    float theta = asin(v.y);
    float phiS = cos(theta);
    float phi = atan2(v.z/phiS, v.x/phiS);
    return vec2(phi, theta);
}

rayContact Scene::getResult(vec3 ray, vec3 origin)
{
    rayContact firstContact = trace(ray, origin);

    int prenumbraDetectionStep = 8;
    int smoothShadowStep = 64;

    vec3 lightResult = vec3(0);
    rayContact lightContact;

    if(firstContact.t >= NO_INTERSECTION)
    {
        /* Calculating theta & phi angle of the ray vector */
        float theta = asin(-ray.y);
        float phiS = cos(theta);
        float phi = atan2(ray.z/phiS, -ray.x/phiS);

        /* Normalazing to the range [0 - 1] */
        theta = (theta/M_PI) + 0.5; 
        phi = (phi/M_PI)*0.5 + 0.5;

        firstContact.color = skybox.texture(vec2(phi, theta));
    }
    else
    {
        for(auto i : lights)
        {
            float shadow = 0.0;

            vec3 o = firstContact.position + firstContact.normal*vec3(0.001);
            vec3 r = normalize(i.position-o);
            float dist = length(i.position-firstContact.position);

            int j = 0;
            for(; j < prenumbraDetectionStep; j++)
            {
                vec3 noise = vec3(0.01)*normalize(vec3(gold_noise3(o, j), gold_noise3(o, j<<2), gold_noise3(o, j<<3)));
                lightContact = trace(normalize(r+noise), o);
                shadow += lightContact.t < dist ? 1.f : 0.f;
            }

            if(shadow != 0.f && shadow != (float)prenumbraDetectionStep)
                for(; j < smoothShadowStep+prenumbraDetectionStep; j++)
                {
                    vec3 noise = vec3(0.02)*vec3(gold_noise3(o, j), gold_noise3(o, j<<2), gold_noise3(o, j<<3));
                    lightContact = trace(normalize(r+noise), o);
                    shadow += lightContact.t < dist ? 1.f : 0.f;
                }

            shadow = 1.0 - shadow/j;
            
            lightResult += shadow == 0.f ? vec3(0.f) : vec3(shadow) * i.getLighting(firstContact, ray);
        }

        firstContact.color = lightResult + (firstContact.color*ambientLight);
    }

    return firstContact;
}

rayContact Scene::getResultReflectivity(vec3 ray, vec3 origin, int maxIt)
{
    rayContact contacts[maxIt+1];
    vec3 view[maxIt+1];
    contacts[0] = getResult(ray, origin);
    view[0] = ray;
    int it = 1;

    for(; it < maxIt+1; it++)
    {
        rayContact &c = contacts[it-1];

        if(c.reflectivity <= 0.f || c.t >= NO_INTERSECTION)
            break;

        view[it] = reflect(view[it-1], c.normal);
        vec3 p = c.position + c.normal*vec3(0.001);
        contacts[it] = getResult(view[it], p);
    }

    if(it == maxIt+1) it--;

    for(it -= 1; it >= 0; it--)
        contacts[it].color = mix(contacts[it].color, contacts[it+1].color, contacts[it].reflectivity);

    return contacts[0];
}


rayContact Scene::getResultTransparency(vec3 ray, vec3 origin, int maxIt)
{
    rayContact contacts[maxIt+1];
    vec3 view[maxIt+1];
    contacts[0] = getResultReflectivity(ray, origin, maxIt);
    view[0] = ray;
    int it = 1;

    for(; it < maxIt+1; it++)
    {
        rayContact &c = contacts[it-1];

        if(c.transparency <= 0.f || c.t >= NO_INTERSECTION)
            break;

        /* 
            Refraction
        */
        const float ratio = 1.33;
        vec3 normal = c.inBackFace ? -c.normal : c.normal;
        float etha = c.inBackFace ? ratio : 1.f/ratio;
        view[it] = refract(view[it-1], normal, etha);
        vec3 p = c.position - normal*vec3(0.001);

        contacts[it] = getResultReflectivity(view[it], p, maxIt);
    }

    if(it == maxIt+1) it--;

    for(it -= 2; it >= 0; it--)
        contacts[it].color = mix(contacts[it].color, contacts[it+1].color, contacts[it].transparency);

    return contacts[0];
}