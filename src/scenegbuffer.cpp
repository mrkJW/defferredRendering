#include "SceneGBuffer.h"

#include <iostream>
using std::endl;
using std::cerr;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <imgui\imgui.h>
#include <GLFW\glfw3.h>

using glm::vec3;
using glm::mat4;

SceneGBuffer::SceneGBuffer() : deltaTime(0.0f), lastFrame(0.0f),
    shaderGeometryPass(Shader("resources/shaders/SceneGBuffer/vertexShader.vert.glsl", "resources/shaders/SceneGBuffer/pass1.frag.glsl")),
    backpack("resources/objects/backpack/backpack.obj"),
    camera(glm::vec3(0.0f, 0.0f, 5.0f)),
    currentRenderType(0)                            //default is position
{}

void SceneGBuffer::initScene(int w, int h)
{
    width = w;
    height = h;
    camera.Position = glm::vec3(5.0f, 3.0f, 10.0f);

    generateObjectBackpackPositions();

    glEnable(GL_DEPTH_TEST);

    setupRenderTypes();
    setCurrentRenderType(0);

#ifdef __APPLE__
    prog.setUniform("PositionTex", 0);
    prog.setUniform("NormalTex", 1);
    prog.setUniform("ColorTex", 2);
#endif
}

void SceneGBuffer::update(float t)
{
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void SceneGBuffer::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //calculate and set matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shaderGeometryPass.use();
    shaderGeometryPass.setMat4("projection", projection);
    shaderGeometryPass.setMat4("view", view);

    //loop through all object positions and render model (backpack) accordingly
    for (unsigned int i = 0; i < objectPositions.size(); i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, objectPositions[i]);
        model = glm::scale(model, glm::vec3(0.5f));
        shaderGeometryPass.setMat4("model", model);
        backpack.Draw(shaderGeometryPass);
    }
}

void SceneGBuffer::renderImGui()
{
    //button for default camera position
    if (ImGui::Button("Default Camera Position"))
    {
        camera.Position = glm::vec3(5.0f, 3.0f, 10.0f);
    }
    //Slider for camera position
    ImGui::SliderFloat3("Camera Position", &camera.Position[0], 0.0f, 20.0f);
    //CheckBox for copy depth buffer when switching from deferred to forward rendering

    //Chosse gBuffer Component to display
    checkButtonsGBuffer();
}

void SceneGBuffer::generateObjectBackpackPositions()
{
    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
    objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
    objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));
}

void SceneGBuffer::setupRenderTypes()
{
    //these entries MUST correspond to the entries in fragment shader switch case statement!
    renderTypes.push_back(std::make_pair("Position", 0));
    renderTypes.push_back(std::make_pair("Normal", 1));
    renderTypes.push_back(std::make_pair("Albedo", 2));
    renderTypes.push_back(std::make_pair("Specular", 3));
}

void SceneGBuffer::checkButtonsGBuffer()
{
    for (auto renderType : renderTypes)
    {
        if (ImGui::Button(renderType.first.c_str()))
        {
            setCurrentRenderType(renderType.second);
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();
}

void SceneGBuffer::setCurrentRenderType(int value)
{
    shaderGeometryPass.use();
    shaderGeometryPass.setInt("renderType", value);
}

void SceneGBuffer::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
}
