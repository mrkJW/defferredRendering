#include "SceneDeferred.h"

#include <iostream>
using std::endl;
using std::cerr;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <imgui\imgui.h>
using glm::vec3;
using glm::mat4;

SceneDeferred::SceneDeferred() : angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f),
                                 torus(0.7f * 1.5f, 0.3f * 1.5f, 50,50),
                                 teapot(14, mat4(1.0)),
                                 distancePerObject(6.0f)
{}

void SceneDeferred::initScene(int w, int h)
{
    generateObjectPositions();
    width = w;
    height = h;
    compileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    float c = 1.5f;

	angle = glm::pi<float>() / 2.0f;

    // Array for quad
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays( 1, &quad );
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer( (GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer( (GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray(2);  // Texture coordinates

    glBindVertexArray(0);

    //gbuffer
    setupFBO();

    prog.setUniform("Light.L", vec3(1.0f) );

#ifdef __APPLE__
    prog.setUniform("PositionTex", 0);
    prog.setUniform("NormalTex", 1);
    prog.setUniform("ColorTex", 2);
#endif
}

void SceneDeferred::createGBufTex( GLenum texUnit, GLenum format, GLuint &texid ) {
    glActiveTexture(texUnit);
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
#ifdef __APPLE__
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#else
    glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}

void SceneDeferred::setupFBO()
{
    GLuint depthBuf, posTex, normTex, colorTex;

    // Create and bind the FBO --> everything below is attached to this framebuffer --> gBufffer
    glGenFramebuffers(1, &deferredFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);

    // The depth buffer
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // Create the textures for position, normal and color
    createGBufTex(GL_TEXTURE0, GL_RGB32F, posTex);  // Position
    createGBufTex(GL_TEXTURE1, GL_RGB32F, normTex); // Normal
    createGBufTex(GL_TEXTURE2, GL_RGB8, colorTex);  // Color

    // Attach the textures to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTex, 0);

    //in fragment shader: corresponds to output 1, 2, 3 for position, normal, texture
    GLenum drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                        GL_COLOR_ATTACHMENT2};
    glDrawBuffers(4, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneDeferred::update(float t )
{
}

void SceneDeferred::render()
{
    pass1();
    pass2();
}

void SceneDeferred::renderImGui()
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

void SceneDeferred::pass1()
{
	prog.setUniform("Pass", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    view = glm::lookAt(vec3(-10.0f, 15.0f, -10.0f), vec3(10.0f, 4.0f, 10.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 1000.0f);

    prog.setUniform("Light.Position", glm::vec4(0.0f,0.0f,0.0f,1.0f) );
    
    //loops over all positions representing the positions of an object and passes them on to vertex and pass 1
    for (auto position : positions)
    {
        //base position for model
        mat4 model_base = mat4(1.0f);
        model_base = glm::translate(model_base, position);

        //render tea pot
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

    glFinish();
}

void SceneDeferred::pass2()
{
	prog.setUniform("Pass", 2);
    prog.setUniform("lightingComplexity", lightingComplexity);

    // Revert to default framebuffer --> target is not the default framebuffer which is then projected on screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    view = mat4(1.0);
    model = mat4(1.0);
    projection = mat4(1.0);
    setMatrices();

    // Render the quad
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneDeferred::generateObjectPositions()
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

void SceneDeferred::setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    glm::mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void SceneDeferred::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
}

void SceneDeferred::compileAndLinkShader()
{
	try {
#ifdef __APPLE__
        prog.compileShader("shader/deferred_41.vs");
        prog.compileShader("shader/deferred_41.fs");
#else
        prog.compileShader("resources/shaders/SceneOverlapForwardDeferred/deferredVertexShader.vert.glsl");
        prog.compileShader("resources/shaders/SceneOverlapForwardDeferred/deferredFragmentShader.frag.glsl");
#endif
    	prog.link();
    	prog.use();
    } catch(GLSLProgramException &e ) {
    	cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}
