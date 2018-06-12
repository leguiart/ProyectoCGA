#ifndef _InputManager3dp_H
#define _InputManager3dp_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standar GLFW for the management of inputs codes keyboards
enum InputCodes {
	kEscape = 27,
	Space = 32,
	Left = 37,
	Up = 38,
	Right = 39,
	Down = 40,
	a = 97,
	A = 65,
	b = 98,
	B = 66,
	c = 99,
	C = 67,
	d = 100,
	D = 68,
	e = 101,
	E = 69,
	f = 102,
	F = 70,
	g = 103,
	G = 71,
	h = 104,
	H = 72,
	i = 105,
	I = 73,
	j = 106,
	J = 74,
	k = 107,
	K = 75,
	l = 108,
	L = 76,
	m = 109,
	M = 77,
	n = 110,
	N = 78,
	o = 111,
	O = 79,
	p = 112,
	P = 80,
	q = 113,
	Q = 81,
	r = 114,
	R = 82,
	s = 115,
	S = 83,
	t = 116,
	T = 84,
	u = 117,
	U = 85,
	v = 118,
	V = 86,
	w = 119,
	W = 87,
	x = 120,
	X = 88,
	y = 121,
	Y = 89,
	z = 122,
	Z = 90
};

enum MouseButtonIndex {
	LEFT = 0, RIGHT = 1, MIDDLE = 2
};

enum State {
	RELESED = 0, PRESSED = 1, REPEATED = 2
};

class InputManager3dp {
public:

	InputManager3dp(glm::vec3 cameraLookAt, int distanceFromPlayer, float pitch, float angleAroundPlayer) :
		lastMousePos(glm::ivec2(0.0)), deltax(0), deltay(0), scrollYoffset(0), 
		cameraPos(glm::vec3(0.0f, 0.0f, 3.0f)), cameraLookAt(cameraLookAt),
		yaw(0), pitch(pitch), roll(0), 
		thetaOffset(0), distanceFromPlayer(distanceFromPlayer), angleAroundPlayer(angleAroundPlayer) {
		mouseButtomState = new bool[3];
		keyState = new bool[1024];
		for (int i = 0; i < 3; i++)
			mouseButtomState[i] = false;
		for (int i = 0; i < 1024; i++)
			keyState[i] = false;
		this->cameraPosOffset = this->cameraLookAt;
		this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		this->updateModelVectors();
		rightVel = 0.0f;
		linearVel = 0.0f;
		/*
		this->cameraPosOffset.x = this->cameraLookAt.x;
		this->cameraPosOffset.z = this->cameraLookAt.z;
		*/
	}

	~InputManager3dp() {
		delete[]mouseButtomState;
		delete[]keyState;
	}

	InputCodes toApplicationKey(int key);
	State toApplicationState(int state);
	MouseButtonIndex toMouseButtonIndex(int button);

	void keyPressed(InputCodes code, float deltaTime, State state);
	void mouseMoved(float mouseX, float mouseY);
	void mouseClicked(MouseButtonIndex code, float mouseX, float mouseY,
		State state);
	void mouseScroll(float yoffset);
	void do_movement(float deltaTime);

	glm::ivec2 getLastMousePos() {
		return lastMousePos;
	}

	bool* getKeyState() {
		return keyState;
	}

	glm::vec3 getCameraPos() {
		return cameraPos;
	}

	float getPitch() const {
		return pitch;
	}

	float getRoll() const {
		return roll;
	}

	float getYaw() const {
		return yaw;
	}

	float getScrollYoffset() const {
		return scrollYoffset;
	}

	float getThetaOffset()
	{
		return thetaOffset;
	}

	glm::vec3 getLookAt() {
		return cameraLookAt;
	}

	glm::vec3 getCameraFront() {
		return glm::normalize(this->cameraLookAt - this->cameraPos);
	}

	glm::vec3 getCameraRight(){
		return glm::normalize(glm::cross(this->getCameraFront(), WorldUp));
	}

	glm::vec3 getCameraUp() {
		return glm::normalize(glm::cross(this->getCameraRight(), this->getCameraFront()));
	}

	glm::vec3 getFront() {
		return this->Front;
	}

	glm::vec3 getUp() {
		return this->Up;
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(this->cameraPos, this->cameraLookAt,
				this->getCameraUp());
	}

	void resetLinearVel() {
		this->linearVel = -this->linearVel*0.8f;
	}

	void resetRightVel() {
		this->rightVel = -this->rightVel*0.8f;
	}

	float getLinearVel() {
		return this->linearVel;
	}

	float getRightVel() {
		return this->rightVel;
	}

	bool isGenerateRay() {
		return generateRay;
	}

	void setGenerateRay(bool generateRay) {
		this->generateRay = generateRay;
	}

	bool generateRay;


protected:
	glm::ivec2 lastMousePos;
	float deltax, deltay, scrollYoffset;
	bool * mouseButtomState;
	bool * keyState;

	glm::vec3 cameraPos, cameraPosOffset, cameraLookAt, Front, Right, Up, WorldUp;
	float linearVel, rightVel;
	float yaw, pitch, roll, theta,thetaOffset, distanceFromPlayer, angleAroundPlayer, ang_vel;

private:
	void updateModelVectors() {
		glm::vec3 front;
		front.x = sin(glm::radians(this->theta)) * cos(glm::radians(this->pitch)); 
		front.y = sin(glm::radians(this->pitch));
		front.z = cos(glm::radians(this->theta)) * cos(glm::radians(this->pitch));
		this->Front = glm::normalize(front);
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
		//std::cout << "Front: " << Front.x << ", "<< Front.y << ", " << Front.z << std::endl;
		//std::cout << "Right: " << Right.x << ", " << Right.y << ", " << Right.z << std::endl;
		//std::cout << "Up: " << Up.x << ", " << Up.y << ", " << Up.z << std::endl;
	}
};

#endif
