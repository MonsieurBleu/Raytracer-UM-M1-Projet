#include <Scene.hpp>

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

rayContact Scene::getResult(vec3 ray, vec3 origin)
{
    rayContact firstContact = trace(ray, origin);

    int smoothShadowStep = 1;
    vec3 lightResult = vec3(0);
    rayContact lightContact;
    for(auto i : lights)
    {
        float shadow = 0.0;

        for(int j = 0; j < smoothShadowStep; j++)
        {
            vec3 lightContactOrigin = firstContact.position;
            lightContactOrigin += firstContact.normal*vec3(0.001);
            // vec3 lightPosition = i.position+vec3(0.0005*(100 - std::rand()%200));
            vec3 lightPosition = i.position;
            vec3 lightContactRay = normalize(lightPosition-lightContactOrigin);
            lightContact = trace(lightContactRay, lightContactOrigin);

            float dist = length(lightPosition-firstContact.position);
            shadow += lightContact.t < dist ? 1.f : 0.f;
        }

        shadow = 1.0 - shadow/smoothShadowStep;
        lightResult += shadow == 0.f ? vec3(0.f) : vec3(shadow) * i.getLighting(firstContact, ray);
    }

    firstContact.color = lightResult + (firstContact.color*ambientLight);

    // firstContact.color = firstContact.t != NO_INTERSECTION ? firstContact.color :  vec3(0x87, 0xCE, 0xEB)/vec3(255.0);
    firstContact.color = firstContact.t != NO_INTERSECTION ? firstContact.color :  vec3(0xE6, 0x9F, 0x83)/vec3(255.0);

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

        if(c.reflectivity <= 0.f || c.t == NO_INTERSECTION)
            break;

        contacts[it] = getResult(
            reflect(view[it-1], c.normal),
            c.position +  c.normal*vec3(0.001)
        );
    }

    for(it -= 2; it >= 0; it--)
        contacts[it].color = mix(contacts[it].color, contacts[it+1].color, contacts[it].reflectivity);

    return contacts[0];
}

