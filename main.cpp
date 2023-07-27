#include <stdlib.h>
#include <iostream>
// OSX systems need their own headers
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
// Use of degrees is deprecated. Use radians for GLM functions
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "Shader.h"
#include "Obj.h"
#include "Camera.h"

static const char* title = "Final Project Surface Subdivision";
static const glm::vec4 background(0.0f, .5f, 0.5f, 1.0f);
static Obj tetraObj;
static Obj cubeObj;
static Obj meshObj;
static Mesh *tetra_mesh;
static Mesh *cube_mesh;
static Mesh *mesh_obj_pt;

static Camera camera;
static const int width = 800;
static const int height = 600;

struct NormalShader : Shader {
    // modelview and projection
    glm::mat4 modelview = glm::mat4(1.0f); GLuint modelview_loc;
    glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;
    
    void initUniforms(){
        modelview_loc = glGetUniformLocation( program, "modelview" );
        projection_loc = glGetUniformLocation( program, "projection" );
    }
    void setUniforms(){
        glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &modelview[0][0]);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);
    }
};
static NormalShader shader;
static bool enable_perspective = false;

static glm::mat4 proj_default = glm::mat4(0.75f,0.0f,0.0f,0.0f,
                                          0.0f,1.0f,0.0f,0.0f,
                                          0.0f,0.0f,-0.1f,0.0f,
                                          0.0f,0.0f,0.0f,1.0f);

static int model_selection = 1; 

void printHelp(){
    std::cout << R"(
    Available commands:
      press 'h' to print this message again.
      press Esc to quit.
      press 'o' to save a screenshot.
      press the arrow keys to rotate camera.
      press 'a'/'z' to zoom. 
      press '+' to increment the subdivision level.
      press '0' to get the original.
      press 'r' to reset camera.
      press 'p' to toggle orthographic/perspective.
      press '1','2','3' to select cube, teapot, bunny.
      press Spacebar to generate images for hw2 submission.
)";
    std::cout<< "perspective: " << (enable_perspective?"on":"off") << std::endl;
}

void initialize(void){
    printHelp();
    glClearColor(background[0], background[1], background[2], background[3]); // background color
    glViewport(0,0,width,height);
    
        // Initialize geometries
    // Initialize tetrahedron
    tetra_mesh = tetraObj.init("models/tetrahedron.obj");
    tetraObj.subdiv(0, tetra_mesh);
    // Initialize cube
    cube_mesh = cubeObj.init("models/cube.obj");
    cubeObj.subdiv(0, cube_mesh);
    // Initialize mesh
    mesh_obj_pt = meshObj.init("models/ico.obj");
    meshObj.subdiv(0, mesh_obj_pt);
    // Initialize camera (set default values)
    camera.eye_default = glm::vec3(0.0f, 0.2f, 5.0f);
    camera.target_default = glm::vec3(0.0f, 0.2f, 0.0f);
    camera.up_default = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.fovy_default = 30.0f;
    camera.aspect_default = float(width) / float(height);
    camera.near_default = 0.01f;
    camera.far_default = 100.0f;
    camera.reset();

    // Initialize shader
    shader.read_source( "shaders/projective.vert", "shaders/normal.frag");
    shader.compile();
    glUseProgram(shader.program);
    shader.initUniforms();
    shader.setUniforms();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // Pre-draw sequence
    camera.computeMatrices();
    shader.modelview = camera.view;
    shader.projection = enable_perspective? camera.proj : proj_default;
    shader.setUniforms();
    // BEGIN draw
    switch(model_selection){
        case 1:
            tetraObj.draw();
            break;
        case 2:
            cubeObj.draw();
            break;
        case 3:
            meshObj.draw();
            break;
    }
    // END draw
    glutSwapBuffers();
    glFlush();
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27: // Escape to quit
            exit(0);
            break;
        case 'h': // print help
            printHelp();
            break;
        case 'o': // save screenshot
            // saveScreenShot();
            break;
        case 'r':
            camera.aspect_default = float(glutGet(GLUT_WINDOW_WIDTH))/float(glutGet(GLUT_WINDOW_HEIGHT));
            camera.reset();
            glutPostRedisplay();
            break;
        case 'a':
            camera.zoom(0.9f);
            glutPostRedisplay();
            break;
        case 'z':
            camera.zoom(1.1f);
            glutPostRedisplay();
            break;
        case 'p':
            enable_perspective = !enable_perspective; 
            std::cout<< "perspective: " << (enable_perspective?"on":"off") << std::endl;
            glutPostRedisplay();
            break;
        case '1':
            model_selection = 1;
            glutPostRedisplay();
            break;
        case '2':
            model_selection = 2;
            glutPostRedisplay();
            break;
        case '3':
            model_selection = 3;
            glutPostRedisplay();
            break;
        case '+':
            tetraObj.subdiv(1, tetra_mesh);
            cubeObj.subdiv(1, cube_mesh);
            meshObj.subdiv(1, mesh_obj_pt);
            glutPostRedisplay();
            break;
        case '0':
            tetra_mesh = tetraObj.init("models/tetrahedron.obj");
            tetraObj.subdiv(0, tetra_mesh);
            cube_mesh = cubeObj.init("models/cube.obj");
            cubeObj.subdiv(0, cube_mesh);
            mesh_obj_pt = meshObj.init("models/ico.obj");
            meshObj.subdiv(0, mesh_obj_pt);
            glutPostRedisplay();
            break;
        case ' ':
            // hw2AutoScreenshots();
            glutPostRedisplay();
            break;
        default:
            glutPostRedisplay();
            break;
    }
}
void specialKey(int key, int x, int y){
    switch (key) {
        case GLUT_KEY_UP: // up
            camera.rotateUp(-10.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: // down
            camera.rotateUp(10.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: // right
            camera.rotateRight(-10.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: // left
            camera.rotateRight(10.0f);
            glutPostRedisplay();
            break;
    }
}

int main (int argc, char** argv) {
    // BEGIN CREATE WINDOW
    glutInit(&argc, argv);

#ifdef __APPLE__
    glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
    glutInitContextVersion(3,1);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
#endif
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit() ;
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
#endif
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    // END CREATE WINDOW

    initialize();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutMainLoop();
    return 0;
}