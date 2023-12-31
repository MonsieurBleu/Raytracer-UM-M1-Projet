#include <PhongLights.hpp>
#include <list>
#include <memory>

#define SceneObject std::shared_ptr<Object>

class Scene
{
    private :
        std::list<SceneObject> objects;
        std::list<PhongLight> lights;

    public : 
        Texture skybox;

        vec3 ambientLight = vec3(0.2);

        Scene& add(SceneObject object);
        Scene& add(PhongLight light);
        rayContact trace(vec3 ray, vec3 origin);
        rayContact getResult(vec3 ray, vec3 origin);
        rayContact getResultReflectivity(vec3 ray, vec3 origin, int maxIt);
        rayContact getResultTranceparency(vec3 ray, vec3 origin, int maxIt);
};