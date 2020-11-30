#ifndef SceneCombined_H
#define SceneCombined_H

#include "scene.h"

#include "objectModel/model.h"
#include "objectModel/shader.h"
#include "objectModel/camera.h"

#include <glm/glm.hpp>

/*
* SceneGBuffer is implemented with Deferred and Forward Rendering
*/
class SceneCombined : public Scene
{
private:
    const unsigned int NR_LIGHTS = 32;          //number of lights to be generated randomly

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    std::vector<glm::vec3> objectPositions;     //positions of backpacks

    Shader shaderGeometryPass;
    Shader shaderLightingPass;
    Shader shaderLightBox;

    bool renderLightSources;                    //value is set by user via ImGui::button and determines weather lights are rendered or not
    bool copyDepthBuffer;                       //value is set by user via ImGui::button and determines weather depth buffer between deferred and foward is copied or not

    //IDs to reference objects/textures/buffers
    unsigned int cubeVAO;
    unsigned int cubeVBO;
    unsigned int quadVAO;
    unsigned int quadVBO;
    unsigned int attachments[3];
    unsigned int rboDepth;
    unsigned int gBuffer;
    unsigned int gPosition, gNormal, gAlbedoSpec;

    Camera camera;
    Model backpack;                             //Model is loaded once and then only transformed by model matric
    float deltaTime;
    float lastFrame;

    void setupQuad();                           //in pass 2 for render to screen
    void setupCube();                           //forward: render cubes for lights
    void generateObjectBackpackPositions();     //fills objectPositions
    void generateObjectLightPositions();        //randomly generates light positions
    void setupFBO();                            //sets up the frame buffer object --> gBuffer
    void pass1(glm::mat4 projection, glm::mat4 view);                               //vertex shader and pass1 render to gbuffer
    void pass2();                               //lighting: renders from gBuffer to default framebuffer
    void forwardLightRender(glm::mat4 projection, glm::mat4 view);                  //forward rendering for lights

public:
    SceneCombined();

    void initScene(int w, int h);
    void update(float t);
    void render();
    void renderImGui();
    void resize(int, int);
};

#endif // SceneCombined_H
