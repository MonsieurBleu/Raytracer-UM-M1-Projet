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
    firstContact.color = vec3(0);

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

    int smoothShadowStep = 32;

    vec3 lightResult = vec3(0);
    rayContact lightContact;
    for(auto i : lights)
    {
        float shadow = 0.0;

        for(int j = 0; j < smoothShadowStep; j++)
        {
            vec3 lightContactOrigin = firstContact.position;
            lightContactOrigin += firstContact.normal*vec3(0.001);
            vec3 lightPosition = i.position+vec3(0.0005*(100 - std::rand()%200));
            vec3 lightContactRay = normalize(lightPosition-lightContactOrigin);
            lightContact = trace(lightContactRay, lightContactOrigin);

            float dist = length(lightPosition-firstContact.position);
            shadow += lightContact.t < dist ? 1.f : 0.f;
        }

        shadow = 1.0 - shadow/smoothShadowStep;
        lightResult += shadow == 0.f ? vec3(0.f) : vec3(shadow) * i.getLighting(firstContact, ray);
    }

    firstContact.color = lightResult + (firstContact.color*ambientLight);

    return firstContact;
}
