#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <algorithm>
#include <vector>
#include <string>
#include <map>

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


int WIDTH = 1600;
int HEIGHT = 900;

int mainWindow;

TriMesh* cube = new TriMesh();
TriMesh* chr_sword = new TriMesh();
TriMesh* land = new TriMesh();

// Luffy
// ����
TriMesh* Body = new TriMesh();
// ͷ��ñ��
TriMesh* Head = new TriMesh();
TriMesh* Hat = new TriMesh();
// ���
TriMesh* LeftUpperHand = new TriMesh();
TriMesh* RightUpperHand = new TriMesh();
// С��
TriMesh* LeftLowerHand = new TriMesh();
TriMesh* RightLowerHand = new TriMesh();
// ��
TriMesh* Meat = new TriMesh();
// ����
TriMesh* LeftUpperLeg = new TriMesh();
TriMesh* RightUpperLeg = new TriMesh();
// С��
TriMesh* LeftLowerLeg = new TriMesh();
TriMesh* RightLowerLeg = new TriMesh();

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter *painter = new MeshPainter();

// ����������պ�ɾ�����Ǵ������������
std::vector<TriMesh *> meshList;


// ����ջ
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

const int NumLuffyParts = 12;
GLfloat LuffyTheta[12] = {0.0};
std::map<std::string, std::map<std::string, int>> AngleMap;
float dir = -1.0, moveSpeed, rotateSpeed;
float lastFrame;
//�����ƶ��ٶȣ���ת�ٶȵ�
void calMoveSpeed(float currentFrame)
{
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	moveSpeed = 100.0f * deltaTime;
	rotateSpeed = 50.00f * deltaTime;
}

void moveanimation()
{
	if (LuffyTheta[AngleMap["Luffy"]["LeftUpperHand"]] > 45.0 || LuffyTheta[AngleMap["Luffy"]["LeftUpperHand"]] < -45.0)
	{
		//���ưڶ�����
		dir = -dir;
		//�����ؼ�֡
		if (dir < 0)
		{
			LuffyTheta[AngleMap["Luffy"]["LeftUpperHand"]] = 45.00f;
			LuffyTheta[AngleMap["Luffy"]["RightUpperHand"]] = -45.00f;
			LuffyTheta[AngleMap["Luffy"]["LeftLowerHand"]] = 0.0f;
			LuffyTheta[AngleMap["Luffy"]["RightLowerHand"]] = -45.00f;
		}
		else
		{
			LuffyTheta[AngleMap["Luffy"]["LeftUpperHand"]] = -45.00f;
			LuffyTheta[AngleMap["Luffy"]["RightUpperHand"]] = 45.00f;
			LuffyTheta[AngleMap["Luffy"]["LeftLowerHand"]] = -45.00f;
			LuffyTheta[AngleMap["Luffy"]["RightLowerHand"]] = 0.0f;
		}
	}
	LuffyTheta[AngleMap["Luffy"]["LeftUpperHand"]] += dir * moveSpeed;
	// ����������ǰ���ʱ����С�۲ſ���
	if(LuffyTheta[AngleMap["Luffy"]["LeftUpperHand"]] < 0) 
		LuffyTheta[AngleMap["Luffy"]["LeftLowerHand"]] += dir * moveSpeed;
	LuffyTheta[AngleMap["Luffy"]["RightUpperHand"]] -= dir * moveSpeed;
	// �Ҵ��������ǰ���ʱ����С�۲ſ���
	if (LuffyTheta[AngleMap["Luffy"]["RightUpperHand"]] < 0) 
		LuffyTheta[AngleMap["Luffy"]["RightLowerHand"]] -= dir * moveSpeed;
	LuffyTheta[AngleMap["Luffy"]["RightUpperLeg"]] += dir * moveSpeed;
	LuffyTheta[AngleMap["Luffy"]["LeftUpperLeg"]] -= dir * moveSpeed;
}	

// �ؽڽ�
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

// �˵�ѡ��ֵ
const int Quit = 4;


// �ߴ����
const GLfloat BASE_HEIGHT      = 0.2;
const GLfloat BASE_WIDTH       = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 0.3;
const GLfloat UPPER_ARM_WIDTH  = 0.2;
const GLfloat LOWER_ARM_HEIGHT = 0.4;
const GLfloat LOWER_ARM_WIDTH  = 0.1;

