#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <vector>
#include <string>

#pragma execution_character_set("utf-8");

#define White	glm::vec3(1.0, 1.0, 1.0)
#define Yellow	glm::vec3(1.0, 1.0, 0.0)
#define Green	glm::vec3(0.0, 1.0, 0.0)
#define Cyan	glm::vec3(0.0, 1.0, 1.0)
#define Magenta	glm::vec3(1.0, 0.0, 1.0)
#define Red		glm::vec3(1.0, 0.0, 0.0)
#define Black	glm::vec3(0.0, 0.0, 0.0)
#define Blue	glm::vec3(0.0, 0.0, 1.0)
#define Brown	glm::vec3(0.5, 0.5, 0.5)


int WIDTH = 600;
int HEIGHT = 600;

int mainWindow;

TriMesh* cube = new TriMesh();
TriMesh* chr_sword = new TriMesh();
TriMesh* land = new TriMesh();

// Luffy
TriMesh* Body = new TriMesh();
TriMesh* LeftUpperLeg = new TriMesh();
TriMesh* RightUpperLeg = new TriMesh();
TriMesh* LeftLowerLeg = new TriMesh();
TriMesh* RightLowerLeg = new TriMesh();

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter *painter = new MeshPainter();

// 这个用来回收和删除我们创建的物体对象
std::vector<TriMesh *> meshList;


// 矩阵栈
class MatrixStack {
	int		_index;
	int		_size;
	glm::mat4* _matrices;

public:
	MatrixStack(int numMatrices = 100) :_index(0), _size(numMatrices)
	{
		_matrices = new glm::mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]_matrices;
	}

	void push(const glm::mat4& m) {
		assert(_index + 1 < _size);
		_matrices[_index++] = m;
	}

	glm::mat4& pop() {
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};

// 关节角
enum{
    Base      = 0,
    LowerArm  = 1,
    UpperArm  = 2,
    NumAngles = 3
};
int     Axis = Base;
GLfloat Theta[NumAngles] = {0.0};
GLfloat myTheta[3] = { 0.0 };
glm::vec3 Scale[3] = { glm::vec3(0.0, 0.0, 0.0) };

// 菜单选项值
const int Quit = 4;


// 尺寸参数
const GLfloat BASE_HEIGHT      = 0.2;
const GLfloat BASE_WIDTH       = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 0.3;
const GLfloat UPPER_ARM_WIDTH  = 0.2;
const GLfloat LOWER_ARM_HEIGHT = 0.4;
const GLfloat LOWER_ARM_WIDTH  = 0.1;

// 绘制底座
void base(glm::mat4 modelView)
{
    // 按长宽高缩放正方体，平移至合适位置
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, BASE_HEIGHT / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH));

	// 绘制，由于我们只有一个立方体数据，所以这里可以直接指定绘制painter中存储的第0个立方体
	painter->drawMesh(0, modelView * instance, light, camera, 1);
	
}


// 绘制大臂
void upper_arm(glm::mat4 modelView)
{
	// @TODO: 参考底座的绘制，在此添加代码绘制大臂
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, (BASE_HEIGHT + UPPER_ARM_HEIGHT) / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));
	// 绘制，由于我们只有一个立方体数据，所以这里可以直接指定绘制painter中存储的第0个立方体
	painter->drawMesh(0, modelView * instance, light, camera, 1);

}

// 绘制小臂
void lower_arm(glm::mat4 modelView)
{
	// @TODO: 参考底座的绘制，在此添加代码绘制小臂
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, (BASE_HEIGHT + UPPER_ARM_HEIGHT + LOWER_ARM_HEIGHT) / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

	// 绘制，由于我们只有一个立方体数据，所以这里可以直接指定绘制painter中存储的第0个立方体
	painter->drawMesh(0, modelView * instance, light, camera, 1);
}

