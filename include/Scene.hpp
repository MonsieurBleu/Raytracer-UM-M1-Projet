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

        vec3 ambientLight = vec3(0.2);

        Scene& add(SceneObject object);
        Scene& add(PhongLight light);
        rayContact trace(vec3 ray, vec3 origin);
        rayContact getResult(vec3 ray, vec3 origin);

};