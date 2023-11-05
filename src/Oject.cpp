#include <Object.hpp>

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

