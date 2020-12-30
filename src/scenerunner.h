#include "scene.h"
#include "geometry/glutils.h"

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
//ImGUI imports
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>
//Scenes
#include "scenedeferred.h"
#include "sceneforward.h"
#include "sceneoverlap.h"
#include "scenecombined.h"
//include last, reduces warnings
#include <GLFW/glfw3.h>
#include "scenegbuffer.h"

class SceneRunner {
private:
    GLFWwindow * window;
    int width, height;                                                          // width and height of window
	bool debug;                                                                 //Set true to enable debug messages
    Scene* currentScene;                                                        //Current Scene Running
    std::vector<std::pair<std::string, std::function<Scene* ()>>> scenes;       //Vector of pairs representing: SceneClass name and function to instantiate object

public:
    /**
    * Initializes a window by using glfw
    *
    * @param values
    *   windowTitle: title display at the top of window
    *   samples: setting for MSAA (Anti-Alising)
    */
    SceneRunner(const std::string & windowTitle, int samples = 0)
        : debug(true), currentScene(nullptr) 
    {
        // Initialize GLFW
        if( !glfwInit() ) exit( EXIT_FAILURE );

#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        if(debug)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        if(samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, samples);
        }

        width = 1000;
        height = 1000;
        window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL );
        if( ! window ) {
			std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent(window);

        // Load the OpenGL functions.
        if(!gladLoadGL()) { exit(-1); } // if function loader is  not available nothing can be done

        GLUtils::dumpGLInfo();

        //Create ImGui context
        const char* glsl_version = "#version 130";
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //Setup Platform/Renderer bindings
        ImGui::StyleColorsDark();
        //Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Initialization
        glClearColor(0.5f,0.5f,0.5f,1.0f);
#ifndef __APPLE__
		if (debug) {
			glDebugMessageCallback(GLUtils::debugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		}
#endif
    }

    /**
     * Registers all scenes that are available in the main menu and can be click by a button
     * 
     * pushes a new pair to vector scenes: (1) name of scene as displayed on button, (2) method to instantiate scene object
     *
     * @tparam
     *  T: scene class
     * @param:
     *  name: name of scene as displayed on button
     * @return void
     */
    template<typename T>
    void registerScene(const std::string& name)
    {
        std::cout << "Registering new scene: " << name << std::endl;
        scenes.push_back(std::make_pair(name, []() { return new T(); }));
    }

    /**
     * Called to start the mainLoop and make necessary settings before
     *
     * registers all available scenes
     *
     * @param
     * @return void
     */
    int run() {
        //Register all scenes for selection in ImGui
        registerScene<SceneOverlap>("Demo Overlapping");
        registerScene<SceneForward>("Forward Shading");
        registerScene<SceneDeferred>("Deferred Shading");
        registerScene<SceneCombined>("Deferred & Forward Shading with Lights");
        registerScene<SceneGBuffer>("Visualize gBuffer");

        // Enter the main loop
        mainLoop(window);

#ifndef __APPLE__
        if (debug)
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
                GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

        // Close window and terminate GLFW
        glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

private:
    /**
     * Main Loop for main menu which provies access to all registeres scenes and renders the currentScene
     *
     * mainLoop
     *      - check if window should be terminated
     *      - sets up new frame of ImGui
     *      - manages current scene and corresponding render calls
     *      - displays performance for all scenes
     * @param
     * @return void
     */
    void mainLoop(GLFWwindow* window) {
        //data that can be shared between scenes, used to hold data between destruct of old scene and construct of new scene
        int lightingComplexity = 1;
        int countObjectsSQRT = 1;

        //main loop
        while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            GLUtils::checkForOpenGLError(__FILE__, __LINE__);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //ImGui: new Frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Scene");
            //there is a running scene
            if (currentScene != nullptr)
            {
                //check if user wants to return to main menu
                if (ImGui::Button("back"))
                {
                    //before destructing the current scene, retrieve data
                    lightingComplexity = currentScene->getLightingComplexity();
                    countObjectsSQRT = currentScene->getCountObjectSQRT();
                    delete currentScene;
                    currentScene = nullptr;
                }
                //user does NOT want to return to main menu --> render current scene 
                else
                {
                    currentScene->update(float(glfwGetTime()));
                    currentScene->render();
                    currentScene->renderImGui();
                }
            }
            //there is no scene running at the moment, check all buttons for scenes
            else
            {
                checkSceneButtons(lightingComplexity, countObjectsSQRT);
            }

            //ImGui: display performance data from ImGui
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("To exit program hit ESC.");
            ImGui::End();

            // Rendering GUI
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
            int state = glfwGetKey(window, GLFW_KEY_SPACE);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }

    /**
     * Generates ImGui::Button for every scene and checks if button was clicked to construct scene
     *
     * @param:
     *  lightingComplexity: sceneForward and sceneDeferred share this data to simulate complexity of lighting shader
     *  countObjectsSQRT: sceneForward and sceneDeferred share this data to display same amound of objects
     * @return void
     */
    void checkSceneButtons(int lightingComplexity, int countObjectsSQRT)
    {

        for (auto& scene : scenes)
        {
            //Generate button for every scene and check if it was pressed
            if (ImGui::Button(scene.first.c_str()))
            {
                //if it was pressed constructs new scene and sets it up properly
                currentScene = scene.second();
                currentScene->setCountObjectsSQRT(countObjectsSQRT);
                currentScene->setLightingComplexity(lightingComplexity);
                currentScene->initScene(width, height);
                break;
            }
        }
    }
};