void drawMonu9(glm::mat4 modelView)
{
	painter->drawMesh(1, modelView, light, camera, 1);
}
void init()
{
	std::string vshader, cfshader, tfshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	cfshader = "shaders/cfshader.glsl";
	tfshader = "shaders/tfshader.glsl";

	// 设置光源位置  --  0
	light->readObj("assets/Myobj/sun/sun.obj");
	light->setTranslation(glm::vec3(5.0, 10.0, 10.0));
	light->setScale(glm::vec3(1, 1, 1));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	painter->addMesh(light, "light", "assets/Myobj/sun/sun.png", vshader, tfshader);
	meshList.push_back(light);

	//创建机器臂的基础立方体  --  1
	cube->setNormalize(false);
	cube->generateCube();
	cube->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	cube->setRotation(glm::vec3(0.0, 0.0, 0.0));
	cube->setScale(glm::vec3(1.0, 1.0, 1.0));
	// 加到painter中
	// 指定纹理与着色器，因为不用纹理图片所以就传个空字符串进去了
	painter->addMesh(cube, "Cube", "", vshader, cfshader);
	meshList.push_back(cube);

	//草地  --  2
	land->setNormalize(false);
	land->generateSquare(glm::vec3(0.78f, 0.5f, 0.4f));
	land->setTranslation(glm::vec3(0.0, -0.001, 0.0));    //加点偏移，不要跟阴影重合
	land->setRotation(glm::vec3(90.0, 0.0, 0.0));
	land->setScale(glm::vec3(50, 50, 50));
	land->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	land->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	land->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	land->setShininess(1.0); //高光系数
	painter->addMesh(land, "land", "assets/grass3.jpg", vshader, tfshader);

	// chr_sword  --  3
	chr_sword->setNormalize(true);
	chr_sword->readObj("assets/chr_sword/chr_sword.obj");
	chr_sword->setTranslation(glm::vec3(1.0, 0.0, 1.0));
	Scale[2] = glm::vec3(1, 1, 1);
	chr_sword->setScale(Scale[2]);
	painter->addMesh(chr_sword, "chr_sword", "assets/chr_sword/chr_sword.png", vshader, tfshader);
	meshList.push_back(chr_sword);

	glm::vec3 LuffyScale = glm::vec3(0.015, 0.015, 0.015);
	//body  --  4
	Body->setNormalize(false);
	Body->readObj("assets/Myobj/Luffy/Body.obj");
	Body->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	Body->setScale(LuffyScale);
	painter->addMesh(Body, "Body", "assets/Myobj/Luffy/Body.png", vshader, tfshader);
	meshList.push_back(Body);

	// LeftUpperLeg  --  5
	LeftUpperLeg->setNormalize(false);
	LeftUpperLeg->readObj("assets/Myobj/Luffy/LeftUpperLeg.obj");
	LeftUpperLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	LeftUpperLeg->setScale(LuffyScale);
	painter->addMesh(LeftUpperLeg, "LeftUpperLeg", "assets/Myobj/Luffy/LeftUpperLeg.png", vshader, tfshader);
	meshList.push_back(LeftUpperLeg);

	// RightUpperLeg  --  6
	RightUpperLeg->setNormalize(false);
	RightUpperLeg->readObj("assets/Myobj/Luffy/RightUpperLeg.obj");
	RightUpperLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	RightUpperLeg->setScale(LuffyScale);
	painter->addMesh(RightUpperLeg, "RightUpperLeg", "assets/Myobj/Luffy/RightUpperLeg.png", vshader, tfshader);
	meshList.push_back(RightUpperLeg);

	// LeftLowerLeg  --  7
	LeftLowerLeg->setNormalize(false);
	LeftLowerLeg->readObj("assets/Myobj/Luffy/LeftLowerLeg.obj");
	LeftLowerLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	LeftLowerLeg->setScale(LuffyScale);
	painter->addMesh(LeftLowerLeg, "LeftLowerLeg", "assets/Myobj/Luffy/LeftLowerLeg.png", vshader, tfshader);
	meshList.push_back(LeftLowerLeg);

	// RightLowerLeg  --  8
	RightLowerLeg->setNormalize(false);
	RightLowerLeg->readObj("assets/Myobj/Luffy/RightLowerLeg.obj");
	RightLowerLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	RightLowerLeg->setScale(LuffyScale);
	painter->addMesh(RightLowerLeg, "RightLowerLeg", "assets/Myobj/Luffy/RightLowerLeg.png", vshader, tfshader);
	meshList.push_back(RightLowerLeg);


	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display()
{
// #ifdef __APPLE__ // 解决 macOS 10.15 显示画面缩小问题
// 	glViewport(0, 0, WIDTH * 2, HEIGHT * 2);
// #endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绘制底座 
	glm::mat4 modelView = glm::mat4(1.0);
	//modelView = glm::translate(modelView, glm::vec3(0.0, 0.0, 0.0));// 稍微下移一下，让机器人整体居中在原点
	//modelView = glm::rotate(modelView, glm::radians(Theta[Base]), glm::vec3(0.0, 1.0, 0.0));// 底座旋转矩阵
	//base(modelView); // 首先绘制底座

 //   // @TODO: 在此添加代码完成整个机械手臂绘制
	//// 大臂变换矩阵
	//modelView = glm::rotate(modelView, glm::radians(Theta[UpperArm]), glm::vec3(0.0, 1.0, 0.0));// 底座旋转矩阵
	//// 绘制大臂
	//upper_arm(modelView);

	//// 小臂变换矩阵
	//modelView = glm::rotate(modelView, glm::radians(Theta[LowerArm]), glm::vec3(0.0, 1.0, 0.0));// 底座旋转矩阵
	//// 绘制小臂	
	//lower_arm(modelView);

	
	modelView = light->getModelMatrix();
	painter->drawMesh(0, modelView, light, camera, 0);

	modelView = land->getModelMatrix();
	painter->drawMesh(2, modelView, light, camera, 0);

	chr_sword->setScale(Scale[2]);
	modelView = chr_sword->getModelMatrix();
	modelView = glm::rotate(modelView, glm::radians(myTheta[2]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(3, modelView, light, camera, 1);

	MatrixStack mstack;
	modelView = Body->getModelMatrix();
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftLowerLeg->getHeight() + LeftUpperLeg->getHeight(), 0.0));
	painter->drawMesh(4, modelView, light, camera, 1);

	//画左大腿和左小腿
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(Body->getLength() * 4.97f / 18.0f, -LeftUpperLeg->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(5, modelView, light, camera, 1);

	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerLeg->getHeight(), 0.0));
	painter->drawMesh(7, modelView, light, camera, 1);

	//画右大腿和右小腿
	modelView = mstack.pop();
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(- Body->getLength() * 5.36f / 18.0f, -RightUpperLeg->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(6, modelView, light, camera, 1);

	modelView = glm::translate(modelView, glm::vec3(0.0, -RightLowerLeg->getHeight(), 0.0));
	painter->drawMesh(8, modelView, light, camera, 1);
}


void printHelp()
{
	std::cout << "================================================" << std::endl;
	std::cout << "Use mouse to controll the light position (drag)." << std::endl;
	std::cout << "================================================" << std::endl << std::endl;
	std::cout << "Use right click to open Menu." << std::endl;
	std::cout << "================================================" << std::endl << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<
		std::endl <<

		"[Part]" << std::endl <<
		"1:		Base" << std::endl <<
		"2:		LowerArm" << std::endl <<
		"3:		UpperArm" << std::endl <<
		std::endl <<

		"[Model]" << std::endl <<
		"a/A:	Increase rotate angle" << std::endl <<
		"s/S:	Decrease rotate angle" << std::endl <<

		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << std::endl;
		
}


// 键盘响应函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE: exit(EXIT_SUCCESS); break;
		case GLFW_KEY_I:
			myTheta[2] = myTheta[2] + 10.0;
			while (myTheta[2] > 360) myTheta[2] -= 360;
			break;
		//case GLFW_KEY_Q: exit(EXIT_SUCCESS); break;
		//case GLFW_KEY_1: Axis = Base; break;
		//case GLFW_KEY_2: Axis = UpperArm; break;
		//case GLFW_KEY_3: Axis = LowerArm; break;
		//// 通过按键旋转
		//case GLFW_KEY_A: 
		//	Theta[Axis] += 5.0;
		//	if (Theta[Axis] > 360.0)
		//		Theta[Axis] -= 360.0;
		//	break;
		//case GLFW_KEY_S:
		//	Theta[Axis] -= 5.0;
		//	if (Theta[Axis] < 0.0)
		//		Theta[Axis] += 360.0;
		//	break;
		}
	}
}

int asd = 0;
void process_key_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		exit(EXIT_SUCCESS);
	else camera->keyboard(window);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera->mouse(xpos, ypos);
}
void cleanData() {
	// 释放内存
	
	delete camera;
	camera = NULL;

	delete light;
	light = NULL;

	painter->cleanMeshes();

	delete painter;
	painter = NULL;
	
	for (int i=0; i<meshList.size(); i++) {
		delete meshList[i];
	}
	meshList.clear();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char **argv)
{
	// 初始化GLFW库，必须是应用程序调用的第一个GLFW函数
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(1600, 900, "2019152051_许琪乐_期末大作业", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Init mesh, shaders, buffer
	init();
	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	// 启用混合
	glEnable(GL_BLEND);
	while (!glfwWindowShouldClose(window))
	{
		//计算当前帧与上一帧时间差
		camera->caldeltaTime(glfwGetTime());
		//接受键盘输入
		process_key_input(window);

		display();

		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanData();


	return 0;
}

// 每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理。
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
