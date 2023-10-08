#include <Camera.hpp>
#include <stb/stb_image_write.h>
#include <memory>
#include <vector>
#include <Object.hpp>
#include <Timer.hpp>

struct pixel
{
    uint8 r, g, b;
};

int main()
{
    ivec2 res = vec2(16, 16);

    std::shared_ptr<pixel> img(new pixel[res.x*res.y]);

    CameraState cs;
    Camera camera;
    camera.init(radians(70.0f), res.x, res.y, 0.1, 10000.0);
    camera.setCameraPosition(vec3(0.0, 0.0, 1.5));
    camera.lookAt(vec3(0.0, 0.0, 0.0));
    camera.setForceLookAtPoint(true);
    camera.updateProjectionViewMatrix(); 
    mat4 iViewProj = inverse(camera.getProjectionViewMatrix());
    // mat4 iProj = inverse(camera.getProjectionMatrix());
    // mat4 iView = inverse(camera.getViewMatrix());

    Sphere s;
    s.center = vec3(0, 0, 0);
    s.setRadius(0.5);

    float uvStep = 2.0/(float)res.x;
    std::vector<vec2> displascements = 
    {
        vec2(0.0, 0.0),
        uvStep*vec2(0.1548, 0.0358),
        uvStep*vec2(-0.0168, -1.0812),
        uvStep*vec2(-0.7581, 0.5893),
        uvStep*vec2(0.0589, -0.2568)
    };
    int nbSample = 4;

    BenchTimer timer("frame time");
    
    int id = 0;
    for(int i = 0; i < res.x; i++)
    for(int j = 0; j < res.y; j++)
    {
        vec3 fragColor(0.0);

        for(int k = 0; k < nbSample; k++)
        {
            vec3 color(0);

            vec2 uv((float)j/(float)res.y, (float)i/(float)res.x);
            uv = uv*vec2(2.0) - vec2(1.0) + vec2(std::rand()%256, std::rand()%256)*vec2(1.0/2E5);

            // near can be remplaced with camera.getPosition
            vec4 spf = iViewProj * vec4(uv.x, uv.y, 1.0, 1.0);
            vec3 far = vec3(spf)/spf.w;
            vec4 spn = iViewProj * vec4(uv.x, uv.y, 0.0, 1.0);
            vec3 near = vec3(spn)/spn.w;
            vec3 direction = normalize(far - near);
   

            timer.start();
            for(int i = 0; i < 1E8; i++)
            {
                rayContact test = s.trace(direction, near); 
                if(test.t != NO_INTERSECTION) 
                    color = test.position*vec3(i); 
            }
            timer.end();

            color = max(min(color, vec3(1.0)), vec3(0.0)); 
            fragColor += color;
        }
        fragColor = fragColor/vec3(nbSample);
        img.get()[id].r = fragColor.x*255;
        img.get()[id].g = fragColor.y*255;
        img.get()[id].b = fragColor.z*255;
        id ++;
    }
    
    std::cout << timer; 

    stbi_write_png("render/result.png", res.x, res.y, 3, img.get(), 0);

    return EXIT_SUCCESS;
}