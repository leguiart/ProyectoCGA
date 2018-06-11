#define _USE_MATH_DEFINES
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#include <vector>

//glfw include
#include <GLFW/glfw3.h>

// program include
//#include "Headers/InputManager.h"
#include "Headers/InputManager3dp.h"
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"

// Texture include
#include "Headers/Texture.h"
#include "Headers/CubemapTexture.h"

// Camera include
#include "Headers/CameraFPS.h"

// Model include
#include "Headers/Model.h"
#include "Headers/Piso.h"
#include "Headers/Sphere.h"
//Estas son para la luna y el sol
Sphere sp(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Sphere sp2(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Sphere sp3(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);

Shader directionalLightShader;
Shader directionalLightPiso;
Shader pointLightShader;
Shader spotLightShader;
Shader * lightingShader;
Shader lampShader;
Shader cubeMapShader;
Shader envCubeShader;

Texture textureDifuse1(GL_TEXTURE_2D, "../Textures/pasto-diffuse.png");
Texture textureSpecular1(GL_TEXTURE_2D, "../Textures/pasto-specular.png");
Texture textureDifuse2(GL_TEXTURE_2D, "../Textures/index-diffuse.png");
Texture textureSpecular2(GL_TEXTURE_2D, "../Textures/index-specular.png");
CubemapTexture * cubeMaptexture1 = new CubemapTexture("../Textures/mp_bloodvalley", "blood-valley_ft.tga", "blood-valley_bk.tga", "blood-valley_up.tga", "blood-valley_dn.tga", "blood-valley_rt.tga", "blood-valley_lf.tga");
CubemapTexture * cubeMaptexture2 = new CubemapTexture("../Textures/ame_nebula", "purplenebula_ft.tga", "purplenebula_bk.tga", "purplenebula_up.tga", "purplenebula_dn.tga", "purplenebula_rt.tga", "purplenebula_lf.tga");


Texture * texture1, * texture2, *texture3;
Piso piso;
static std::vector<Model> objModel{Model(), Model(), Model(), Model(), Model()};


int screenWidth;
int screenHeight;

GLFWwindow * window;
InputManager3dp inputManager = InputManager3dp(glm::vec3(32.0f, -1.25f, 40.0f), 5, 15, 180);
double deltaTime, elapsedTime;
GLuint VAO, VBO, EBO;
// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable test depth, must be clear depth buffer bit
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//inputManager = InputManager(setCameraLookAt(glm::vec3(0.0f, 0.0f, 20.0f));
	piso = Piso::Piso(30, 30);

	textureDifuse1.load();
	textureSpecular1.load();
	textureDifuse2.load();
	textureSpecular2.load();

	texture1 = new Texture(GL_TEXTURE_2D, "../Textures/sun.jpg");
	texture1->load();
	sp.init();
	sp.load();

	cubeMaptexture1->Load();
	cubeMaptexture2->Load();

	elapsedTime = 0;
	directionalLightShader.initialize("../Shaders/typeLight.vs", "../Shaders/directionalLight.fs");
	directionalLightPiso.initialize("../Shaders/typeLightPiso.vs", "../Shaders/directionalLightPiso.fs");
	pointLightShader.initialize("../Shaders/typeLight.vs", "../Shaders/pointLight.fs");
	spotLightShader.initialize("../Shaders/typeLight.vs", "../Shaders/spotLight.fs");
	lampShader.initialize("../Shaders/lampShader.vs", "../Shaders/lampShader.fs");
	cubeMapShader.initialize("../Shaders/cubemapTexture.vs", "../Shaders/cubemapTexture.fs");
	envCubeShader.initialize("../Shaders/envRefCubemapTexture.vs", "../Shaders/envRefCubemapTexture.fs");
	lightingShader = &directionalLightShader;

	objModel[0].loadModel("../objects/tree/AL05m.3DS");
	objModel[1].loadModel("../objects/ufo/Low_poly_UFO.obj");
	objModel[2].loadModel("../objects/ufo/Low_poly_UFO.obj");


	texture2 = new Texture(GL_TEXTURE_2D, "../Textures/tanque.jpg");
	texture2->load();
	objModel[3].loadModel("../objects/tank/TANK.obj");
	
	objModel[4].loadModel("../objects/cyborg/cyborg.obj");

	piso.init();
	piso.load();

	texture3 = new Texture(GL_TEXTURE_2D, "../Textures/moon.jpg");
	texture3->load();
	sp2.init();
	sp2.load();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	sp3.init();
	sp3.load();

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();
	directionalLightShader.destroy();
	lampShader.destroy();
	cubeMapShader.destroy();
	envCubeShader.destroy();
	//piso.~Piso();
	delete (texture1);
	delete (texture2);
	delete (texture3);
	delete (cubeMaptexture1);
	delete (cubeMaptexture2);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	inputManager.keyPressed(inputManager.toApplicationKey(key), deltaTime,
		inputManager.toApplicationState(action));
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	inputManager.mouseMoved(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	inputManager.mouseClicked(inputManager.toMouseButtonIndex(button), xpos,
		ypos, inputManager.toApplicationState(state));
}

void mouseScrollCallback(GLFWwindow* window, double xoffset,
	double yoffset) {
	inputManager.mouseScroll(yoffset);
}

bool processInput(bool continueApplication) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	glfwPollEvents();
	inputManager.do_movement(deltaTime);
	return continueApplication;
}

void view_proj(Shader * lightingShader, glm::vec3 lightPos1, glm::vec3 lightPos2)
{
	lightingShader->turnOn();
	GLint viewPosLoc = lightingShader->getUniformLocation("viewPos");
	glUniform3f(viewPosLoc, inputManager.getCameraPos().x, inputManager.getCameraPos().y,
		inputManager.getCameraPos().z);

	// Set material properties
	GLint matDiffuseLoc = lightingShader->getUniformLocation(
		"material.diffuse");
	GLint matSpecularLoc = lightingShader->getUniformLocation(
		"material.specular");
	GLint matShineLoc = lightingShader->getUniformLocation(
		"material.shininess");
	glUniform1i(matDiffuseLoc, 0);
	glUniform1i(matSpecularLoc, 1);
	glUniform1f(matShineLoc, 32.0f);

	// Set lights properties
	GLint lightAmbientLoc = lightingShader->getUniformLocation(
		"light.ambient");
	GLint lightDiffuseLoc = lightingShader->getUniformLocation(
		"light.diffuse");
	GLint lightSpecularLoc = lightingShader->getUniformLocation(
		"light.specular");
	GLint lightPosLoc = lightingShader->getUniformLocation("light.position");
	if (lightPos1.y > 0.0f) {
		glUniform3f(lightAmbientLoc, 0.6f, 0.6f, 0.6f);
		glUniform3f(lightDiffuseLoc, 1.8f, 1.6f, 1.7f);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos1.x, lightPos1.y, lightPos1.z);

		//GLint lightDirLoc = lightingShader->getUniformLocation("light.direction");
		//glUniform3f(lightDirLoc, -0.3, 1.0, -0.2);
	}
	else {

		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f); // Let's darken the light a bit to fit the scene
		glUniform3f(lightSpecularLoc, 0.3f, 0.4f, 0.8f);
		glUniform3f(lightPosLoc, lightPos2.x, lightPos2.y, lightPos2.z);
		//GLint lightDirLoc = lightingShader->getUniformLocation("light.direction");
		//glUniform3f(lightDirLoc, 0.3, 0.8, 0.2);
		/* Para linterna... recordar hacer que con un botón se prenda
		glUniform3f(lightingShader->getUniformLocation("light.position"), inputManager.getLookAt().x*2.0f, inputManager.getLookAt().y*2.0f, inputManager.getLookAt().z*2.0f);
		glUniform3f(lightingShader->getUniformLocation("light.direction"), inputManager.getFront().x, inputManager.getFront().y, inputManager.getFront().z);
		glUniform1f(lightingShader->getUniformLocation("light.cutOff"), glm::cos(glm::radians(22.5f)));
		glUniform1f(lightingShader->getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(27.5f)));

		glUniform1f(lightingShader->getUniformLocation("light.constant"), 1.0f);
		glUniform1f(lightingShader->getUniformLocation("light.linear"), 0.35f);
		glUniform1f(lightingShader->getUniformLocation("light.quadratics"), 0.44f);
		*/
	}

	glm::mat4 view;
	view = glm::rotate(view,
		glm::radians(inputManager.getPitch()),
		glm::vec3(1, 0, 0));
	view = glm::rotate(view,
		glm::radians(inputManager.getYaw()),
		glm::vec3(0, 1, 0));
	glm::vec3 cameraPos = inputManager.getCameraPos();
	view = glm::translate(view,
		glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z));

	glm::mat4 projection;
	projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 500.0f);

	// Get the uniform locations
	GLint modelLoc = lightingShader->getUniformLocation("model");
	GLint viewLoc = lightingShader->getUniformLocation("view");
	GLint projLoc = lightingShader->getUniformLocation("projection");
	// Pass the matrices to the shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void applicationLoop() {
	bool psi = true;
	glm::vec3 lightPos;
	glm::vec3 lightPos2;
	double lastTime = TimeManager::Instance().GetTime();

	glm::vec3 treePositions[] =
	{ glm::vec3(6.0f, -2.5f, 10.0f), glm::vec3(-20.0f, -2.5f, 15.0f),
		glm::vec3(-1.5f, -2.5f, -42.5f), glm::vec3(10.8f, -2.5f, 12.3f),
		glm::vec3(24.4f, -2.5f, 45.5f), glm::vec3(-26.7f, -2.5f, 7.5f),
		glm::vec3(-24.3f, -2.5f, 25.5f), glm::vec3(45.5f, -2.5f, -22.5f),
		glm::vec3(-90.7f, -2.5f, 65.5f), glm::vec3(33.3f, -2.5f, 11.5f) };

	while (psi) {
		psi = processInput(true);
		// This is new, need clear depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		elapsedTime += deltaTime;
		lightPos = glm::vec3(0.4f*sin(0.1f*elapsedTime) - 0.4f*sin(elapsedTime), 100.0f*glm::cos(0.1f*elapsedTime), 100.0f*sin(0.1f * elapsedTime));
		lightPos2 = glm::vec3(0.4f*sin(0.1f*elapsedTime) - 0.4f*sin(elapsedTime), 100.0f*glm::cos(0.1f*elapsedTime + M_PI), 100.0f*sin(0.1f * elapsedTime));

		/* Algo con la linterna
		if(lightPos.y>0)
			lightingShader = &directionalLightShader;
		else 
			lightingShader = &spotLightShader;
		*/
		lightingShader = &directionalLightPiso;
		lightingShader->turnOn();
		view_proj(lightingShader, lightPos, lightPos2);
		// Get the uniform locations
		GLint modelLoc = lightingShader->getUniformLocation("model");

		glm::mat4 model;
		model = glm::translate(glm::vec3(-100.0f, 0.0f, -100.0f))*glm::scale(glm::vec3(200.0f, 5.0f, 200.0f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		textureDifuse2.bind(GL_TEXTURE0);
		textureSpecular2.bind(GL_TEXTURE1);
		int diffuseMapLoc = lightingShader->getUniformLocation("diffuse");
		int specularMapLoc = lightingShader->getUniformLocation("specular");

		glUniform1i(diffuseMapLoc, 0);
		glUniform1i(specularMapLoc, 1);

		piso.render();
		lightingShader->turnOff();

		lightingShader = &directionalLightShader;
		lightingShader->turnOn();
		view_proj(lightingShader, lightPos, lightPos2);
		/*
		model = glm::mat4();
		//model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		model = glm::translate(model, glm::vec3(5.0f, -2.5f, -100.0f));
		model = glm::rotate(model, -(float)M_PI*0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		objModel[0].render(lightingShader);
		*/
		modelLoc = lightingShader->getUniformLocation("model");
		// Draw Trees
		for (GLuint i = 0; i < 10; i++) {
			model = glm::mat4();
			model = glm::translate(model, treePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, -(float)M_PI*0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			objModel[0].render(lightingShader);
		}
		
		/*
		// Draw a sphere
		for (GLuint i = 0; i < 10; i++) {
			glm::mat4 model;
			model = glm::translate(model, treePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			textureDifuse2.bind(GL_TEXTURE0);
			textureSpecular2.bind(GL_TEXTURE1);
			int diffuseMapLoc = lightingShader->getUniformLocation("diffuse");
			int specularMapLoc = lightingShader->getUniformLocation("specular");

			glUniform1i(diffuseMapLoc, 0);
			glUniform1i(specularMapLoc, 1);

			sp2.render();
		}
		*/
		model = glm::mat4();
		//model = glm::translate(model, glm::vec3(-6.0f + 60.0f*glm::sin(lastTime*10.5f + deltaTime) - glm::sin(lastTime*0.5), -2.75f, -3.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-6.0f + 80.0f*glm::cos(1.5f*elapsedTime) - 80.0f*glm::cos(elapsedTime), -2.75f + 0.4f*sin(1.5f*elapsedTime) - 0.4f*sin(elapsedTime), -3.0f + 40.0f*sin(1.5f * elapsedTime) - 40.0f*sin(elapsedTime)));
		//std::cout << glm::sin(elapsedTime) << std::endl;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		objModel[1].render(lightingShader);

		texture2->bind(GL_TEXTURE0);
		glUniform1i(lightingShader->getUniformLocation("textura1"), 0);
		model = glm::mat4();
		model = glm::translate(model, inputManager.getLookAt()*2.0f)*glm::rotate(model, -glm::radians(inputManager.getYaw() + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		model = glm::rotate(model, (float)90.0*glm::pi<float>() / 180.0f, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		objModel[3].render(lightingShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(12.0f, -2.5f, -10.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		objModel[4].render(lightingShader);
	
		lightingShader->turnOff();

		lampShader.turnOn();
		glm::mat4 view;
		view = glm::rotate(view,
			glm::radians(inputManager.getPitch()),
			glm::vec3(1, 0, 0));
		view = glm::rotate(view,
			glm::radians(inputManager.getYaw()),
			glm::vec3(0, 1, 0));
		glm::vec3 cameraPos = inputManager.getCameraPos();
		view = glm::translate(view,
			glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z));

		glm::mat4 projection;
		projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 500.0f);
		texture1->bind(GL_TEXTURE0);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glUniform1i(lampShader.getUniformLocation("textura1"), 0);
		// Create transformations
		modelLoc = lampShader.getUniformLocation("model");
		GLint viewLoc = lampShader.getUniformLocation("view");
		GLint projLoc = lampShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4();
		model = glm::translate(glm::mat4(), lightPos);
		model = glm::scale(model, glm::vec3(1.5, 1.5, 1.5));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		texture3->bind(GL_TEXTURE0);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glUniform1i(lampShader.getUniformLocation("textura1"), 0);
		model = glm::mat4();
		model = glm::translate(glm::mat4(), lightPos2);
		model = glm::scale(model, glm::vec3(2.0, 2.0, 2.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		sp2.render();
		lampShader.turnOff();

		cubeMapShader.turnOn();
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;

		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

		//view = glm::mat3(inputManager.getViewMatrix());
		glUniformMatrix4fv(cubeMapShader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(cubeMapShader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 cubeModel;
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 55.0f, 0.0f));
		cubeModel = glm::scale(cubeModel, glm::vec3(250.0f, 150.0f, 250.0f));
		glUniformMatrix4fv(cubeMapShader.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(cubeModel));

		if (lightPos.y > 0.0f)
		{
			cubeMaptexture1->Bind(GL_TEXTURE0);
			glUniform1f(cubeMapShader.getUniformLocation("skybox"), 0);
		}
		else
		{
			cubeMaptexture2->Bind(GL_TEXTURE0);
			glUniform1f(cubeMapShader.getUniformLocation("skybox"), 0);
		}

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sp3.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		cubeMapShader.turnOff();

		envCubeShader.turnOn();
		//view = inputManager.getViewMatrix();
		viewLoc = envCubeShader.getUniformLocation("view");
		projLoc = envCubeShader.getUniformLocation("projection");
		modelLoc = envCubeShader.getUniformLocation("model");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		if (lightPos.y > 0.0f)
		{
			cubeMaptexture1->Bind(GL_TEXTURE0);
			glUniform1f(envCubeShader.getUniformLocation("skybox"), 0);
		}
		else
		{
			cubeMaptexture2->Bind(GL_TEXTURE0);
			glUniform1f(envCubeShader.getUniformLocation("skybox"), 0);
		}

		glUniform3fv(envCubeShader.getUniformLocation("viewPos"), 1, glm::value_ptr(inputManager.getCameraPos()));
		envCubeShader.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
