#include <Object.hpp>
#include <cstring>
#include <stb/stb_image.h>
#include <Utils.hpp>
#include <algorithm>
#include <list>

rayContact Object::trace(vec3 ray, vec3 origin)
{
    return rayContact();
}

void Sphere::setRadius(float r)
{
    radius = r;
    radiusSquared = r*r;
}

rayContact Sphere::trace(vec3 ray, vec3 origin)
{
    rayContact result;
    result.reflectivity = reflectivity;
    vec3 oc = origin-center;
    float a = ray.x*ray.x + ray.y*ray.y + ray.z*ray.z;
    float b = ray.x*oc.x + ray.y*oc.y + ray.z*oc.z;
    float c = oc.x*oc.x + oc.y*oc.y + oc.z*oc.z - radiusSquared;
    
    float sqrtDelta = sqrtf(b*b - a*c);
    float x1 = -b-sqrtDelta;
    float x2 = -b+sqrtDelta;

    x1 = x1 < 0.f ? NO_INTERSECTION : x1;
    x2 = x2 < 0.f ? NO_INTERSECTION : x2;
    result.t = x1 < x2 ? x1/a : x2/a;
        
    result.position = origin + ray*result.t;
    result.normal = normalize(result.position - center);

    result.color = color;

    return result;
}

Quad::Quad()
{
    genNormalMinMax();
}

Quad::Quad(vec3 p1, vec3 p2, vec3 p3, vec3 p4)
{
    setPoints(p1, p2, p3, p4);
}

vec3 Quad::operator[](int i)
{
    return points[i];
}

void Quad::genNormalMinMax()
{
    normal = normalize(cross(points[1]-points[0], points[2]-points[3]));
    normal *= invertNormal ? -1 : 1;
    // std::cout << normal.x << " " << normal.y << " " << normal.z << "\n";

    min = glm::min(glm::min(points[0], points[1]), glm::min(points[2], points[3]));
    max = glm::max(glm::max(points[0], points[1]), glm::max(points[2], points[3]));
    // We add a small imprecision, to prevent floating point precision bugs in certain situation
    float p = 0.00001;
    min -= p;
    max += p;
    min += position;
    max += position;

}

void Quad::setPoints(vec3 p1, vec3 p2, vec3 p3, vec3 p4)
{
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    points[3] = p4;

    genNormalMinMax();
}

rayContact Quad::trace(vec3 ray, vec3 origin)
{
    rayContact result;
    result.color = color;
    result.reflectivity = reflectivity;
    result.t = (dot(position, normal)-dot(origin, normal))/dot(ray, normal);

    result.t = result.t < 0.0 || result.t == NAN || result.t == INFINITY ? NO_INTERSECTION : result.t;

    result.position = origin + ray*result.t;

    bool inBackFace = dot(normal, normalize(position - origin)) < 0.f;
    result.normal = inBackFace ? normal : -normal;

    bool outOfBound = false;
    outOfBound |= min.x > result.position.x || max.x < result.position.x;
    outOfBound |= min.y > result.position.y || max.y < result.position.y;
    outOfBound |= min.z > result.position.z || max.z < result.position.z;

    result.t = outOfBound ? NO_INTERSECTION : result.t;

    return result;
}

void Triangle::setPoints(vec3 p1, vec3 p2, vec3 p3)
{
    position[0] = p1;
    position[1] = p2;
    position[2] = p3;

    normal = -normalize(cross(p2-p1, p3-p1));

    iArea = 1.0 / (0.5*length(position[0]-position[1])*length(position[0]-position[2]));

    center = mix(position[0], position[1], 0.5);
    center = mix(center, position[2], 0.5);

    minP = min(min(position[0], position[1]), position[2]);
    maxP = max(max(position[0], position[1]), position[2]);
}

void Triangle::setNormals(vec3 n1, vec3 n2, vec3 n3)
{
    normals[0] = n1;
    normals[1] = n1;
    normals[2] = n1;
}

