#include <iostream>

#include "Headers/InputManager3dp.h"
#include "Headers/TimeManager.h"

InputCodes InputManager3dp::toApplicationKey(int key) {
	switch (key) {
	case 256:
		return InputCodes::kEscape;
	case 87:
		return InputCodes::W;
	case 83:
		return InputCodes::S;
	case 65:
		return InputCodes::A;
	case 68:
		return InputCodes::D;
	case 265:
		return InputCodes::Up;
	case 264:
		return InputCodes::Down;
	case 263:
		return InputCodes::Left;
	case 262:
		return InputCodes::Right;
	case 69:
		return InputCodes::E;
	case 111:
		return InputCodes::e;
	case 81:
		return InputCodes::Q;
	case 113:
		return InputCodes::q;
	case 32:
		return InputCodes::Space;

	}
}

State InputManager3dp::toApplicationState(int state) {
	switch (state) {
	case 0:
		return State::RELESED;
	case 1:
		return State::PRESSED;
	}
}

MouseButtonIndex InputManager3dp::toMouseButtonIndex(int button) {
	switch (button) {
	case 0:
		return MouseButtonIndex::LEFT;
	case 1:
		return MouseButtonIndex::RIGHT;
	case 2:
		return MouseButtonIndex::MIDDLE;
	}
}

void InputManager3dp::keyPressed(InputCodes code, float deltaTime, State state) {
	keyState[code] = state;
}

void InputManager3dp::mouseMoved(float mouseX, float mouseY) {
	if (mouseButtomState[MouseButtonIndex::LEFT]
		|| mouseButtomState[MouseButtonIndex::RIGHT]) {
		deltax = mouseX - lastMousePos.x;
		deltay = mouseY - lastMousePos.y;
	}
	else {
		deltax = 0;
		deltay = 0;
	}
	lastMousePos.x = mouseX;
	lastMousePos.y = mouseY;
}

void InputManager3dp::mouseClicked(MouseButtonIndex code, float mouseX,
	float mouseY, State state) {
	mouseButtomState[code] = state;
	switch (code) {
	case RIGHT:
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		break;
	case LEFT:
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		break;
	case MIDDLE:
		break;
	}
}

void InputManager3dp::mouseScroll(float yoffset) {
	scrollYoffset = yoffset;
}

void InputManager3dp::do_movement(float deltaTime) {
	// Camera controls
	float cameraSpeed = 50.0f * deltaTime;
	// Calculate zoom
	float zoomLevel = scrollYoffset * cameraSpeed;
	distanceFromPlayer -= zoomLevel;

	// Calculate pitch
	if (mouseButtomState[MouseButtonIndex::RIGHT])
		pitch -= deltay * cameraSpeed;
	//std::cout << "pitch:" << pitch << std::endl;

	// Calculate Angle Arround
	if (mouseButtomState[MouseButtonIndex::LEFT])
		angleAroundPlayer -= deltax * cameraSpeed;
	//std::cout << "angleAroundPlayer:" << angleAroundPlayer << std::endl;

	//Calculate Horizontal distance
	float horizontalDistance = distanceFromPlayer * glm::cos(glm::radians(pitch));
	//Calculate Vertical distance
	float verticalDistance = distanceFromPlayer * glm::sin(glm::radians(pitch));

	//Calculate camera position
	theta = 0 + angleAroundPlayer + thetaOffset;
	float fr = 0.014f;
	float offsetx = horizontalDistance * glm::sin(glm::radians(theta));
	float offsetz = horizontalDistance * glm::cos(glm::radians(theta));
	cameraPos.x = cameraLookAt.x - offsetx + cameraPosOffset.x;
	cameraPos.z = cameraLookAt.z - offsetz + cameraPosOffset.z;
	cameraPos.y = cameraLookAt.y + verticalDistance;

	this->updateModelVectors();
	if (keyState[InputCodes::W] || keyState[InputCodes::w] || keyState[InputCodes::Up])
	{
		linearVel += 0.00015f;
	}
	else if (keyState[InputCodes::S] || keyState[InputCodes::s] || keyState[InputCodes::Down])
	{
		linearVel -= 0.00015f;
	}

	if (keyState[InputCodes::A] || keyState[InputCodes::a] || keyState[InputCodes::Left])
	{
		thetaOffset += cameraSpeed;
		//cameraPosOffset.x -= cameraPos.x*cameraSpeed * 0.1;
	}
	else if (keyState[InputCodes::D] || keyState[InputCodes::d] || keyState[InputCodes::Right])
	{
		thetaOffset -= cameraSpeed;
		//cameraPosOffset.x += cameraPos.x*cameraSpeed * 0.1;
	}

	if (keyState[InputCodes::Q] || keyState[InputCodes::q])
	{
		//std::cout << "move left" << std::endl;
		rightVel -= 0.00013f;
	}
	else if (keyState[InputCodes::E] || keyState[InputCodes::e])
	{
		//std::cout << "move right" << std::endl;
		rightVel += 0.00013f;
	}

	cameraLookAt.z += offsetz * linearVel;
	cameraLookAt.x += offsetx * linearVel;
	cameraPosOffset.z += offsetz * linearVel;
	cameraPosOffset.x += offsetx * linearVel;
	cameraLookAt += rightVel * Right;
	cameraPosOffset += rightVel * Right;
	linearVel = linearVel * (1.0f - fr);
	rightVel = rightVel * (1.0f - fr);
	//linearVel = linearVel * (1.0f - fr);
	//rightVel = rightVel * (1.0f - fr);
	yaw = 180 - theta;

	scrollYoffset = 0;
	deltax = 0;
	deltay = 0;

}