#include <Camera.hpp>
#include <stb/stb_image_write.h>
#include <memory>
#include <vector>
#include <Timer.hpp>
#include <PhongLights.hpp>
#include <Scene.hpp>
#include <Utils.hpp>

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

    // camera.init(radians(25.0f), res.x, res.y, 0.1, 10000.0);
    camera.init(radians(30.0f), res.x, res.y, 0.1, 10000.0);


    // camera.setCameraPosition(vec3(4.85, 2.0, 0.0));
    // camera.lookAt(vec3(0.0, 2.0, 0.0));

    // camera.setCameraPosition(vec3(40.0, 5.0, 25.0));

    camera.setCameraPosition(
        vec3(50.f)*normalize(vec3(-10.0, 0.0, 30.0)));

    // camera.lookAt(vec3(0, 2, 0));
    camera.lookAt(vec3(-3.5, -2.3, 0));
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

    int nbSample = 1;

    PhongLight sun;
    // sun.direction = normalize(vec3(-1, -0.5, 0));
    sun.position = vec3(5E2)*normalize(vec3(5, 6, 10)); // vec3(5E2)*normalize(vec3(-7, 6, 2.7));
    sun.color = vec3(1.0, 0.95, 0.8);
    sun.intensity = 0.75;
    sun.radius = 1E4;

    PhongLight light;
    light.position = vec3(5E2)*normalize(vec3(1, 3.5, 0));
    light.color = vec3(0.75, 0.5, 1.0);
    light.intensity = 0.5;
    light.radius = 1E3;

    std::shared_ptr<Triangle> t1(new Triangle);
    t1->setPoints(vec3(-1, 3, -1), vec3(-1, 0, -1), vec3(1, 0, 1));

    std::shared_ptr<Mesh> fox(new Mesh);
    fox->readOBJ("./ressources/fox.obj");
    fox->tColor.readTexture("./ressources/fox.png");
    // fox->transparency = 0.65;
    // fox->reflectivity = 1.f;

    std::shared_ptr<Mesh> rock(new Mesh);
    rock->readOBJ("./ressources/rock2.obj");
    rock->tColor.readTexture("./ressources/rock.png");
    rock->reflectivity = 0.1;

    std::shared_ptr<Mesh> mountain(new Mesh);
    mountain->readOBJ("./ressources/mountain.obj");
    mountain->tColor.readTexture("./ressources/mountain.png");
    // mountain->reflectivity = 0.75;

    std::shared_ptr<Mesh> sakura(new Mesh);
    sakura->readOBJ("./ressources/sakura.obj", true);
    // sakura->reflectivity = 0.75;

    std::shared_ptr<Mesh> water(new Mesh);
    water->readOBJ("./ressources/water.obj", true);
    water->reflectivity = 0.75;
    // water->transparency = 0.5;

    Scene scene;
    scene.skybox.readTexture("./ressources/skybox/s.jpg");
    // s->reflectivity = 0.9;
    // s2->reflectivity = 0.9;
    // r->reflectivity = 0.9;
    // r2->reflectivity = 0.9;
    // r3->reflectivity = 0.9;
    // r4->reflectivity = 0.9;
    // r5->reflectivity = 0.9;
    // scene.add(s);
    // scene.add(s2);
    // scene.add(r);
    // scene.add(r2);
    // scene.add(r3);
    // scene.add(r4);
    // scene.add(r5);
    scene.add(sun); 
    scene.add(light);
    // scene.add(t1);
    // scene.add(fox);
    scene.add(rock);
    // scene.add(mountain);
    scene.add(sakura);
    scene.add(water);
    scene.ambientLight = vec3(0.5);
    // scene.ambientLight = vec3(1);



    std::shared_ptr<Mesh> flamingo(new Mesh);
    flamingo->readOBJ("./ressources/flamingo2/model.obj", true);
    scene.add(flamingo);


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
            uv = vec2(-uv.x, uv.y);

            vec4 spf = iViewProj * vec4(uv.x, uv.y, 1.0, 1.0);
            vec3 far = vec3(spf)/spf.w;
            vec3 direction = normalize(far - camera.getPosition());

            rayContact rc = scene.getResultTranceparency(direction, camera.getPosition(), 16);

            color = rc.color;

            color = clamp(color, vec3(0), vec3(1)); 
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