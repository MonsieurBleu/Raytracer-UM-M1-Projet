#include <Object.hpp>
#include <cstring>

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
    float p = 0.000001;
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
    result.t = (dot(position, normal)-dot(origin, normal))/dot(ray, normal);

    result.t = result.t < 0.0 || result.t == NAN || result.t == INFINITY ? NO_INTERSECTION : result.t;

    result.position = origin + ray*result.t;
    result.normal = normal;

    result.t = min.x <= result.position.x && max.x >= result.position.x ? result.t : NO_INTERSECTION;
    result.t = min.y <= result.position.y && max.y >= result.position.y ? result.t : NO_INTERSECTION;
    result.t = min.z <= result.position.z && max.z >= result.position.z ? result.t : NO_INTERSECTION;

    return result;
}

void Triangle::setPoints(vec3 p1, vec3 p2, vec3 p3)
{
    position[0] = p1;
    position[1] = p2;
    position[2] = p3;

    normal = -normalize(cross(p2-p1, p3-p1));

    iArea = 1.0 / (0.5*length(position[0]-position[1])*length(position[0]-position[2]));
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

rayContact Triangle::trace(vec3 ray, vec3 origin)
{
    rayContact result;
    result.color = color;
    result.t = (dot(position[0], normal)-dot(origin, normal))/dot(ray, normal);

    result.t = result.t < 0.0 || result.t == NAN || result.t == INFINITY ? NO_INTERSECTION : result.t;

    result.position = origin + ray*result.t;
    result.normal = normal;

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

    float alpha = s1*iArea; 
    float beta = s2*iArea;
    float gamma = s3*iArea;

    result.color = vec3(alpha, beta, gamma);

    return result;
}

rayContact Mesh::trace(vec3 ray, vec3 origin)
{
    rayContact result;

    for(auto i : triangles)
    {
        rayContact t = i.trace(ray, color);
        if(t.t < result.t)
            result = t;
    }

    return result;
}

void Mesh::readOBJ(std::string filePath)
{
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
                sscanf(reader+2, "%f %f %f", &buff.x, &buff.y, &buff.z);
                tempVertices.push_back(buff);                
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
            // for(int i = 0; i < 3; i++)
            // {
            //     memcpy(positionWriter, (void*)&tempVertices[vertexIndex[i]-1], sizeof(vec3)); 
            //     positionWriter += sizeof(vec3);

            //     memcpy(colorWriter, (void*)&tempUvs[uvIndex[i]-1], sizeof(vec2)); 
            //     colorWriter += sizeof(vec2);

            //     memcpy(normalWriter, (void*)&tempNormals[normalIndex[i]-1], sizeof(vec3)); 
            //     normalWriter += sizeof(vec3);
            // }
        }

    }
}
