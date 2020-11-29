#ifndef SCENEDEFERRED_H
#define SCENEDEFERRED_H

#include "scene.h"
#include "geometry/glslprogram.h"
#include "geometry/torus.h"
#include "geometry/teapot.h"

#include <glm/glm.hpp>
#include <GLFW\glfw3.h>

class SceneDeferred : public Scene
{
private:
    GLSLProgram prog;                               //program which the shaders are being attached to

    GLuint deferredFBO;                             //ID gbuffer
    GLuint quad;

    Torus torus;
    Teapot teapot;

    std::vector<glm::vec3> positions;               //positions of one teapot and one torus

    float angle, tPrev, rotSpeed;

    float distancePerObject;                        //distance between objects

    void generateObjectPositions();                 //generates positions of objects
    void setMatrices();                             //sets matrices on shader
    void compileAndLinkShader();
    void setupFBO();                                //set up of framebuffer object --> gbuffer
    void createGBufTex(GLenum, GLenum, GLuint &);   //addes a new texture to gbuffer
    void pass1();                                   //DEFERRED: vertex and pass 1
    void pass2();                                   //DEFERRED: pass 2

public:
    SceneDeferred();

    void initScene(int w, int h);
    void update(float t);
    void render();
    void renderImGui();
    void resize(int, int);
};

#endif // SCENEDEFERRED_H