void Triangle::setUVs(vec2 uv1, vec2 uv2, vec2 uv3)
{
    uvs[0] = uv1;
    uvs[1] = uv2;
    uvs[2] = uv3;
}

void Triangle::setColors(vec3 c1, vec3 c2, vec3 c3)
{
    colors[0] = c1;
    colors[1] = c2;
    colors[2] = c3;
    useVertexColors = true;
}

rayContact Triangle::trace(vec3 ray, vec3 origin)
{
    rayContact result;
    result.color = color;
    result.reflectivity = reflectivity;
    result.transparency = transparency;
    result.t = (dot(position[0], normal)-dot(origin, normal))/dot(ray, normal);

    result.t = result.t < 0.0 || result.t == NAN || result.t == INFINITY ? NO_INTERSECTION : result.t;

    if(result.t >= NO_INTERSECTION) return result;

    result.position = origin + ray*result.t;

    bool inBackFace = dot(normal, normalize(position[0] - origin)) < 0.f;
    // result.normal = inBackFace ? normal : -normal;
    result.inBackFace = inBackFace;

    vec3 ba = position[0]-position[1];
    vec3 ac = position[2]-position[0];
    vec3 cb = position[1]-position[2];

    vec3 ad = result.position-position[0];
    vec3 bd = result.position-position[1];
    vec3 cd = result.position-position[2];

    vec3 t1 = cross(ba, ad);
    vec3 t2 = cross(ac, cd);
    vec3 t3 = cross(cb, bd);

    float s1 = dot(t1, normal);
    float s2 = dot(t2, normal);
    float s3 = dot(t3, normal);

    result.t = s1 < 0 ? NO_INTERSECTION : result.t;
    result.t = s2 < 0 ? NO_INTERSECTION : result.t;
    result.t = s3 < 0 ? NO_INTERSECTION : result.t;

    if(result.t >= NO_INTERSECTION) return result;

    float alpha = s1*iArea; 
    float beta = s2*iArea;
    float gamma = s3*iArea;

    result.color = normalize(vec3(alpha, beta, gamma));

    if(!useVertexColors)
        result.uv = (uvs[2]*alpha + uvs[1]*beta + uvs[0]*gamma)/vec2(alpha+beta+gamma);
    else
        result.color = (colors[2]*alpha + colors[1]*beta + colors[0]*gamma)/vec3(alpha+beta+gamma);

    result.normal = (normals[2]*alpha + normals[1]*beta + normals[0]*gamma)/vec3(alpha+beta+gamma);

    traceCall ++;
    tmpTraceCall ++;

    return result;
}

vec3 Triangle::getCenter() const {return center;}
vec3 Triangle::getMin() const {return minP;}
vec3 Triangle::getMax() const {return maxP;}

void Mesh::genKDTree()
{
    int size = triangles.size();
    kdTreeMaxDepth = log2((float)size);
    
    if(size <= 8)
    {
        useKDTree = false;
        return;
    }

    /*TMP DEBUG*/
    // kdTreeMaxDepth = 3;

    std::cout << "Number of Triangles : " << triangles.size() << "\n";

    for(Triangle &i : triangles)
    {
        tree.triangles.push_back(&i);
    }

    std::cout << "Generating KD Tree... ";
    genKDNode(tree, 0);
    std::cout << "Done !\n";
    useKDTree = true;
}

