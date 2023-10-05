#include <Camera.hpp>
#include <stb/stb_image_write.h>
#include <memory>

#include <Object.hpp>

struct pixel
{
    uint8 r, g, b;
};

int main()
{
    ivec2 res = vec2(1024, 1024);

    std::shared_ptr<pixel> img(new pixel[res.x*res.y]);


    Camera camera;
    camera.setCameraPosition(vec3(1.0, 0.0, 1.0));
    camera.setForceLookAtPoint(true);
    camera.lookAt(vec3(0.0, 0.0, 0.0));
    mat4 unproj = inverse(camera.updateProjectionViewMatrix());

    int id = 0;
    for(int i = 0; i < res.x; i++)
    for(int j = 0; j < res.y; j++)
    {
        vec3 color(0.0);
        vec2 uv((float)i/(float)res.x, (float)j/(float)res.y);

        vec3 direction = camera.getPosition() - vec3(vec4(uv.x*2.0 - 1.0, uv.y*2.0 - 1.0, 0.0, 1.0)*unproj);
        
        color = normalize(direction)*vec3(0.5) + vec3(0.5);

        img.get()[id].r = color.x*255;
        img.get()[id].g = color.y*255;
        img.get()[id].b = color.z*255;

        id ++;
    }


    stbi_write_png("render/result.png", res.x, res.y, 3, img.get(), 0);

    return EXIT_SUCCESS;
}