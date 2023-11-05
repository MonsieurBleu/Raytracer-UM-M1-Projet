#include <PhongLights.hpp>

vec3 PhongLight::getLighting(const rayContact& rc, vec3 ray) const
{
    vec3 lightDist = position - rc.position;
    vec3 lightDirection = normalize(lightDist);
    
    float distFactor = lightDist.length()/radius;
    distFactor = distFactor >= 1.f ? 0.f : 1.f - distFactor*distFactor;

    float diffuse = dot(rc.normal, lightDirection);
    diffuse = diffuse < 0.f ? 0.f : diffuse;

    vec3 halfwayDir = normalize(lightDirection - ray);
    float blinnSpec = pow(max(dot(rc.normal, halfwayDir), 0.f), 32.f);

    return distFactor * intensity * rc.color * color * (diffuse + blinnSpec);
}

