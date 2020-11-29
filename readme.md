Notes for Project Deferred Shading / Deferred Rendering

General: Some code and especially shaders (and models like backpack) are based on the following two sources:
	*   David Wolf: OpenGL 4 Shading Language Cookbook: Build high-quality, real-time 3D graphics with OpenGL 4.6, GLSL 4.6 and C++17
	*   Joey de Vries: https://learnopengl.com/, Twitter: https://twitter.com/JoeyDeVriez., https://creativecommons.org/licenses/by/4.0/legalcode

Purpose of this project:
	Visualize forward and deferred rendering within different scenes. Comments have been added for further explanation of the code, for the each scene most comments can be found in the corresponding .h file.

Basic structure of the project:
	- src: contains source code
		- geometry: rendering of teapot and torus (David Wolf)
		- objectModel: handling of models, in this case backpack model (Joey de Vries)
		- vendors: ImGui (used to get user input), glm (math libary), stb_image (loading of textures)
	- dependencies: external libaries / headers (ASSIMP, GLAD, GLFW)
	- dlls: dynamic libaries (assimp: .dll file is copied to outDir by postprocess comand in visual studio)
	- resources: shaders and models (objects like backpack)
	
	
	The ways of implementing deferred rendering change dependind on the scene signicantly due to differences in the approaches chosen by David Wolf and Joey de Vries.
	Same structures like using pass1() and pass2() have been implemented for both.
	The class scenerunner.h by David Wolf has been 

Notes:
	[1] ImGui is used for an easy way to get and process user input
	[2] scenerunner.h manages all scenes and provides a button for each scene so the user can start a scene
	[3] scenerunner.h contains mainLoop of the programm, prints some basic information on top of every scene and issues update() and render() for currently running scene
	[4] David Wolf and Joey de Vries have partially very different approaches to implement deferred rendering, while some differences have been kept and can be seen in the code, some unification has been made, example
		- handling of shaders sceneoverlap, sceneforward, scenedeferred: direcly in the class of scene
		- handlung of shaders in scenecombined, scenefguffer: wrapper class around shaders which provides additional level of abstraction
	[5] The following lists states how the scenes are implemented
		- SceneOverlap: deferred
		- SceneForward: forward
		- SceneDeferred: deferred
		- SceneCombined: (1) deferred, (2) forward
		- SceneGBuffer: forward
	