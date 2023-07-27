#ifndef __FACE_H__
#define __FACE_H__
#include <glm/glm.hpp>
class HalfEdge;
class Point;
using namespace glm;

class Face {
public:
    HalfEdge *he;
    vec3 facePointCoord() {
        HalfEdge *he0 = he;
        HalfEdge *h = he0;
        float count = 0.0f;
        vec3 res= vec3(0.0f,0.0f,0.0f);
        do {
            // Visit h->src
            res = res + h->src->coord;
            h = h->next;
            count++;
        } while (h != he0);
        res = res / count;
        return res;
    }

    vec3 getNormal() {
        return cross(he->src->coord, he->next->src->coord);
    }
};
#endif