void init()
{
	std::string vshader, cfshader, tfshader;
	// ��ȡ��ɫ����ʹ��
	vshader = "shaders/vshader.glsl";
	cfshader = "shaders/cfshader.glsl";
	tfshader = "shaders/tfshader.glsl";

	// ���ù�Դλ��  --  0
	light->readObj("assets/Myobj/sun/sun.obj");
	light->setTranslation(glm::vec3(5.0, 10.0, 10.0));
	light->setScale(glm::vec3(1, 1, 1));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // ������
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // ������
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // ���淴��
	painter->addMesh(light, "light", "assets/Myobj/sun/sun.png", vshader, tfshader);
	meshList.push_back(light);

	//���������۵Ļ���������  --  1
	cube->setNormalize(false);
	cube->generateCube();
	cube->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	cube->setRotation(glm::vec3(0.0, 0.0, 0.0));
	cube->setScale(glm::vec3(1.0, 1.0, 1.0));
	// �ӵ�painter��
	// ָ����������ɫ������Ϊ��������ͼƬ���Ծʹ������ַ�����ȥ��
	painter->addMesh(cube, "Cube", "", vshader, cfshader);
	meshList.push_back(cube);

	//�ݵ�  --  2
	land->setNormalize(false);
	land->generateSquare(glm::vec3(0.78f, 0.5f, 0.4f));
	land->setTranslation(glm::vec3(0.0, -0.001, 0.0));    //�ӵ�ƫ�ƣ���Ҫ����Ӱ�غ�
	land->setRotation(glm::vec3(90.0, 0.0, 0.0));
	land->setScale(glm::vec3(50, 50, 50));
	land->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // ������
	land->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // ������
	land->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // ���淴��
	land->setShininess(1.0); //�߹�ϵ��
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
	AngleMap["Luffy"]["Body"] = 0;

	//Head  --  5
	Head->setNormalize(false);
	Head->readObj("assets/Myobj/Luffy/Head.obj");
	Head->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	Head->setScale(LuffyScale);
	painter->addMesh(Head, "Head", "assets/Myobj/Luffy/Head.png", vshader, tfshader);
	meshList.push_back(Head);
	AngleMap["Luffy"]["Head"] = 1;

	//Hat  --  6
	Hat->setNormalize(false);
	Hat->readObj("assets/Myobj/Luffy/Hat.obj");
	Hat->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	Hat->setScale(LuffyScale);
	painter->addMesh(Hat, "Hat", "assets/Myobj/Luffy/Hat.png", vshader, tfshader);
	meshList.push_back(Hat);
	AngleMap["Luffy"]["Hat"] = 2;

	// LeftUpperHand  --  7
	LeftUpperHand->setNormalize(false);
	LeftUpperHand->readObj("assets/Myobj/Luffy/LeftUpperHand.obj");
	LeftUpperHand->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	LeftUpperHand->setScale(LuffyScale);
	painter->addMesh(LeftUpperHand, "LeftUpperHand", "assets/Myobj/Luffy/LeftUpperHand.png", vshader, tfshader);
	meshList.push_back(LeftUpperHand);
	AngleMap["Luffy"]["LeftUpperHand"] = 3;

	// LeftLowerHand  --  8
	LeftLowerHand->setNormalize(false);
	LeftLowerHand->readObj("assets/Myobj/Luffy/LeftLowerHand.obj");
	LeftLowerHand->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	LeftLowerHand->setScale(LuffyScale);
	painter->addMesh(LeftLowerHand, "LeftLowerHand", "assets/Myobj/Luffy/LeftLowerHand.png", vshader, tfshader);
	meshList.push_back(LeftLowerHand);
	AngleMap["Luffy"]["LeftLowerHand"] = 4;

	// Meat  --  9
	Meat->setNormalize(false);
	Meat->readObj("assets/Myobj/Luffy/Meat.obj");
	Meat->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	Meat->setScale(LuffyScale);
	painter->addMesh(Meat, "Meat", "assets/Myobj/Luffy/Meat.png", vshader, tfshader);
	meshList.push_back(Meat);
	AngleMap["Luffy"]["Meat"] = 5;

	// RightUpperHand  --  10
	RightUpperHand->setNormalize(false);
	RightUpperHand->readObj("assets/Myobj/Luffy/RightUpperHand.obj");
	RightUpperHand->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	RightUpperHand->setScale(LuffyScale);
	painter->addMesh(RightUpperHand, "RightUpperHand", "assets/Myobj/Luffy/RightUpperHand.png", vshader, tfshader);
	meshList.push_back(RightUpperHand);
	AngleMap["Luffy"]["RightUpperHand"] = 6;

	// RightLowerHand  --  11
	RightLowerHand->setNormalize(false);
	RightLowerHand->readObj("assets/Myobj/Luffy/RightLowerHand.obj");
	RightLowerHand->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	RightLowerHand->setScale(LuffyScale);
	painter->addMesh(RightLowerHand, "RightLowerHand", "assets/Myobj/Luffy/RightLowerHand.png", vshader, tfshader);
	meshList.push_back(RightLowerHand);
	AngleMap["Luffy"]["RightLowerHand"] = 7;

	// LeftUpperLeg  --  12
	LeftUpperLeg->setNormalize(false);
	LeftUpperLeg->readObj("assets/Myobj/Luffy/LeftUpperLeg.obj");
	LeftUpperLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	LeftUpperLeg->setScale(LuffyScale);
	painter->addMesh(LeftUpperLeg, "LeftUpperLeg", "assets/Myobj/Luffy/LeftUpperLeg.png", vshader, tfshader);
	meshList.push_back(LeftUpperLeg);
	AngleMap["Luffy"]["LeftUpperLeg"] = 8;

	// LeftLowerLeg  --  13
	LeftLowerLeg->setNormalize(false);
	LeftLowerLeg->readObj("assets/Myobj/Luffy/LeftLowerLeg.obj");
	LeftLowerLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	LeftLowerLeg->setScale(LuffyScale);
	painter->addMesh(LeftLowerLeg, "LeftLowerLeg", "assets/Myobj/Luffy/LeftLowerLeg.png", vshader, tfshader);
	meshList.push_back(LeftLowerLeg);
	AngleMap["Luffy"]["LeftLowerLeg"] = 9;

	// RightUpperLeg  --  14
	RightUpperLeg->setNormalize(false);
	RightUpperLeg->readObj("assets/Myobj/Luffy/RightUpperLeg.obj");
	RightUpperLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	RightUpperLeg->setScale(LuffyScale);
	painter->addMesh(RightUpperLeg, "RightUpperLeg", "assets/Myobj/Luffy/RightUpperLeg.png", vshader, tfshader);
	meshList.push_back(RightUpperLeg);
	AngleMap["Luffy"]["RightUpperLeg"] = 10;

	// RightLowerLeg  --  15
	RightLowerLeg->setNormalize(false);
	RightLowerLeg->readObj("assets/Myobj/Luffy/RightLowerLeg.obj");
	RightLowerLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	RightLowerLeg->setScale(LuffyScale);
	painter->addMesh(RightLowerLeg, "RightLowerLeg", "assets/Myobj/Luffy/RightLowerLeg.png", vshader, tfshader);
	meshList.push_back(RightLowerLeg);
	AngleMap["Luffy"]["RightLowerLeg"] = 11;


	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void drawLuffy()
{
	float bias = 0.5;

	// **** ������ ****
	MatrixStack mstack;
	glm::mat4 modelView = Body->getModelMatrix();
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftLowerLeg->getHeight() + LeftUpperLeg->getHeight() - bias, 0.0));
	modelView = glm::rotate(modelView, glm::radians(LuffyTheta[AngleMap["Luffy"]["Body"]]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(4, modelView, light, camera, 1);
	
	// **** ��ͷ��ñ�� ****
	// ͷ
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(0.0, Body->getHeight(), 0.0));
	painter->drawMesh(5, modelView, light, camera, 1);
	// ñ��
	modelView = glm::translate(modelView, glm::vec3(0.0, Head->getHeight(), 0.0));
	painter->drawMesh(6, modelView, light, camera, 1);

	// **** �����ۺ���С�ۺ���
	// LeftUpperHand
	modelView = mstack.pop();
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3((Body->getLength() + LeftUpperHand->getLength()) / 2, Body->getHeight() - LeftUpperHand->getHeight(), 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftUpperHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(LuffyTheta[AngleMap["Luffy"]["LeftUpperHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftUpperHand->getHeight(), 0.0));
	painter->drawMesh(7, modelView, light, camera, 1);

	// LeftLowerHand
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerHand->getHeight() + bias, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftLowerHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(LuffyTheta[AngleMap["Luffy"]["LeftLowerHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerHand->getHeight(), 0.0));
	painter->drawMesh(8, modelView, light, camera, 1);

	// Meat
	modelView = glm::translate(modelView, glm::vec3(LeftLowerHand->getLength() / 4, Meat->getWidth() / 8, 0.0));
	modelView = glm::rotate(modelView, glm::radians(80.0f), glm::vec3(1.0, 0.0, -0.3));
	painter->drawMesh(9, modelView, light, camera, 1);

	// **** ���Ҵ�ۺ���С��
	// RightUpperHand
	modelView = mstack.pop();
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(-(Body->getLength() + RightUpperHand->getLength()) * 41 / 80, Body->getHeight() - RightUpperHand->getHeight(), 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, RightUpperHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(LuffyTheta[AngleMap["Luffy"]["RightUpperHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightUpperHand->getHeight(), 0.0));
	painter->drawMesh(10, modelView, light, camera, 1);

	// RightLowerHand
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightLowerHand->getHeight() + bias, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftLowerHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(LuffyTheta[AngleMap["Luffy"]["RightLowerHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerHand->getHeight(), 0.0));
	painter->drawMesh(11, modelView, light, camera, 1);

	// **** ������Ⱥ���С�� **** 
	// �����
	modelView = mstack.pop();
	modelView = glm::translate(modelView, glm::vec3(0.0, bias / 2, 0.0));
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(Body->getLength() * 13.0f / 45.0f, -LeftUpperLeg->getHeight(), 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftUpperLeg->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(LuffyTheta[AngleMap["Luffy"]["LeftUpperLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftUpperLeg->getHeight(), 0.0));
	painter->drawMesh(12, modelView, light, camera, 1);
	//��С��
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerLeg->getHeight() + bias / 2, 0.0));
	painter->drawMesh(13, modelView, light, camera, 1);

	// **** ���Ҵ��Ⱥ���С�� ****
	// �Ҵ���
	modelView = mstack.pop();
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(-Body->getLength() * 13.0f / 45.0f, -RightUpperLeg->getHeight(), 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, RightUpperLeg->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(LuffyTheta[AngleMap["Luffy"]["RightUpperLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightUpperLeg->getHeight(), 0.0));
	painter->drawMesh(14, modelView, light, camera, 1);
	// ��С��
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightLowerLeg->getHeight() + bias / 2, 0.0));
	painter->drawMesh(15, modelView, light, camera, 1);
}

void display()
{
// #ifdef __APPLE__ // ��� macOS 10.15 ��ʾ������С����
// 	glViewport(0, 0, WIDTH * 2, HEIGHT * 2);
// #endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelView = glm::mat4(1.0);
	
	modelView = light->getModelMatrix();
	painter->drawMesh(0, modelView, light, camera, 0);

	modelView = land->getModelMatrix();
	painter->drawMesh(2, modelView, light, camera, 0);

	chr_sword->setScale(Scale[2]);
	modelView = chr_sword->getModelMatrix();
	modelView = glm::rotate(modelView, glm::radians(myTheta[2]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(3, modelView, light, camera, 1);

	drawLuffy();
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


// ������Ӧ����
std::map<std::string, bool> KeyMap;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (action == GLFW_PRESS || action == GLFW_REPEAT || action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE: 
			KeyMap["ESCAPE"] = true; 
			break;
		case GLFW_KEY_I:
			myTheta[2] = myTheta[2] + 10.0;
			while (myTheta[2] > 360) myTheta[2] -= 360;
			break;
		case GLFW_KEY_UP:
		case GLFW_KEY_DOWN:
		case GLFW_KEY_LEFT:
		case GLFW_KEY_RIGHT:
			if (action == GLFW_PRESS)
			{
				camera->upAngle = 15.00f;
				if (key == GLFW_KEY_UP) KeyMap["UP"] = true;
				else if (key == GLFW_KEY_DOWN) KeyMap["DOWN"] = true;
				else if (key == GLFW_KEY_LEFT) KeyMap["LEFT"] = true;
				else if (key == GLFW_KEY_RIGHT) KeyMap["RIGHT"] = true;
			}
			else if (action == GLFW_RELEASE)
			{
				if (key == GLFW_KEY_UP) KeyMap["UP"] = false;
				else if (key == GLFW_KEY_DOWN) KeyMap["DOWN"] = false;
				else if (key == GLFW_KEY_LEFT) KeyMap["LEFT"] = false;
				else if (key == GLFW_KEY_RIGHT) KeyMap["RIGHT"] = false;
			}
			break;
		//case GLFW_KEY_Q: exit(EXIT_SUCCESS); break;
		//case GLFW_KEY_1: Axis = Base; break;
		//case GLFW_KEY_2: Axis = UpperArm; break;
		//case GLFW_KEY_3: Axis = LowerArm; break;
		//// ͨ��������ת
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
	if (KeyMap["ESCAPE"])
		exit(EXIT_SUCCESS);
	else if (KeyMap["UP"] || KeyMap["DOWN"] || KeyMap["LEFT"] || KeyMap["RIGHT"])
	{
		moveanimation();

		auto dist = 0.05 * moveSpeed;
		glm::vec3 trans = Body->getTranslation();
		//�������Ҫ��ת�ĽǶȣ�������һ�£�
		GLfloat angle = LuffyTheta[AngleMap["Luffy"]["Body"]];
		//����������
		glm::vec3 dir = glm::normalize(glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle))));

		//�����������������Ϸ�
		camera->eye = glm::vec4(trans - 2.0f * dir + glm::vec3(0.0f, 1.0f, 0.0f), 1.0);
		camera->yaw = -angle;  //ע��updatecamera��Ϊcos(90 + yaw)��sin(angle) = cos(90 - angle);
		if (KeyMap["UP"])
		{
			trans += glm::vec3(dist * sin(glm::radians(angle)), 0.0, dist * cos(glm::radians(angle)));
		}
		if (KeyMap["DOWN"])
		{
			trans -= glm::vec3(dist * sin(glm::radians(angle)), 0.0, dist * cos(glm::radians(angle)));
		}
		if (KeyMap["LEFT"])
		{
			LuffyTheta[AngleMap["Luffy"]["Body"]] += rotateSpeed;
			if (LuffyTheta[AngleMap["Luffy"]["Body"]] > 360.0f) LuffyTheta[AngleMap["Luffy"]["Body"]] = 0.0f;
		}
			// trans += glm::vec3(dist * cos(glm::radians(angle)), 0.0, -dist * sin(glm::radians(angle)));
		if (KeyMap["RIGHT"])
		{
			LuffyTheta[AngleMap["Luffy"]["Body"]] -= rotateSpeed;
			if (LuffyTheta[AngleMap["Luffy"]["Body"]] > 360.0f) LuffyTheta[AngleMap["Luffy"]["Body"]] = 0.0f;
		}
			//trans -= glm::vec3(dist * cos(glm::radians(angle)), 0.0, -dist * sin(glm::radians(angle)));
		Body->setTranslation(trans);
	}
	else camera->keyboard(window);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera->mouse(xpos, ypos);
}
void cleanData() {
	// �ͷ��ڴ�
	
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
	// ��ʼ��GLFW�⣬������Ӧ�ó�����õĵ�һ��GLFW����
	glfwInit();

	// ����GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// ���ô�������
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "2019152051_������_��ĩ����ҵ", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//���
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	// �����κ�OpenGL�ĺ���֮ǰ��ʼ��GLAD
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Init mesh, shaders, buffer
	init();
	// ���������Ϣ
	printHelp();
	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	// ���û��
	glEnable(GL_BLEND);
	while (!glfwWindowShouldClose(window))
	{
		//���㵱ǰ֡����һ֡ʱ���
		auto currentFrame = glfwGetTime();
		camera->caldeltaTime(currentFrame);
		calMoveSpeed(currentFrame);
		//���ܼ�������
		process_key_input(window);

		display();

		// ������ɫ���� �Լ� �����û�д���ʲô�¼�������������롢����ƶ��ȣ�
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanData();


	return 0;
}

// ÿ�����ڸı��С��GLFW�������������������Ӧ�Ĳ������㴦��
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
