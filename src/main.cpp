/*
* Program to visualize forward and deferred rendering in an interactive way
* 
* @author Markus Jechow
* @sources:
*   David Wolf: OpenGL 4 Shading Language Cookbook: Build high-quality, real-time 3D graphics with OpenGL 4.6, GLSL 4.6 and C++17
*   Joey de Vries: https://learnopengl.com/.
*/

#include "scenerunner.h"
#include "GLAD/glad.h"


int main(int argc, char* argv[])
{
    SceneRunner runner("Render party", 8);
    return runner.run();
}