void Mesh::genKDNode(MeshKDTreeNode &node, int depth)
{
    node.splitingAttempts ++;
    int dCoord = depth%3;
    int dCoord2 = (depth+1)%3;
    int dCoord3 = (depth+2)%3;

    int size = node.triangles.size();

    std::sort(
        node.triangles.begin(),
        node.triangles.end(),
        [=, &dCoord, &dCoord2, &dCoord3]
        (const Triangle *a, const Triangle *b)
        {
            return a->getCenter()[dCoord] < b->getCenter()[dCoord];
        }
    );


    float medianCoord = node.triangles[size/2]->getCenter()[dCoord];

    node.median = medianCoord;
    node.channel = dCoord;

    if(size <= 2)
        return;

    node.frontChild = new MeshKDTreeNode;
    node.backChild = new MeshKDTreeNode;

    /* New way of building better AABB*/
    if(&node == &tree)
    {
        node.max = vec3(-NO_INTERSECTION);
        node.min = vec3(NO_INTERSECTION);
        for(auto i : node.triangles)
        {
            node.max = max(node.max, i->getMax());
            node.min = min(node.min, i->getMin());
        }
    }
    
    node.frontChild->max = node.max;
    node.frontChild->min = node.min;
    node.frontChild->min[dCoord] = node.median;

    node.backChild->max = node.max;
    node.backChild->min = node.min;
    node.backChild->max[dCoord] = node.median;

    for(auto i : node.triangles)
    {
        if(i->getMax()[dCoord] >= medianCoord)
            node.frontChild->triangles.push_back(i);
        
        if(i->getMin()[dCoord] <= medianCoord)
            node.backChild->triangles.push_back(i);
    }

    int fsize = node.frontChild->triangles.size();
    int bsize = node.backChild->triangles.size();

    if(fsize && 
       bsize && 
       (float)fsize < size*0.75 && 
       (float)bsize < size*0.75
       )
    {
        genKDNode(*node.frontChild, depth+1);
        genKDNode(*node.backChild, depth+1);
    }
    else
    {
        delete node.frontChild;
        delete node.backChild;
        node.frontChild = nullptr;
        node.backChild = nullptr;
        if(node.splitingAttempts <=2)
            genKDNode(node, depth+1);
    }

}

bool inInterval(float min, float max, float val)
{
    return val >= min && val <= max;
}

#include <deque>

const int gstackSize = 0x2000;
MeshKDTreeNode* gstack[gstackSize];

