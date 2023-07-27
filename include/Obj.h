/**************************************************
Obj is subclass class of Geometry
that loads an obj file.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "Geometry.h"
#include "Mesh.h"
#ifndef __OBJ_H__
#define __OBJ_H__

class Obj : public Geometry {
public:
    Mesh* init(const char * filename);
    void subdiv(GLuint subdivLevel, Mesh* mesh);
    // void initfinish();
};

#endif 
