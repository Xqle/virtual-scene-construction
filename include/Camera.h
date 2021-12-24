#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Angel.h"

class Camera
{
public:
	Camera();
	~Camera();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix( bool isOrtho );

	glm::mat4 lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up);

	glm::mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);

	glm::mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar);

	glm::mat4 frustum(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);

	// 每次更改相机参数后更新一下相关的数值
	void updateCamera();
	// 初始化相机参数
	void initCamera();
	// 处理相机的鼠标键盘操作
	void keyboard(GLFWwindow* window);
	void mouse(double xpos, double ypos);
	// 计算deltaTime
	void caldeltaTime(float currentFrame);
	// 模视矩阵
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	// 相机位置参数
	float radius;
	float rotateAngle;
	float upAngle;
	glm::vec4 eye;
	glm::vec4 at;
	glm::vec4 up;
	glm::vec4 front;  //相机看向的方向

	// 投影参数
	float zNear;
	float zFar;
	// 透视投影参数
	float fovy;
	float aspect;
	// 正交投影参数
	float scale;
	// 鼠标控制视角
	float pitch;       // 两个欧拉角
	float yaw;
	float lastX, lastY;
	float sensitivity; // 鼠标灵敏度
	float cameraSpeedBase;
	float cameraSpeed; // 摄像头移动速度
	bool firstMouse;   // 用于初始化鼠标位置
	// 渲染时间，用于平衡不同机器的移动速度
	float deltaTime; // 当前帧与上一帧的时间差
	float lastFrame; // 上一帧的时间
};
#endif