rayContact Mesh::traceKDNode(MeshKDTreeNode &firstNode, vec3 ray, vec3 origin)
{
    rayContact result;

    vec3 tmax;
    vec3 tmin;

    const vec3 inDotR = vec3(1.0)/ray;

    int front = 0;
    gstack[0] = &firstNode;
    int back = 1;

    do
    {
        MeshKDTreeNode& node = *gstack[front];
        front ++;

        tmax = (node.max-origin)*inDotR;
        tmin = (node.min-origin)*inDotR;

        tmax.x = tmax.x == NAN || tmax.x == INFINITY ? NO_INTERSECTION : tmax.x;
        tmax.y = tmax.y == NAN || tmax.y == INFINITY ? NO_INTERSECTION : tmax.y;
        tmax.z = tmax.z == NAN || tmax.z == INFINITY ? NO_INTERSECTION : tmax.z;
        tmin.x = tmin.x == NAN || tmin.x == INFINITY ? NO_INTERSECTION : tmin.x;
        tmin.y = tmin.y == NAN || tmin.y == INFINITY ? NO_INTERSECTION : tmin.y;
        tmin.z = tmin.z == NAN || tmin.z == INFINITY ? NO_INTERSECTION : tmin.z;

        mat3 pmax(
            origin + ray*tmax.x, 
            origin + ray*tmax.y, 
            origin + ray*tmax.z
        );

        mat3 pmin(
            origin + ray*tmin.x, 
            origin + ray*tmin.y, 
            origin + ray*tmin.z
        );

        tmax.x = inInterval(node.min.y, node.max.y, pmax[0].y) && inInterval(node.min.z, node.max.z, pmax[0].z) ? tmax.x : NO_INTERSECTION;
        tmax.y = inInterval(node.min.x, node.max.x, pmax[1].x) && inInterval(node.min.z, node.max.z, pmax[1].z) ? tmax.y : NO_INTERSECTION;
        tmax.z = inInterval(node.min.y, node.max.y, pmax[2].y) && inInterval(node.min.x, node.max.x, pmax[2].x) ? tmax.z : NO_INTERSECTION;

        tmin.x = inInterval(node.min.y, node.max.y, pmin[0].y) && inInterval(node.min.z, node.max.z, pmin[0].z) ? tmin.x : NO_INTERSECTION;
        tmin.y = inInterval(node.min.x, node.max.x, pmin[1].x) && inInterval(node.min.z, node.max.z, pmin[1].z) ? tmin.y : NO_INTERSECTION;
        tmin.z = inInterval(node.min.y, node.max.y, pmin[2].y) && inInterval(node.min.x, node.max.x, pmin[2].x) ? tmin.z : NO_INTERSECTION;

        vec3 minT = min(tmax, tmin);
        float tIn = min(minT.x, min(minT.y, minT.z));

        float tOut = tIn;
        tOut = tmax.x != NO_INTERSECTION && tmax.x > tOut ? tmax.x : tOut;
        tOut = tmax.y != NO_INTERSECTION && tmax.y > tOut ? tmax.y : tOut;
        tOut = tmax.z != NO_INTERSECTION && tmax.z > tOut ? tmax.z : tOut;

        tOut = tmin.x != NO_INTERSECTION && tmin.x > tOut ? tmin.x : tOut;
        tOut = tmin.y != NO_INTERSECTION && tmin.y > tOut ? tmin.y : tOut;
        tOut = tmin.z != NO_INTERSECTION && tmin.z > tOut ? tmin.z : tOut;

        if(tIn == NO_INTERSECTION || tOut < 0.f) continue;

        char c = node.channel;

        float tSplit = (node.median-origin[c])*inDotR[c];

        bool invertChilds = ray[c] > 0.f;


        if(!node.backChild || !node.frontChild)
        {
            for(Triangle* i : node.triangles)
            {
                rayContact t = i->trace(ray, origin);
                if(t.t < result.t)
                    result = t;   
            }

            continue;
        }

        /*
            Method here :
            https://people.cs.vt.edu/yongcao/teaching/csx984/fall2011/documents/Lecture10_Acceleration_structure.pdf
        */
        MeshKDTreeNode *nearChild = invertChilds ? node.backChild : node.frontChild;
        MeshKDTreeNode *farChild  = invertChilds ? node.frontChild: node.backChild;

        if(tSplit <= tIn)
        {
            gstack[back] = farChild;
            back ++;
        }
        else if(tSplit >= tOut)
        {
            gstack[back] = nearChild;
            back ++;
        }
        else
        {
            gstack[back] = nearChild;
            back ++;
            gstack[back] = farChild;
            back ++;
        }

    }
    while(front < back && back < gstackSize);

    return result;
}

rayContact Mesh::traceKDNodeQuads(MeshKDTreeNode &node, vec3 ray, vec3 origin)
{
    return rayContact();
//    if(!node.backChild || !node.frontChild)
//         return rayContact();

//     rayContact p = node.plane.trace(ray, origin);

//     vec3 color = vec3(
//         ((uint64)(&node)>>0)%64, 
//     ((uint64)(&node.frontChild)>>(node.last%8))%64, 
//     ((uint64)(node.first)>>0)%64
//     )/vec3(64);

//     p.color = color;


//     rayContact c1 = traceKDNodeQuads(*node.frontChild, ray, origin);
//     rayContact c2 = traceKDNodeQuads(*node.backChild, ray, origin);

//     p = c1.t < p.t ? c1 : p;
//     p = c2.t < p.t ? c2 : p;

//     return p;
}

rayContact Mesh::trace(vec3 ray, vec3 origin)
{
    rayContact result;
    int size = triangles.size();

    Triangle::tmpTraceCall = 0;

    if(useKDTree)
    {
        result = traceKDNode(tree, ray, origin);
    }
    else 
    {
        for(int i = 0; i < size; i++)
        {
            rayContact t = triangles[i].trace(ray, origin);
            if(t.t < result.t)
                result = t;
        }
    }

    result.color = !useVertexColors && result.t != NO_INTERSECTION ? tColor.texture(result.uv) : result.color;

    if(result.t >= NO_INTERSECTION)
        Triangle::missedTracedCall += Triangle::tmpTraceCall;
    else
        Triangle::contactTraceCall += Triangle::tmpTraceCall;

    result.reflectivity = reflectivity;
    result.transparency = transparency;

    return result;
}

