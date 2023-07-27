#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#ifndef __MESH_H__
#define __MESH_H__
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <map>
#include <iostream>
#include "Point.h"
#include "HalfEdge.h"
#include "Face.h"
using namespace std;
using namespace glm;

// forward declaration

template <typename T>
bool match( int i, int j, vector<T> vect) {
    int sharecount = 0;
    for( int k = 3*i; k < 3*(i+1); k++) {
        for( int l=3*j; l < 3*(j+1); l++){
            sharecount = vect[k]==vect[l]? sharecount + 1 : sharecount;
        }
    }
    return sharecount == 2;
} 

template <typename T>
bool contains(vector<T> vec, const T &elem)
{
    bool result = false;
    if (find(vec.begin(), vec.end(), elem) != vec.end())
    {
        result = true;
    }
    return result;
}


class Mesh {
public:
    vector<Point *> pts;
    vector<HalfEdge *> hes;
    vector<Face *> faces;
    vector<vec3> out_vertices;
    vector<vec3> out_normals;
    vector<GLuint> out_indices;

    vector<Face*> getNeighborFaces(Face* face);


    void printCoord(HalfEdge* h);

    Point *getPoint(vec3 v);

    vector<vec3> getVertices()
    {
        return out_vertices;
    }
    vector<vec3> getNormals()
    {
        return out_normals;
    }
    vector<GLuint> getIndices() {
        return out_indices;
    }

    GLuint getIndex(vector<Point *> vec, vec3 coordinate);
 
    Mesh(vector<vec3> vertices, vector<vec3> normals, vector<GLuint> indices);
    void printMap(map<Point*, map<Point*, HalfEdge*>> map);


    //
    Mesh *Subdiv(int subdivLevel);
    void Subdivstep();
};
#endif