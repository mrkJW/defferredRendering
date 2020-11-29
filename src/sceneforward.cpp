#include "SceneForward.h"

#include <iostream>
using std::endl;
using std::cerr;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <imgui\imgui.h>
using glm::vec3;
using glm::mat4;

SceneForward::SceneForward() : angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f),
                                 torus(0.7f * 1.5f, 0.3f * 1.5f, 50,50),
                                 teapot(14, mat4(1.0)),
                                 distancePerObject(6.0f)
{}

void SceneForward::initScene(int w, int h)
{
    generateObjectPositions();
    width = w;
    height = h;
    compileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    float c = 1.5f;

	angle = glm::pi<float>() / 2.0f;

    prog.setUniform("Light.L", vec3(1.0f) );

#ifdef __APPLE__
    prog.setUniform("PositionTex", 0);
    prog.setUniform("NormalTex", 1);
    prog.setUniform("ColorTex", 2);
#endif
}

void SceneForward::update(float t )
{
}

void SceneForward::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(-10.0f, 15.0f, -10.0f), vec3(10.0f, 4.0f, 10.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 500.0f);

    prog.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    //loops over all positions representing the positions of an object and passes them on to shaders
    for (auto position : positions)
    {
        //base position for model
        mat4 model_base = mat4(1.0f);
        model_base = glm::translate(model_base, position);

        //render teapot
        prog.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
        model = glm::translate(model_base, vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
        setMatrices();
        teapot.render();

        //render torus
        prog.setUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
        model = glm::translate(model_base, vec3(1.0f, 1.0f, 3.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        setMatrices();
        torus.render();
    }
}

void SceneForward::renderImGui()
{
    int newCountObjectsSQRT = countObjectsSQRT;
    ImGui::SliderInt("objects per edge", &newCountObjectsSQRT, 0, 20);
    if (newCountObjectsSQRT != countObjectsSQRT)
    {
        countObjectsSQRT = newCountObjectsSQRT;
        generateObjectPositions();
    }

    int newLightingComplexity = lightingComplexity;
    ImGui::SliderInt("Lighting complexity", &newLightingComplexity, 1, 2000);
    if (newLightingComplexity != lightingComplexity)
    {
        lightingComplexity = newLightingComplexity;
        prog.setUniform("lightingComplexity", lightingComplexity);
    }
}

void SceneForward::generateObjectPositions()
{
    positions.clear();
    for (int i = 0; i < countObjectsSQRT; i++)
    {
        for (int j = 0; j < countObjectsSQRT; j++)
        {
            float x = i * distancePerObject;
            float y = 1.0f;
            float z = j * distancePerObject;
            positions.push_back(vec3(x, y, z));
        }
    }
}

void SceneForward::setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    glm::mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void SceneForward::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
}

void SceneForward::compileAndLinkShader()
{
	try {
#ifdef __APPLE__
        prog.compileShader("shader/deferred_41.vs");
        prog.compileShader("shader/deferred_41.fs");
#else
        prog.compileShader("resources/shaders/SceneOverlapForwardDeferred/forwardVertexShader.vert.glsl");
        prog.compileShader("resources/shaders/SceneOverlapForwardDeferred/forwardFragmentShader.frag.glsl");
#endif
    	prog.link();
    	prog.use();
    } catch(GLSLProgramException &e ) {
    	cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}
