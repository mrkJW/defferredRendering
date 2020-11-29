 #pragma once

#include <glm/glm.hpp>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

class Scene
{
protected:
	glm::mat4 model, view, projection;

public:
    int width;                              //window width
    int height;                             //window height
    int lightingComplexity;                 //sceneForwardand sceneDeferred share this data to simulate complexity of lighting shader
    int countObjectsSQRT;                   //sceneForward and sceneDeferred share this data to display same amound of objects

	Scene() : m_animate(true), width(800), height(600), countObjectsSQRT(1), lightingComplexity(1) { }
	virtual ~Scene() {}

	void setDimensions( int w, int h ) {
	    width = w;
	    height = h;
	}
	
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene(int w, int h) = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update(float t) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Set up ImGui.
      */
    virtual void renderImGui() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(int, int) = 0;

    /**
      Called when scene is terminated to retrieve data
      */
    virtual int getLightingComplexity()
    {
        return lightingComplexity;
    }

    /**
      Called when scene is instantiated to set data
    */
    virtual void setLightingComplexity(int v)
    {
        lightingComplexity = v;
    }

    /**
      Called when scene is terminated to retrieve data
      */
    virtual int getCountObjectSQRT()
    {
        return countObjectsSQRT;
    }

    /**
      Called when scene is instantiated to set data
    */
    virtual void setCountObjectsSQRT(int v)
    {
        countObjectsSQRT = v;
    }
    
    void animate( bool value ) { m_animate = value; }
    bool animating() { return m_animate; }
    
protected:
	bool m_animate;
};
