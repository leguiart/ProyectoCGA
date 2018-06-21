#define _USE_MATH_DEFINES
//glew include
#include <GL/glew.h>

#include <glm/gtc/random.hpp>

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
#include "Headers/collision.h"

//OpenAL include
#include <AL/alut.h>

#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1

//Estas son para la luna, el sol y el boundingbox
Sphere sp(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Sphere sp2(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Sphere sp3(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Sphere BoundingSphere(1.0, 50, 50, MODEL_MODE::VERTEX);
Sphere  Proyectil(1.0, 30, 30, MODEL_MODE::VERTEX_LIGHT_COLOR);

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
static std::vector<Model> objModel{Model(), Model(), Model(), Model()};


int screenWidth;
int screenHeight;

GLFWwindow * window;
InputManager3dp inputManager = InputManager3dp(glm::vec3(32.0f, -1.25f, 40.0f), 5, 15, 180);
double deltaTime, elapsedTime;
GLuint VAO, VBO, EBO;

//OpenAL config
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };

ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };

ALfloat source2Pos[] = { 0.0, 0.0, -4.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };

ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];

ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch, cont = 0;
ALboolean loop;

//Atributos de los modelos estáticos
struct static_model_attribs {
	glm::vec3 pos;
	glm::vec3 scale;
	AABB aabb;
	glm::mat4 model;
};

std::vector<int> places;

static std::vector<static_model_attribs>  trees = {
	{glm::vec3(6.0f, -2.5f, 10.0f), glm::vec3(0.05f, 0.05f, 0.05f)}, {glm::vec3(-20.0f, -2.5f, 15.0f), glm::vec3(0.04f, 0.045f, 0.05f)},
	{glm::vec3(-1.5f, -2.5f, -42.5f), glm::vec3(0.05f, 0.04f, 0.05f)}, {glm::vec3(10.8f, -2.5f, 12.3f), glm::vec3(0.05f, 0.04f, 0.05f)},
	{glm::vec3(24.4f, -2.5f, 45.5f), glm::vec3(0.05f, 0.048f, 0.05f)}, {glm::vec3(-26.7f, -2.5f, 7.5f), glm::vec3(0.06f, 0.036f, 0.055f)},
	{glm::vec3(-24.3f, -2.5f, 25.5f), glm::vec3(0.05f, 0.056f, 0.05f)}, {glm::vec3(45.5f, -2.5f, -22.5f), glm::vec3(0.043f, 0.06f, 0.057f)},
	{glm::vec3(-90.7f, -2.5f, 65.5f), glm::vec3(0.05f, 0.045f, 0.05f)}, { glm::vec3(33.3f, -2.5f, 11.5f), glm::vec3(0.06f, 0.055f, 0.05f)}
};

static std::vector<static_model_attribs> rocks = {
	{glm::vec3(66.0f, -2.3f, 12.0f), glm::vec3(0.4f, 0.4f, 0.5f)}, {glm::vec3(8.0f, -2.3f, -5.75f), glm::vec3(0.55f, 0.58f, 0.5f)},
	{glm::vec3(4.0f, -2.3f, 32.4f), glm::vec3(0.5f, 0.5f, 0.5f)}, {glm::vec3(15.0f, -2.3f, 4.65f), glm::vec3(0.5f, 0.5f, 0.5f)},
	{glm::vec3(90.0f, -2.3f, 43.6f), glm::vec3(0.4f, 0.35f, 0.6f)}, {glm::vec3(67.0f, -2.3f, 50.0f), glm::vec3(0.3f, 0.56f, 0.4f)},
	{glm::vec3(40.0f, -2.3f, -21.0f), glm::vec3(0.35f, 0.5f, 0.55f)}, {glm::vec3(50.0f, -2.3f, 35.0f), glm::vec3(0.44f, 0.45f, 0.34f)},
	{glm::vec3(80.0f, -2.3f, -32.0f), glm::vec3(0.2f, 0.5f, 0.3f)}, {glm::vec3(35.0f, -2.3f, 88.0f), glm::vec3(0.5f, 0.6f, 0.32f)}
};

