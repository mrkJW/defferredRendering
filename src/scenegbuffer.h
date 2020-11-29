#ifndef SceneGBuffer_H
#define SceneGBuffer_H

#include "scene.h"

#include "objectModel/model.h"
#include "objectModel/shader.h"
#include "objectModel/camera.h"

#include <glm/glm.hpp>
#include <utility>


/*
* SceneGBuffer is implemented as Forward Rendering
*/
class SceneGBuffer : public Scene
{
private:
    std::vector<glm::vec3> objectPositions;                         //positions of backpacks
    std::vector<std::pair<std::string,int>> renderTypes;            //vector of pairs, which result in buttons to adjust which texture of gBuffer is displayed: (1) name of button, (2) ID based on which in shader different output is rendered
                                                                    // IDs: 0-->Position, 1--> Normal, 2--> Albedo, 3--> Specular 

    Shader shaderGeometryPass;                                      //Shader wrapper: holds vertex shader and pass 1

    Camera camera;
    Model backpack;                                                 //Model is loaded once and then only transformed by model matric

    int currentRenderType;                                          //variable which stores what is currently rendered by shader by ID

    float deltaTime;
    float lastFrame;

    void generateObjectBackpackPositions();                         //generates positions of backpacks
    void setupRenderTypes();                                        //registers all the render types in variable renderTypes, new ones must be added in this functions and in shader (switch-case statement)
    void checkButtonsGBuffer();                                     //generates and checks buttons registered in renderTypes, if buttons was pressed then sets rederType in shader to new value
    void setCurrentRenderType(int value);                           //wrapper for set unform in shader, since before setting a uniform the shader must be bound
public:
    SceneGBuffer();

    void initScene(int w, int h);
    void update(float t);
    void render();
    void renderImGui();
    void resize(int, int);
};

#endif // SceneGBuffer_H
