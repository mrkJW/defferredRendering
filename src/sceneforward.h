#ifndef SceneForward_H
#define SceneForward_H

#include "scene.h"
#include "geometry/glslprogram.h"
#include "geometry/torus.h"
#include "geometry/teapot.h"

#include <glm/glm.hpp>
#include <GLFW\glfw3.h>

class SceneForward : public Scene
{
private:
    GLSLProgram prog;                               //program which the shaders are being attached to

    Torus torus;
    Teapot teapot;

    std::vector<glm::vec3> positions;               //positions of one teapot and one torus

    float angle, tPrev, rotSpeed;

    float distancePerObject;

    void generateObjectPositions();                 //generates positions of objects
    void setMatrices();                             //sets matrices on shader
    void compileAndLinkShader();

public:
    SceneForward();

    void initScene(int w, int h);
    void update(float t );
    void render();
    void renderImGui();
    void resize(int, int);
};

#endif // SceneForward_H
