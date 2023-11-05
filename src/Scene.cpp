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
        if(tmp.t < firstContact.t)
            firstContact = tmp;
    }

    return firstContact;
}

rayContact Scene::getResult(vec3 ray, vec3 origin)
{
    rayContact firstContact = trace(ray, origin);

    vec3 scale = vec3(0.00075);
    vec3 displascements[] = 
    {
        vec3(0),
        // normalize(vec3(0.2584818, 1.004456126, -0.895113))*scale,
        // normalize(vec3(0.0041899, -0.311986848, 0.125948))*scale,
        // normalize(vec3(0.5841963, 0.697478951, 0.7489515))*scale,
        // normalize(vec3(-2.481630, 0.959004801, 0.0158941))*scale,
        // normalize(vec3(-0.281630, -0.19850801, -0.0158941))*scale,
    };

    vec3 lightResult = vec3(0);
    rayContact lightContact;
    for(auto i : lights)
    {
        float shadow = 0.0;
        for(int j = 0; j < sizeof(displascements)/sizeof(vec3); j++)
        {
            
            vec3 lightContactOrigin = firstContact.position + displascements[j];
            lightContactOrigin += firstContact.normal*vec3(0.001);
            vec3 lightContactRay = normalize(i.position-lightContactOrigin);
            lightContact = trace(lightContactRay, lightContactOrigin);

            float dist = length(i.position-firstContact.position);

            if(lightContact.t < dist)
                shadow += 1.0;
        }

        shadow = 1.0 - shadow/(sizeof(displascements)/sizeof(vec3));

        lightResult += shadow == 0.f ? vec3(0.f) : vec3(shadow) * i.getLighting(firstContact, ray);
    }

    // firstContact.color = lightResult;

    firstContact.color = lightResult + (firstContact.color*ambientLight);

    return firstContact;
}
