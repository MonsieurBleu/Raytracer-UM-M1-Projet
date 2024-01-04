#ifndef OBJECT_HPP
#define OBJECT_HPP

#define NO_INTERSECTION 10E9

#include <vector>
#include <Texture.hpp>
#include <memory>

using namespace glm;

struct rayContact
{
    float t = NO_INTERSECTION;
    float reflectivity = 0.f;
    float transparency = 0.f;
    vec3 position;
    vec3 color;
    vec3 normal = vec3(0);
    vec2 uv;
    bool inBackFace = false;
};

class Object
{
    public : 
        vec3 color = vec3(1);
        float reflectivity = 0.f;
        float transparency = 0.f;
        virtual rayContact trace(vec3 ray, vec3 origin);
};

class Sphere : public Object
{
    private :
        float radiusSquared;
        float radius;

    public :
        vec3 center;

        void setRadius(float r);
        virtual rayContact trace(vec3 ray, vec3 origin);

};

class Quad : public Object
{
    private : 
        vec3 normal;
        vec3 points[4] = {vec3(1, 0, 1), vec3(-1, 0, -1), vec3(-1, 0, 1), vec3(1, 0, -1)};

        vec3 min, max;

    public : 
        bool invertNormal = false;
        void genNormalMinMax();

        Quad();
        Quad(vec3 p1, vec3 p2, vec3 p3, vec3 p4);

        vec3 position;

        vec3 operator[](int i);

        void setPoints(vec3 p1, vec3 p2, vec3 p3, vec3 p4);
        virtual rayContact trace(vec3 ray, vec3 origin);
};

class Triangle : public Object
{
    private : 
        float iArea = 1.f;
        vec3 normal;

        vec3 position[3];
        vec3 normals[3];
        vec2 uvs[3];
        vec3 colors[3];

        vec3 center;
        vec3 minP;
        vec3 maxP;

        bool useVertexColors = false;

    public : 
        void setPoints(vec3 p1, vec3 p2, vec3 p3);
        void setNormals(vec3 n1, vec3 n2, vec3 n3);
        void setUVs(vec2 uv1, vec2 uv2, vec2 uv3);
        void setColors(vec3 c1, vec3 c2, vec3 c3);
        rayContact trace(vec3 ray, vec3 origin);

        vec3 getCenter() const;
        vec3 getMin() const;
        vec3 getMax() const;

        static uint64 traceCall;
        static uint64 missedTracedCall;
        static uint64 contactTraceCall;
        static uint64 tmpTraceCall;
        static bool tmpDebug;
};

struct MeshKDTreeNode
{
    char channel;
    char splitingAttempts = 0;
    float median;
    std::shared_ptr<MeshKDTreeNode> frontChild = NULL;
    std::shared_ptr<MeshKDTreeNode> backChild = NULL;
    vec3 max;
    vec3 min;
    std::vector<Triangle*> triangles;
};

class Mesh : public Object
{
    private : 
        bool useKDTree = false;
        MeshKDTreeNode tree;
        int kdTreeMaxDepth = 0;
        void genKDTree();
        void genKDNode(MeshKDTreeNode &node, int depth);

        rayContact traceKDNode(MeshKDTreeNode &firstNode, vec3 ray, vec3 origin);
        rayContact traceKDNodeQuads(MeshKDTreeNode &node, vec3 ray, vec3 origin);

        std::vector<Triangle> triangles;

        int texW, texH;
        unsigned char *tex = NULL;

        bool useVertexColors = false;

    public : 
        void readOBJ(std::string filePath, bool useVertexColors = false);
        rayContact trace(vec3 ray, vec3 origin);

        Texture tColor;
};


#endif
