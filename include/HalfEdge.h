#ifndef __HALF_EDGE_H__
#define __HALF_EDGE_H__
class Point;
class Face;
class HalfEdge
{
public:
    HalfEdge *next;
    HalfEdge *flip;
    Point *src;
    Face *face;
};
#endif