void Mesh::readOBJ(std::string filePath, bool useVertexColors)
{
    this->useVertexColors = useVertexColors;

    FILE *obj = fopen(filePath.c_str(), "r");

    fseek(obj, 0, SEEK_END);
    const uint64 fsize = ftell(obj);
    fseek(obj, 0, SEEK_SET);

    if(obj == nullptr || fsize == UINT64_MAX)
    {
        std::cout << "Error loading file : "<< filePath << "\n";
        perror("\treadOBJ");
        return;
    }

    char *data = new char[fsize]{'\0'};

    fread(data, fsize, 1, obj);
    fclose(obj);

    std::vector<vec3> tempVertices;
    std::vector<vec2> tempUvs;
    std::vector<vec3> tempColors;
    std::vector<vec3> tempNormals;

    const int f_ = 0x2066;
    const int v_ = 0x2076;
    const int vt = 0x7476;
    const int vn = 0x6E76;

    char *reader = data;

    int faceCounter = 0;

    while(reader < data+fsize)
    {
        reader = strchr(reader, '\n');

        if(!reader) break;

        reader ++;

        vec3 buff;
        vec3 buff2;

        switch (((uint16*)reader)[0])
        {
        case vt :
                sscanf(reader+2, "%f %f %f", &buff.x, &buff.y, &buff.z);
                tempUvs.push_back(buff);
            break;

        case v_ :
            if(useVertexColors)
            {
                sscanf(reader+2, "%f %f %f %f %f %f", &buff.x, &buff.y, &buff.z, &buff2.x, &buff2.y, &buff2.z);
                tempVertices.push_back(buff);
                tempColors.push_back(buff2);
            }
            else
            {
                sscanf(reader+2, "%f %f %f", &buff.x, &buff.y, &buff.z);
                tempVertices.push_back(buff);                
            }            
            break;

        case vn :
            sscanf(reader+2, "%f %f %f", &buff.x, &buff.y, &buff.z);
            tempNormals.push_back(buff);
            break;

        case f_ :
            faceCounter ++;
            break;

        default:
            break;
        }
    }

    reader = data;

    unsigned int vertexIndex[3];
    unsigned int uvIndex[3];
    unsigned int normalIndex[3];

    if(!useVertexColors)
        while(reader < data+fsize)
        {
            reader = strchr(reader, '\n');

            if(!reader) break;
            reader ++;

            if(((uint16*)reader)[0] == f_)
            {
                sscanf(reader+2, "%u/%u/%u %u/%u/%u %u/%u/%u\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                Triangle newT;
                newT.setPoints(tempVertices[vertexIndex[0]-1], tempVertices[vertexIndex[1]-1], tempVertices[vertexIndex[2]-1]);
                newT.setNormals(tempNormals[normalIndex[0]-1], tempNormals[normalIndex[1]-1], tempNormals[normalIndex[2]-1]);
                newT.setUVs(tempUvs[uvIndex[0]-1], tempUvs[uvIndex[1]-1], tempUvs[uvIndex[2]-1]);
                triangles.push_back(newT);
            }

        }
    else
        while(reader < data+fsize)
        {
            reader = strchr(reader, '\n');

            if(!reader) break;
            reader ++;

            if(((uint16*)reader)[0] == f_)
            {
                sscanf(reader+2, "%u//%u %u//%u %u//%u\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );
                Triangle newT;
                newT.setPoints(tempVertices[vertexIndex[0]-1], tempVertices[vertexIndex[1]-1], tempVertices[vertexIndex[2]-1]);
                newT.setNormals(tempNormals[normalIndex[0]-1], tempNormals[normalIndex[1]-1], tempNormals[normalIndex[2]-1]);
                newT.setColors(tempColors[vertexIndex[0]-1], tempColors[vertexIndex[1]-1], tempColors[vertexIndex[2]-1]);
                triangles.push_back(newT);
            }

        }

    genKDTree();
}
