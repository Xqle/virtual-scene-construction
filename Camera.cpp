#include "Camera.h"
#include <iostream>
#include <algorithm>

Camera::Camera() { 
	initCamera();
	updateCamera(); 
};
Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix()
{
	// 传入摄像头看向的方向向量front而不是at
	return this->lookAt(eye, front, up);
}

glm::mat4 Camera::getProjectionMatrix(bool isOrtho)
{
	if (isOrtho) {
		return this->ortho(-scale, scale, -scale, scale, this->zNear, this->zFar);
	}
	else {
		return this->perspective(this->fovy, this->aspect, this->zNear, this->zFar);
	}
}

glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& front, const glm::vec4& up)
{
	// use glm.
	glm::vec3 eye_3 = eye;
	glm::vec3 front_3 = front;
	glm::vec3 up_3 = up;
	
	// eye + front就是对应的at
	glm::mat4 view = glm::lookAt(eye_3, eye_3 + front_3, up_3);

	return view;
}

glm::mat4 Camera::ortho(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 / (right - left);
	c[1][1] = 2.0 / (top - bottom);
	c[2][2] = -2.0 / (zFar - zNear);
	c[3][3] = 1.0;
	c[0][3] = -(right + left) / (right - left);
	c[1][3] = -(top + bottom) / (top - bottom);
	c[2][3] = -(zFar + zNear) / (zFar - zNear);

	c = glm::transpose(c);
	return c;
}

glm::mat4 Camera::perspective(const GLfloat fovy, const GLfloat aspect,
	const GLfloat zNear, const GLfloat zFar)
{
	GLfloat top = tan(fovy * M_PI / 180 / 2) * zNear;
	GLfloat right = top * aspect;

	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = zNear / right;
	c[1][1] = zNear / top;
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;

	c = glm::transpose(c);
	return c;
}

glm::mat4 Camera::frustum(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 任意视锥体矩阵
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 * zNear / (right - left);
	c[0][2] = (right + left) / (right - left);
	c[1][1] = 2.0 * zNear / (top - bottom);
	c[1][2] = (top + bottom) / (top - bottom);
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;

	c = glm::transpose(c);
	return c;
}

void Camera::updateCamera()
{
	// 利用欧拉角更新摄像机看向的方向
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(90.0f + yaw));
	front.y = sin(glm::radians(pitch - upAngle));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(90.0f + yaw));
	front = glm::normalize(front);
}

void Camera::caldeltaTime(float currentFrame)
{
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	cameraSpeed = cameraSpeedBase * deltaTime;
}

void Camera::initCamera(){
	radius = 23.0;
	rotateAngle = 0.0;
	upAngle = 0.0;
	fovy = 45.0;
	aspect = 16.0/9.0;  //16 : 9分辨率
	scale = 1.5;
	zNear = 0.01;
	zFar = 100.0;
	eye = glm::vec4(0.0, 0.5f, radius, 1.0);
	at = glm::vec4(0.0, 0.0, 0.0, 1.0);
	up = glm::vec4(0.0, 1.0, 0.0, 0.0);
	pitch = 0.0f, yaw = -180.0f, upAngle = 0.0f;
	lastX = 800.0f, lastY = 450.0f;
	sensitivity = 0.02;
	cameraSpeedBase = 4.0f;
	cameraSpeed = 0.005;
	firstMouse = true;
}

void Camera::keyboard(GLFWwindow* window)
{
	// 键盘事件处理
	// WASD控制相机移动
	int W = glfwGetKey(window, GLFW_KEY_W), S = glfwGetKey(window, GLFW_KEY_S);
	int A = glfwGetKey(window, GLFW_KEY_A), D = glfwGetKey(window, GLFW_KEY_D);
	int SPACE = glfwGetKey(window, GLFW_KEY_SPACE);
	if (W == GLFW_PRESS || W == GLFW_REPEAT)
	{
		eye += cameraSpeed * front;
	}
	else if (S == GLFW_PRESS || S == GLFW_REPEAT)
	{
		eye -= cameraSpeed * front;
	}
	else if (A == GLFW_PRESS || A == GLFW_REPEAT)
	{
		glm::vec3 ftmp = front;
		glm::vec3 utmp = up;
		glm::vec4 delta = glm::vec4(cameraSpeed * glm::normalize(glm::cross(ftmp, utmp)), 1.0);
		eye -= delta;
	}
	else if (D == GLFW_PRESS || D == GLFW_REPEAT)
	{
		glm::vec3 ftmp = front;
		glm::vec3 utmp = up;
		glm::vec4 delta = glm::vec4(cameraSpeed * glm::normalize(glm::cross(ftmp, utmp)), 1.0);
		eye += delta;
	}
	//空气墙
	eye.x = std::min(25.0f, eye.x); 
	eye.x = std::max(-25.0f, eye.x);
	eye.y = std::max(0.01f, eye.y);
	eye.z = std::min(25.0f, eye.z); 
	eye.z = std::max(-25.0f, eye.z);
}

// 鼠标控制摄像头方向，利用欧拉角
void Camera::mouse(double xpos, double ypos)
{
	// 如果鼠标是第一次移动，直接初始化lastX和lastY
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	// 注意要反过来，因为原点在左上方；
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}