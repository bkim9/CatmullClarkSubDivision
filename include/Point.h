#ifndef __POINT_H__
#define __POINT_H__
#include <glm/glm.hpp>
using namespace glm;
class HalfEdge;
class Point
{
public:
    HalfEdge *he;
    vec3 coord;
};
#endif