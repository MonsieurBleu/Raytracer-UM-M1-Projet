#include <Camera.hpp>
#include <stb/stb_image_write.h>
#include <memory>
#include <vector>
#include <Timer.hpp>
#include <PhongLights.hpp>
#include <Scene.hpp>

struct pixel
{
    uint8 r, g, b;
};

uint64 Triangle::traceCall;
uint64 Triangle::tmpTraceCall;
uint64 Triangle::missedTracedCall;
uint64 Triangle::contactTraceCall;
bool Triangle::tmpDebug = false;

int main()
{
    Triangle::traceCall = 0;
    Triangle::tmpTraceCall = 0;
    Triangle::missedTracedCall = 0;
    Triangle::contactTraceCall = 0;

    ivec2 res = vec2(256);

    std::shared_ptr<pixel> img(new pixel[res.x*res.y]);

    CameraState cs;
    Camera camera;

    camera.init(radians(25.0f), res.x, res.y, 0.1, 10000.0);
    // camera.init(radians(10.0f), res.x, res.y, 0.1, 10000.0);


    // camera.setCameraPosition(vec3(4.85, 2.0, 0.0));
    // camera.lookAt(vec3(0.0, 2.0, 0.0));

    camera.setCameraPosition(vec3(40.0, 5.0, 25.0));
    // camera.setCameraPosition(vec3(40.0, 15.0, 25.0));
    camera.lookAt(vec3(0, 2, 0));
    // camera.lookAt(vec3(-2.5, 2, 0));
    camera.setForceLookAtPoint(true);

    // camera.setCameraPosition(vec3(5.0, 0.0, 0.0));
    // camera.setCameraDirection(normalize(vec3(-1, 0, 0)));


    camera.updateProjectionViewMatrix(); 
    mat4 iViewProj = inverse(camera.getProjectionViewMatrix());
    // mat4 iProj = inverse(camera.getProjectionMatrix());
    // mat4 iView = inverse(camera.getViewMatrix());

    std::shared_ptr<Sphere> s(new Sphere);
    s->center = vec3(1, 1.0, 1);
    s->setRadius(0.85);
    s->color = vec3(1.0, 0.5, 0.1);

    std::shared_ptr<Sphere> s2(new Sphere);
    s2->center = vec3(0, 1.0, -1);
    s2->setRadius(0.85);
    s2->color = vec3(0.1, 0.5, 1.0);
    
    float BoxSize = 4.0;
    std::shared_ptr<Quad> r(new Quad(
        vec3( 0.5, 0.f,  0.5)*vec3(BoxSize), 
        vec3(-0.5, 0.f, -0.5)*vec3(BoxSize), 
        vec3(-0.5, 0.f,  0.5)*vec3(BoxSize), 
        vec3( 0.5, 0.f, -0.5)*vec3(BoxSize)
    ));
    r->color = vec3(1.0);
    r->position = vec3(0.0, 0.0, 0.0);

    std::shared_ptr<Quad> r2(new Quad(
        vec3( 0.5,  1, 0.f)*vec3(BoxSize), 
        vec3(-0.5,  0, 0.f)*vec3(BoxSize), 
        vec3(-0.5,  1, 0.f)*vec3(BoxSize), 
        vec3( 0.5,  0, 0.f)*vec3(BoxSize)
    ));
    r2->color = vec3(1.0, 0.1, 0.1);
    r2->invertNormal = true;
    r2->position = vec3(0.0, 0.0, -BoxSize*0.5);

    std::shared_ptr<Quad> r3(new Quad(
        vec3( 0.5,  1, 0.f)*vec3(BoxSize), 
        vec3(-0.5,  0, 0.f)*vec3(BoxSize), 
        vec3(-0.5,  1, 0.f)*vec3(BoxSize), 
        vec3( 0.5,  0, 0.f)*vec3(BoxSize)
    ));
    r3->color = vec3(0.1, 1.0, 0.5);
    r3->position = vec3(0.0, 0.0, 2.0);

    std::shared_ptr<Quad> r4(new Quad(
        vec3( 0.5, 0.f,  0.5)*vec3(BoxSize), 
        vec3(-0.5, 0.f, -0.5)*vec3(BoxSize), 
        vec3(-0.5, 0.f,  0.5)*vec3(BoxSize), 
        vec3( 0.5, 0.f, -0.5)*vec3(BoxSize)
    ));
    r4->color = vec3(1.0);
    r4->position = vec3(0.0, BoxSize, 0.0);
    r4->invertNormal = true;
    

    std::shared_ptr<Quad> r5(new Quad(
        vec3(0.f,  1,  0.5)*vec3(BoxSize), 
        vec3(0.f,  0, -0.5)*vec3(BoxSize), 
        vec3(0.f,  1, -0.5)*vec3(BoxSize), 
        vec3(0.f,  0,  0.5)*vec3(BoxSize)
    ));
    r5->color = vec3(0.5, 0.1, 1.0);
    r5->position = vec3(-BoxSize*0.5, 0.0, 0.0);

    r->genNormalMinMax();
    r2->genNormalMinMax();
    r3->genNormalMinMax();
    r4->genNormalMinMax();
    r5->genNormalMinMax();

    float uvStep = 2.0/(float)res.x;
    // std::vector<vec2> displascements = 
    // {
    //     vec2(0.0, 0.0),
    //     uvStep*vec2(0.1548, 0.0358),
    //     uvStep*vec2(-0.0168, -1.0812),
    //     uvStep*vec2(-0.7581, 0.5893),
    //     uvStep*vec2(0.0589, -0.2568)
    // };
    int nbSample = 1;

    PhongLight sun;
    // sun.direction = normalize(vec3(-1, -0.5, 0));
    sun.position = vec3(5E2)*normalize(vec3(7, 6, 2.7)); // vec3(5E2)*normalize(vec3(-7, 6, 2.7));
    sun.color = vec3(1.0, 0.95, 0.8);
    sun.intensity = 1.5;
    sun.radius = 1E4;

    PhongLight light;
    light.position = vec3(5E2)*normalize(vec3(1, 3.5, 0));
    light.color = vec3(0.5, 0.75, 1.0);
    light.intensity = 0.35;
    light.radius = 1E3;

    std::shared_ptr<Triangle> t1(new Triangle);
    t1->setPoints(vec3(-1, 3, -1), vec3(-1, 0, -1), vec3(1, 0, 1));

    std::shared_ptr<Mesh> fox(new Mesh);
    fox->readOBJ("./ressources/fox.obj");
    fox->readTexture("./ressources/fox.png");
    fox->reflectivity = 0.f;

    std::shared_ptr<Mesh> rock(new Mesh);
    rock->readOBJ("./ressources/rock.obj");
    rock->readTexture("./ressources/rock.png");
    rock->reflectivity = 0.1f;

    std::shared_ptr<Mesh> mountain(new Mesh);
    mountain->readOBJ("./ressources/mountain.obj");
    mountain->readTexture("./ressources/mountain.png");
    mountain->reflectivity = 0.f;

    std::shared_ptr<Mesh> sakura(new Mesh);
    sakura->readOBJ("./ressources/sakura.obj", true);
    sakura->reflectivity = 0.f;

    std::shared_ptr<Mesh> water(new Mesh);
    water->readOBJ("./ressources/water.obj", true);
    water->reflectivity = 0.7f;

    Scene scene;
    // scene.add(s);
    // scene.add(s2);
    // scene.add(r);
    // scene.add(r2);
    // scene.add(r3);
    // scene.add(r4);
    // scene.add(r5);
    // scene.add(sun);
    // scene.add(light);
    // scene.add(t1);
    // scene.add(fox);
    // scene.add(rock);
    scene.add(mountain);
    // scene.add(sakura);
    // scene.add(water);
    // scene.ambientLight = vec3(0.25);
    scene.ambientLight = vec3(1);

    BenchTimer timer("frame time");
    timer.start();

    int id = 0;
    for(int i = 0; i < res.x; i++)
    for(int j = 0; j < res.y; j++)
    {
        vec3 fragColor(0.0);

        for(int k = 0; k < nbSample; k++)
        {
            vec3 color(0);

            vec2 uv((float)j/(float)res.y, (float)i/(float)res.x);
            uv = -uv*vec2(2.0) + vec2(1.0);
            uv += k ? vec2(std::rand()%256, std::rand()%256)*vec2(1E-4) : vec2(0);
            uv = vec2(-uv.x, uv.y);

            Triangle::tmpDebug = uv.x < 0.6 && uv.x > 0.59 && uv.y < 0.5 && uv.y > 0.49; 

            vec4 spf = iViewProj * vec4(uv.x, uv.y, 1.0, 1.0);
            vec3 far = vec3(spf)/spf.w;
            vec3 direction = normalize(far - camera.getPosition());

            // rayContact rc = scene.getResult(direction, camera.getPosition());
            rayContact rc = scene.getResultReflectivity(direction, camera.getPosition(), 4);

            color = rc.color;

            if(Triangle::tmpDebug)
            {
                color = mix(color, vec3(0, 1, 0), 0.5);
            }

            color = max(min(color, vec3(1.0)), vec3(0.0)); 
            fragColor += color;
        }
        fragColor = fragColor/vec3(nbSample);
        img.get()[id].r = fragColor.x*255;
        img.get()[id].g = fragColor.y*255;
        img.get()[id].b = fragColor.z*255;
        id ++;
    }
    timer.end();
    std::cout << timer; 

    stbi_write_png("render/result.png", res.x, res.y, 3, img.get(), 0);

    std::cout << "Triangles Trace Calls : " << Triangle::traceCall / 1E6f << "\n";
    std::cout << "Triangles Trace Calls That resulted in miss    : " << Triangle::missedTracedCall / 1E6f << "\n";
    std::cout << "Triangles Trace Calls That resulted in contact : " << Triangle::contactTraceCall / 1E6f << "\n";

    return EXIT_SUCCESS;
}