std::vector<Enemy> enemies = {
{ glm::vec3(-6.0f, -2.75f, -3.0f), glm::vec3(80.0f, 1.5f, 80.0f) , glm::vec3(0.4f, 1.5f, .4f),  glm::vec3(40.0f, 1.5f, 40.0f)},{ glm::vec3(8.0f, -2.3f, -5.75f), glm::vec3(180.0f, 2.5f, 50.0f) , glm::vec3(5.4f, 1.5f, 5.4f),  glm::vec3(60.0f, 3.5f, 20.0f) },
{ glm::vec3(4.0f, 2.5f, 32.4f), glm::vec3(150.0f, 1.5f,160.0f) , glm::vec3(60.4f, 1.5f, 80.4f),  glm::vec3(20.0f, 2.5f, 10.0f) }, { glm::vec3(15.0f, -1.3f, 4.65f), glm::vec3(30.0f, 3.5f, 30.0f) , glm::vec3(0.4f, 1.5f, .4f),  glm::vec3(20.0f, 1.5f, 20.0f) },
{ glm::vec3(90.0f, 2.6f, 43.6f), glm::vec3(20.0f, 1.5f, 80.0f) , glm::vec3(20.4f, 1.5f, 10.4f),  glm::vec3(20.0f, 4.5f, 70.0f) },{ glm::vec3(67.0f, -1.3f, 50.0f), glm::vec3(20.0f, 1.5f, 20.0f) , glm::vec3(20.4f, 6.5f, 20.4f),  glm::vec3(60.0f, 1.0f, 40.0f) },
{ glm::vec3(40.0f, 1.3f, -21.0f), glm::vec3(30.0f, 1.5f, 80.0f) , glm::vec3(2.4f, 1.5f, 4.4f),  glm::vec3(10.0f, 2.5f, 20.0f) },{ glm::vec3(50.0f, -1.3f, 35.0f), glm::vec3(80.0f, 1.5f, 80.0f) , glm::vec3(0.4f, 1.5f, .4f),  glm::vec3(40.0f, 1.5f, 40.0f) },
{ glm::vec3(80.0f, 1.5f, -32.0f), glm::vec3(80.0f, 1.5f, 80.0f) , glm::vec3(3.4f, 1.5f, 4.4f),  glm::vec3(40.0f, 2.5f, 40.0f) },{ glm::vec3(35.0f, -1.3f, 88.0f), glm::vec3(80.0f, 1.5f, 80.0f) , glm::vec3(0.4f, 1.5f, .4f),  glm::vec3(40.0f, 1.5f, 40.0f)}};

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
void view_proj(Shader*, glm::vec3, glm::vec3);
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

	//BoundingSphere.init();
	//BoundingSphere.load();

	Proyectil.init();
	Proyectil.load();

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
	ch = 0;
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
	objModel[2].loadModel("../objects/rock/rock.obj");

	texture2 = new Texture(GL_TEXTURE_2D, "../Textures/tanque.jpg");
	texture2->load();
	objModel[3].loadModel("../objects/tank/TANK.obj");

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

	AABB a1, a2;
	SBB s1;
	float aux;
	a1 = getAABB(objModel[0].getMeshes());
	aux = a1.max.y;
	a1.max.y = a1.min.y;
	a1.min.y = aux;
	a2 = getAABB(objModel[2].getMeshes());
	s1 = getSBB(objModel[1].getMeshes());
	
	for (GLuint i = 0; i < trees.size(); i++) {
		trees[i].model = glm::mat4();
		trees[i].model = glm::translate(trees[i].model, trees[i].pos);
		trees[i].model = glm::rotate(trees[i].model, -(float)M_PI*0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		trees[i].model = glm::scale(trees[i].model, trees[i].scale);
		trees[i].aabb.max = glm::vec3(trees[i].model * glm::vec4(a1.max, 1.0f));
		trees[i].aabb.min = glm::vec3(trees[i].model * glm::vec4(a1.min, 1.0f)); 
		std::cout << "Tree" << i << " Max: " << trees[i].aabb.max.x << ", " << trees[i].aabb.max.y << ", " << trees[i].aabb.max.z << std::endl;
		std::cout << "Tree" << i << " Min: " << trees[i].aabb.min.x << ", " << trees[i].aabb.min.y << ", " << trees[i].aabb.min.z << std::endl;
	}

	for (GLuint i = 0; i < rocks.size(); i++) {
		rocks[i].model = glm::mat4();
		rocks[i].model = glm::translate(rocks[i].model, rocks[i].pos);
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		rocks[i].model = glm::scale(rocks[i].model, rocks[i].scale);
		rocks[i].aabb.max = glm::vec3(rocks[i].model * glm::vec4(a2.max, 1.0f));
		rocks[i].aabb.min = glm::vec3(rocks[i].model * glm::vec4(a2.min, 1.0f));
		std::cout << "Rock" << i << " Max: " << rocks[i].aabb.max.x << ", " << rocks[i].aabb.max.y << ", " << rocks[i].aabb.max.z << std::endl;
		std::cout << "Rock" << i << " Min: " << rocks[i].aabb.min.x << ", " << rocks[i].aabb.min.y << ", " << rocks[i].aabb.min.z << std::endl;
	}

	for (GLuint i = 0; i < enemies.size(); i++)
	{
		enemies[i].boundingSphere = s1;
	}

	// OpenAL init
	alutInit(0, NULL);

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	alGetError(); // clear any error messages

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}

	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);

	buffer[0] = alutCreateBufferFromFile("../sounds/Robot Fire Fire.wav");
	//buffer[0] = alutCreateBufferHelloWorld();

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}

	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 1.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 1200);

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
	SBB sbbTanque = getSBB(objModel[3].getMeshes());
	SBB s1 = getSBB(objModel[1].getMeshes());

	while (psi) {
		psi = processInput(true);
		// This is new, need clear depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		elapsedTime += deltaTime;
		lightPos = glm::vec3(0.4f*sin(0.1f*elapsedTime) - 0.4f*sin(elapsedTime), 50.0f*glm::cos(0.1f*elapsedTime), 100.0f*sin(0.1f * elapsedTime));
		lightPos2 = glm::vec3(0.4f*sin(0.1f*elapsedTime) - 0.4f*sin(elapsedTime), 50.0f*glm::cos(0.1f*elapsedTime + M_PI), 100.0f*sin(0.1f * elapsedTime));

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
		modelLoc = lightingShader->getUniformLocation("model");

		// Draw Trees
		for (GLuint i = 0; i < trees.size(); i++) {
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(trees[i].model));
			objModel[0].render(lightingShader);
		}
		/*
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-6.0f + 80.0f*glm::cos(1.5f*elapsedTime) - 80.0f*glm::cos(elapsedTime), -2.75f + 0.4f*sin(1.5f*elapsedTime) - 0.4f*sin(elapsedTime), -3.0f + 40.0f*sin(1.5f * elapsedTime) - 40.0f*sin(elapsedTime)));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		objModel[1].render(lightingShader);
		*/

		for (GLuint i = 0; i < enemies.size(); i++)
		{
			enemies[i].model = glm::mat4();
			enemies[i].model = glm::scale(enemies[i].model, glm::vec3(0.2f, 0.2f, 0.2f));
			enemies[i].model = glm::translate(enemies[i].model, glm::vec3(enemies[i].pos.x + enemies[i].movx.x*glm::cos(enemies[i].movx.y*elapsedTime) - enemies[i].movx.z*glm::cos(elapsedTime), enemies[i].pos.y + enemies[i].movy.x*sin(enemies[i].movy.y*elapsedTime) - enemies[i].movy.z*sin(elapsedTime), enemies[i].pos.z + enemies[i].movz.x*sin(enemies[i].movz.y * elapsedTime) - enemies[i].movz.z*sin(elapsedTime)));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(enemies[i].model));
			objModel[1].render(lightingShader);
			enemies[i].model = glm::translate(enemies[i].model,
				glm::vec3(enemies[i].boundingSphere.center.x, enemies[i].boundingSphere.center.y, enemies[i].boundingSphere.center.z));
			enemies[i].model = glm::scale(enemies[i].model,
				glm::vec3(enemies[i].boundingSphere.ratio, enemies[i].boundingSphere.ratio, enemies[i].boundingSphere.ratio));
		}
		
		// Draw rocks
		for (GLuint i = 0; i < rocks.size(); i++) {
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rocks[i].model));
			objModel[2].render(lightingShader);
		}

		texture2->bind(GL_TEXTURE0);
		glUniform1i(lightingShader->getUniformLocation("textura1"), 0);
		model = glm::mat4();
		model = glm::translate(model, inputManager.getLookAt()*2.0f)*glm::rotate(model, -glm::radians(inputManager.getYaw() + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		model = glm::rotate(model, (float)90.0*glm::pi<float>() / 180.0f, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		objModel[3].render(lightingShader);
		glm::mat4 model1 = model;
	
		lightingShader->turnOff();

		//Render SBB
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
		modelLoc = lampShader.getUniformLocation("model");
		GLint viewLoc = lampShader.getUniformLocation("view");
		GLint projLoc = lampShader.getUniformLocation("projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model1 = glm::translate(model1,
			glm::vec3(sbbTanque.center.x, sbbTanque.center.y, sbbTanque.center.z));
		model1 = glm::scale(model1,
			glm::vec3(sbbTanque.ratio, sbbTanque.ratio, sbbTanque.ratio));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));

		//BoundingSphere.render();
		lampShader.turnOff();

		lampShader.turnOn();
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		for (int i = 0; i < inputManager.municion.size(); i++) {
			inputManager.municion[i].model = glm::mat4();
			inputManager.municion[i].model = glm::translate(inputManager.municion[i].model, inputManager.municion[i].boundingSphere.center);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(inputManager.municion[i].model));
			Proyectil.render();
			//inputManager.municion[i].sphere.render();
			//std::cout << inputManager.municion[i].lifeSpan << std::endl;
			//std::cout << inputManager.municion[i].boundingSphere.center.x << ", " << inputManager.municion[i].boundingSphere.center.y << ", " << inputManager.municion[i].boundingSphere.center.z << std::endl;
		}
		lampShader.turnOff();

		// Render Light Sphere models
		lampShader.turnOn();

		// Create transformations
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		texture1->bind(GL_TEXTURE0);
		glUniform1i(lampShader.getUniformLocation("textura1"), 0);
		model = glm::mat4();
		model = glm::translate(glm::mat4(), lightPos);
		model = glm::scale(model, glm::vec3(1.5, 1.5, 1.5));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		texture3->bind(GL_TEXTURE0);
		glUniform1i(lampShader.getUniformLocation("textura1"), 0);
		model = glm::mat4();
		model = glm::translate(glm::mat4(), lightPos2);
		model = glm::scale(model, glm::vec3(2.0, 2.0, 2.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		sp2.render();
		lampShader.turnOff();

		// For test collision sphere vs sphere
		lampShader.turnOn();
		/*
		// Render the rays
		lampShader.turnOn();
		if (inputManager.isGenerateRay()) {
			std::cout << "Generate ray:" << std::endl;
			glm::vec4 viewport = glm::vec4(0.0f, 0.0f, screenWidth, screenHeight);
			glm::vec3 o =
				glm::unProject(
					glm::vec3(
						inputManager.getLastMousePos().x,
						screenHeight
						- inputManager.getLastMousePos().y,
						0.0f), view, projection,
					viewport);
			glm::vec3 t =
				glm::unProject(
					glm::vec3(
						inputManager.getLastMousePos().x,
						screenHeight
						- inputManager.getLastMousePos().y,
						1.0f), view, projection,
					viewport);

			glm::vec3 c1 = glm::vec3(model2 * glm::vec4(0, 0, 0, 1));
			glm::vec3 c2 = glm::vec3(model3 * glm::vec4(0, 0, 0, 1));
			glm::vec3 dir = glm::normalize(t - o);
			float t1;

			if (raySphereIntersect(o, t, dir, c1, sbb3.ratio * 0.2f, t1))
				std::cout << "Picking ufo" << std::endl;

			if (raySphereIntersect(o, t, dir, c2, sbb2.ratio * 0.8f, t1))
				std::cout << "Picking huevos" << std::endl;

			inputManager.setGenerateRay(false);

			GLuint VAO;

			VertexColor vertex[2] = { { o, glm::vec3(0.0) },
			{ t, glm::vec3(0.0) } };
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex,
				GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
				(GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			//		rays.push_back(VAO);
		}
		*/

		SBB s1, s2, s3;
		AABB a;
		s1.center = glm::vec3(model1 * glm::vec4(0, 0, 0, 1));
		s1.ratio = sbbTanque.ratio*0.005f;
		
		//std::cout << "Centro esfera: " << s1.center.x <<", " << s1.center.y << ", " << s1.center.z << std::endl;
		
		//std::cout << "Centro esfera: " << sbbTanque.center.x << ", " << sbbTanque.center.y << ", " << sbbTanque.center.z << std::endl;
		/*
		std::cout << "Radio: " << sbbTanque.ratio << std::endl;
		std::cout << "Radio: " << s1.ratio << std::endl;
		std::cout << "Modelo: " << inputManager.getLookAt().x << ", " << inputManager.getLookAt().y << ", " << inputManager.getLookAt().z << std::endl;
		*/

		for (int i = 0; i < trees.size(); i++)
		{
			//trees[i].aabb.max = glm::vec3(trees[i].model * glm::vec4(trees[i].aabb.max, 1.0f));
			//trees[i].aabb.min = glm::vec3(trees[i].model * glm::vec4(trees[i].aabb.min, 1.0f));
			//std::cout << "Tree"<< i << " Max: " << trees[i].aabb.max.x << ", " << trees[i].aabb.max.y << ", " << trees[i].aabb.max.z << std::endl;
			//std::cout << "Tree" << i << " Min: " << trees[i].aabb.min.x << ", " << trees[i].aabb.min.y << ", " << trees[i].aabb.min.z << std::endl;
			if (testSphereBoxIntersection(s1, trees[i].aabb))
			{
				std::cout << "Tree collision:" << std::endl;
				inputManager.resetLinearVel();
				inputManager.resetRightVel();
			}
				
		}

		for (int i = 0; i < rocks.size(); i++)
		{
			//rocks[i].aabb.max = glm::vec3(rocks[i].model * glm::vec4(rocks[i].aabb.max, 1.0f));
			//rocks[i].aabb.min = glm::vec3(rocks[i].model * glm::vec4(rocks[i].aabb.min, 1.0f));
			//std::cout << "Rock" << i << " Max: " << rocks[i].aabb.max.x << ", " << rocks[i].aabb.max.y << ", " << rocks[i].aabb.max.z << std::endl;
			//std::cout << "Rock" << i << " Min: " << rocks[i].aabb.min.x << ", " << rocks[i].aabb.min.y << ", " << rocks[i].aabb.min.z << std::endl;
			if (testSphereBoxIntersection(s1, rocks[i].aabb))
			{
				std::cout << "Rock collision:" << std::endl;
				inputManager.resetLinearVel();
				inputManager.resetRightVel();
			}
				
		}

		for (int i = 0; i < enemies.size(); i++) {
			s2.center = glm::vec3(enemies[i].model * glm::vec4(0, 0, 0, 1));
			s2.ratio = enemies[i].boundingSphere.ratio*0.2f;
			for (int j = 0; j < inputManager.municion.size(); j++) {
				s3.center = glm::vec3(inputManager.municion[j].model * glm::vec4(0, 0, 0, 1));
				s3.ratio = inputManager.municion[j].boundingSphere.ratio;
				if (testSphereSphereIntersection(s2, s3)) {
					inputManager.municion[j].collision = true;
					places.push_back(i);
				}
			}
		}

		for (int i = 0; i < places.size(); i++) {
			if(places[i]<enemies.size())
				enemies.erase(enemies.begin() + places[i]);
		}

		places.clear();

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

		source0Pos[0] = model[3].x;
		source0Pos[1] = model[3].y;
		source0Pos[2] = model[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);

		listenerPos[0] = inputManager.getCameraPos().x;
		listenerPos[1] = inputManager.getCameraPos().y;
		listenerPos[2] = inputManager.getCameraPos().z;
		alListenerfv(AL_POSITION, listenerPos);

		listenerOri[0] = inputManager.getFront().x;
		listenerOri[1] = inputManager.getFront().y;
		listenerOri[2] = inputManager.getFront().z;
		listenerOri[3] = inputManager.getUp().x;
		listenerOri[4] = inputManager.getUp().y;
		listenerOri[5] = inputManager.getUp().z;
		alListenerfv(AL_ORIENTATION, listenerOri);

		if (inputManager.getKeyState()[InputCodes::Space]) {
			alSourcePlay(source[0]);
			ch = 0;
		}
		else if (ch > 500) {
			alSourceRewind(source[0]);
		}
		else {
			ch++;
		}

	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
