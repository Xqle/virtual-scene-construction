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

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter* painter = new MeshPainter();

TriMesh* BackGround = new TriMesh();
TriMesh* chr_sword = new TriMesh();
TriMesh* land = new TriMesh();

// Luffy
// 躯干
TriMesh* Body = new TriMesh();
// 头和帽子
TriMesh* Head = new TriMesh();
TriMesh* Hat = new TriMesh();
// 大臂
TriMesh* LeftUpperHand = new TriMesh();
TriMesh* RightUpperHand = new TriMesh();
// 小臂
TriMesh* LeftLowerHand = new TriMesh();
TriMesh* RightLowerHand = new TriMesh();
// 肉
TriMesh* Meat = new TriMesh();
// 大腿
TriMesh* LeftUpperLeg = new TriMesh();
TriMesh* RightUpperLeg = new TriMesh();
// 小腿
TriMesh* LeftLowerLeg = new TriMesh();
TriMesh* RightLowerLeg = new TriMesh();

// 救护车
TriMesh* AmbulanceBody = new TriMesh();
TriMesh* AmbulanceFrontWheels = new TriMesh();
TriMesh* AmbulanceBackWheels = new TriMesh();
//给对应mesh下标建立索引
const int NumMeshes = 200;
std::map<std::string, int> IndexMap;
glm::vec3 Scale[NumMeshes] = { glm::vec3(0.0, 0.0, 0.0) };
GLfloat Theta[NumMeshes] = { 0.0 };

const int NumCtrlMesh = 3;  //可以控制的Mesh数量
std::map<int, std::string> CtrlMeshMap;
int CurCtrlMesh;   //当前控制的Mesh
std::string CurMeshName = "camera";  //当前控制的Mesh的名字

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

float dir = -1.0, moveSpeedBase = 100.0f, moveSpeed, rotateSpeedBase = 100.0f, rotateSpeed;
float lastFrame;
//计算移动速度，旋转速度等
void calMoveSpeed(float currentFrame)
{
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	moveSpeed = moveSpeedBase * deltaTime;
	rotateSpeed = rotateSpeedBase * deltaTime;
}

void moveanimation()
{
	// 路飞摆手动画
	if (CurMeshName == "Body")
	{
		if (Theta[IndexMap["LeftUpperHand"]] > 45.0 || Theta[IndexMap["LeftUpperHand"]] < -45.0)
		{
			//控制摆动方向
			dir = -dir;
			//两个关键帧
			if (dir < 0)
			{
				Theta[IndexMap["LeftUpperHand"]] = 45.00f;
				Theta[IndexMap["RightUpperHand"]] = -45.00f;
				Theta[IndexMap["LeftLowerHand"]] = 0.0f;
				Theta[IndexMap["RightLowerHand"]] = -45.00f;
			}
			else
			{
				Theta[IndexMap["LeftUpperHand"]] = -45.00f;
				Theta[IndexMap["RightUpperHand"]] = 45.00f;
				Theta[IndexMap["LeftLowerHand"]] = -45.00f;
				Theta[IndexMap["RightLowerHand"]] = 0.0f;
			}
		}
		Theta[IndexMap["LeftUpperHand"]] += dir * moveSpeed;
		// 左大臂在身体前面的时候，左小臂才开摆
		if (Theta[IndexMap["LeftUpperHand"]] < 0)
			Theta[IndexMap["LeftLowerHand"]] += dir * moveSpeed;
		Theta[IndexMap["RightUpperHand"]] -= dir * moveSpeed;
		// 右大臂在身体前面的时候，右小臂才开摆
		if (Theta[IndexMap["RightUpperHand"]] < 0)
			Theta[IndexMap["RightLowerHand"]] -= dir * moveSpeed;
		Theta[IndexMap["RightUpperLeg"]] += dir * moveSpeed;
		Theta[IndexMap["LeftUpperLeg"]] -= dir * moveSpeed;
	}
	//轮胎转动动画
	else if (CurMeshName == "AmbulanceBody")
	{
		Theta[IndexMap["AmbulanceFrontWheels"]] -= dir * moveSpeed * 4; 
		if (Theta[IndexMap["AmbulanceFrontWheels"]] > 360.0f) Theta[IndexMap["AmbulanceFrontWheels"]] -= 360.0f;
		Theta[IndexMap["AmbulanceBackWheels"]] -= dir * moveSpeed * 4;
		if (Theta[IndexMap["AmbulanceBackWheels"]] > 360.0f) Theta[IndexMap["AmbulanceBackWheels"]] -= 360.0f;
	}
}	


// 尺寸参数
const GLfloat BASE_HEIGHT      = 0.2;
const GLfloat BASE_WIDTH       = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 0.3;
const GLfloat UPPER_ARM_WIDTH  = 0.2;
const GLfloat LOWER_ARM_HEIGHT = 0.4;
const GLfloat LOWER_ARM_WIDTH  = 0.1;

void init()
{
	std::string vshader, cfshader, tfshader, NoPhonefshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	cfshader = "shaders/cfshader.glsl";
	tfshader = "shaders/tfshader.glsl";
	NoPhonefshader = "shaders/NoPhonefshader.glsl";
	int idx = 0, CtrlMeshMapIndex = 0;
	IndexMap["camera"] = 100;   //给camera一个不可能被覆盖的index
	CtrlMeshMap[CtrlMeshMapIndex++] = "camera";   //摄像机可控制

	// 设置光源位置  --  0
	IndexMap["light"] = idx++;
	light->readObj("assets/Myobj/sun/sun.obj");
	light->setTranslation(glm::vec3(20.0, 45.0f, 20.0));
	light->setScale(glm::vec3(2, 2, 2));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	painter->addMesh(light, "light", "assets/Myobj/sun/sun.png", vshader, tfshader);
	meshList.push_back(light);

	// BackGround  --  1
	IndexMap["BackGround"] = idx++;
	BackGround->setNormalize(false);
	BackGround->readObj("assets/Myobj/BackGround/BackGround.obj");
	BackGround->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	BackGround->setRotation(glm::vec3(0.0, 0.0, 0.0));
	BackGround->setScale(glm::vec3(1.0, 1.0, 1.0));
	// 加到painter中
	painter->addMesh(BackGround, "BackGround", "assets/Myobj/BackGround/BackGround.png", vshader, NoPhonefshader);
	meshList.push_back(BackGround);

	//草地  --  2
	IndexMap["land"] = idx++;
	land->setNormalize(false);
	land->generateSquare(glm::vec3(0.78f, 0.5f, 0.4f));
	land->setTranslation(glm::vec3(0.0, -0.001, 0.0));    //加点偏移，不要跟阴影重合
	land->setRotation(glm::vec3(90.0, 0.0, 0.0));
	Scale[IndexMap["land"]] = glm::vec3(50.0, 50.0, 50.0);
	land->setScale(Scale[IndexMap["land"]]);
	land->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	land->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	land->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	land->setShininess(1.0); //高光系数
	painter->addMesh(land, "land", "assets/grass3.jpg", vshader, tfshader);

	// chr_sword  --  3
	IndexMap["chr_sword"] = idx++;
	chr_sword->setNormalize(true);
	chr_sword->readObj("assets/chr_sword/chr_sword.obj");
	Scale[2] = glm::vec3(1, 1, 1);
	chr_sword->setScale(Scale[IndexMap["chr_sword"]]);
	painter->addMesh(chr_sword, "chr_sword", "assets/chr_sword/chr_sword.png", vshader, tfshader);
	meshList.push_back(chr_sword);

	glm::vec3 LuffyScale = glm::vec3(0.015, 0.015, 0.015);
	//Body  --  4
	IndexMap["Body"] = idx++;
	CtrlMeshMap[CtrlMeshMapIndex++] = "Body";     // 路飞可控制
	Body->setNormalize(false);
	Body->readObj("assets/Myobj/Luffy/Body.obj");
	Scale[IndexMap["Body"]] = LuffyScale;
	Body->setScale(Scale[IndexMap["Body"]]);
	painter->addMesh(Body, "Body", "assets/Myobj/Luffy/Body.png", vshader, tfshader);
	meshList.push_back(Body);

	//Head  --  5
	IndexMap["Head"] = idx++;
	Head->setNormalize(false);
	Head->readObj("assets/Myobj/Luffy/Head.obj");
	Scale[IndexMap["Head"]] = LuffyScale;
	Head->setScale(Scale[IndexMap["Head"]]);
	painter->addMesh(Head, "Head", "assets/Myobj/Luffy/Head.png", vshader, tfshader);
	meshList.push_back(Head);

	//Hat  --  6
	IndexMap["Hat"] = idx++;
	Hat->setNormalize(false);
	Hat->readObj("assets/Myobj/Luffy/Hat.obj");
	Scale[IndexMap["Hat"]] = LuffyScale;
	Hat->setScale(Scale[IndexMap["Hat"]]);
	painter->addMesh(Hat, "Hat", "assets/Myobj/Luffy/Hat.png", vshader, tfshader);
	meshList.push_back(Hat);

	// LeftUpperHand  --  7
	IndexMap["LeftUpperHand"] = idx++;
	LeftUpperHand->setNormalize(false);
	LeftUpperHand->readObj("assets/Myobj/Luffy/LeftUpperHand.obj");
	Scale[IndexMap["LeftUpperHand"]] = LuffyScale;
	LeftUpperHand->setScale(Scale[IndexMap["LeftUpperHand"]]);
	painter->addMesh(LeftUpperHand, "LeftUpperHand", "assets/Myobj/Luffy/LeftUpperHand.png", vshader, tfshader);
	meshList.push_back(LeftUpperHand);

	// LeftLowerHand  --  8
	IndexMap["LeftLowerHand"] = idx++;
	LeftLowerHand->setNormalize(false);
	LeftLowerHand->readObj("assets/Myobj/Luffy/LeftLowerHand.obj");
	Scale[IndexMap["LeftLowerHand"]] = LuffyScale;
	LeftLowerHand->setScale(Scale[IndexMap["LeftLowerHand"]]);
	painter->addMesh(LeftLowerHand, "LeftLowerHand", "assets/Myobj/Luffy/LeftLowerHand.png", vshader, tfshader);
	meshList.push_back(LeftLowerHand);

	// Meat  --  9
	IndexMap["Meat"] = idx++;
	Meat->setNormalize(false);
	Meat->readObj("assets/Myobj/Luffy/Meat.obj");
	Scale[IndexMap["Meat"]] = LuffyScale;
	Meat->setScale(Scale[IndexMap["Meat"]]);
	painter->addMesh(Meat, "Meat", "assets/Myobj/Luffy/Meat.png", vshader, tfshader);
	meshList.push_back(Meat);

	// RightUpperHand  --  10
	IndexMap["RightUpperHand"] = idx++;
	RightUpperHand->setNormalize(false);
	RightUpperHand->readObj("assets/Myobj/Luffy/RightUpperHand.obj");
	Scale[IndexMap["RightUpperHand"]] = LuffyScale;
	RightUpperHand->setScale(Scale[IndexMap["RightUpperHand"]]);
	painter->addMesh(RightUpperHand, "RightUpperHand", "assets/Myobj/Luffy/RightUpperHand.png", vshader, tfshader);
	meshList.push_back(RightUpperHand);

	// RightLowerHand  --  11
	IndexMap["RightLowerHand"] = idx++;
	RightLowerHand->setNormalize(false);
	RightLowerHand->readObj("assets/Myobj/Luffy/RightLowerHand.obj");
	Scale[IndexMap["RightLowerHand"]] = LuffyScale;
	RightLowerHand->setScale(Scale[IndexMap["RightLowerHand"]]);
	painter->addMesh(RightLowerHand, "RightLowerHand", "assets/Myobj/Luffy/RightLowerHand.png", vshader, tfshader);
	meshList.push_back(RightLowerHand);

	// LeftUpperLeg  --  12
	IndexMap["LeftUpperLeg"] = idx++;
	LeftUpperLeg->setNormalize(false);
	LeftUpperLeg->readObj("assets/Myobj/Luffy/LeftUpperLeg.obj");
	Scale[IndexMap["LeftUpperLeg"]] = LuffyScale;
	LeftUpperLeg->setScale(Scale[IndexMap["LeftUpperLeg"]]);
	painter->addMesh(LeftUpperLeg, "LeftUpperLeg", "assets/Myobj/Luffy/LeftUpperLeg.png", vshader, tfshader);
	meshList.push_back(LeftUpperLeg);

	// LeftLowerLeg  --  13
	IndexMap["LeftLowerLeg"] = idx++;
	LeftLowerLeg->setNormalize(false);
	LeftLowerLeg->readObj("assets/Myobj/Luffy/LeftLowerLeg.obj");
	Scale[IndexMap["LeftLowerLeg"]] = LuffyScale;
	LeftLowerLeg->setScale(Scale[IndexMap["LeftLowerLeg"]]);
	painter->addMesh(LeftLowerLeg, "LeftLowerLeg", "assets/Myobj/Luffy/LeftLowerLeg.png", vshader, tfshader);
	meshList.push_back(LeftLowerLeg);

	// RightUpperLeg  --  14
	IndexMap["RightUpperLeg"] = idx++;
	RightUpperLeg->setNormalize(false);
	RightUpperLeg->readObj("assets/Myobj/Luffy/RightUpperLeg.obj");
	Scale[IndexMap["RightUpperLeg"]] = LuffyScale;
	RightUpperLeg->setScale(Scale[IndexMap["RightUpperLeg"]]);
	painter->addMesh(RightUpperLeg, "RightUpperLeg", "assets/Myobj/Luffy/RightUpperLeg.png", vshader, tfshader);
	meshList.push_back(RightUpperLeg);

	// RightLowerLeg  --  15
	IndexMap["RightLowerLeg"] = idx++;
	RightLowerLeg->setNormalize(false);
	RightLowerLeg->readObj("assets/Myobj/Luffy/RightLowerLeg.obj");
	Scale[IndexMap["RightLowerLeg"]] = LuffyScale;
	RightLowerLeg->setScale(Scale[IndexMap["RightLowerLeg"]]);
	painter->addMesh(RightLowerLeg, "RightLowerLeg", "assets/Myobj/Luffy/RightLowerLeg.png", vshader, tfshader);
	meshList.push_back(RightLowerLeg);


	// Ambulance
	glm::vec3 AmbulanceScale = glm::vec3(0.25, 0.25, 0.25);
	// Ambulance Body  --  16
	IndexMap["AmbulanceBody"] = idx++;
	CtrlMeshMap[CtrlMeshMapIndex++] = "AmbulanceBody";  //救护车可控制
	AmbulanceBody->setNormalize(false);
	AmbulanceBody->setTranslation(glm::vec3(-2.00f, 0.0f, 0.0f));
	AmbulanceBody->readObj("assets/Myobj/Ambulance/AmbulanceBody.obj");
	Scale[IndexMap["AmbulanceBody"]] = AmbulanceScale;
	AmbulanceBody->setScale(Scale[IndexMap["AmbulanceBody"]]);
	painter->addMesh(AmbulanceBody, "AmbulanceBody", "assets/Myobj/Ambulance/AmbulanceBody.png", vshader, tfshader);
	meshList.push_back(AmbulanceBody);

	// Ambulance Front Wheels  --  17
	IndexMap["AmbulanceFrontWheels"] = idx++;
	AmbulanceFrontWheels->setNormalize(false);
	AmbulanceFrontWheels->readObj("assets/Myobj/Ambulance/AmbulanceFrontWheels.obj");
	Scale[IndexMap["AmbulanceFrontWheels"]] = AmbulanceScale;
	AmbulanceFrontWheels->setScale(Scale[IndexMap["AmbulanceFrontWheels"]]);
	painter->addMesh(AmbulanceFrontWheels, "AmbulanceFrontWheels", "assets/Myobj/Ambulance/AmbulanceFrontWheels.png", vshader, tfshader);
	meshList.push_back(AmbulanceFrontWheels);

	// Ambulance Back Wheels  --  18
	IndexMap["AmbulanceBackWheels"] = idx++;
	AmbulanceBackWheels->setNormalize(false);
	AmbulanceBackWheels->readObj("assets/Myobj/Ambulance/AmbulanceBackWheels.obj");
	Scale[IndexMap["AmbulanceBackWheels"]] = AmbulanceScale;
	AmbulanceBackWheels->setScale(Scale[IndexMap["AmbulanceBackWheels"]]);
	painter->addMesh(AmbulanceBackWheels, "AmbulanceBackWheels", "assets/Myobj/Ambulance/AmbulanceBackWheels.png", vshader, tfshader);
	meshList.push_back(AmbulanceBackWheels);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void drawLuffy()
{
	float bias = 0.5;

	// **** 画Body ****
	MatrixStack mstack;
	glm::mat4 modelView = Body->getModelMatrix();
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftLowerLeg->getHeight() + LeftUpperLeg->getHeight() - bias, 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["Body"]]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(IndexMap["Body"], modelView, light, camera, 1);
	
	// **** 画头和帽子 ****
	// Head
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(0.0, Body->getHeight(), 0.0));
	painter->drawMesh(IndexMap["Head"], modelView, light, camera, 1);
	// Hat
	modelView = glm::translate(modelView, glm::vec3(0.0, Head->getHeight(), 0.0));
	painter->drawMesh(IndexMap["Hat"], modelView, light, camera, 1);

	// **** 画左大臂和左小臂和肉
	// LeftUpperHand
	modelView = mstack.pop();
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3((Body->getLength() + LeftUpperHand->getLength()) / 2, Body->getHeight() - LeftUpperHand->getHeight(), 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftUpperHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["LeftUpperHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftUpperHand->getHeight(), 0.0));
	painter->drawMesh(IndexMap["LeftUpperHand"], modelView, light, camera, 1);

	// LeftLowerHand
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerHand->getHeight() + bias, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftLowerHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["LeftLowerHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerHand->getHeight(), 0.0));
	painter->drawMesh(IndexMap["LeftLowerHand"], modelView, light, camera, 1);

	// Meat
	modelView = glm::translate(modelView, glm::vec3(LeftLowerHand->getLength() / 4, Meat->getWidth() / 8, 0.0));
	modelView = glm::rotate(modelView, glm::radians(80.0f), glm::vec3(1.0, 0.0, -0.3));
	painter->drawMesh(9, modelView, light, camera, 1);

	// **** 画右大臂和右小臂
	// RightUpperHand
	modelView = mstack.pop();
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(-(Body->getLength() + RightUpperHand->getLength()) * 41 / 80, Body->getHeight() - RightUpperHand->getHeight(), 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, RightUpperHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["RightUpperHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightUpperHand->getHeight(), 0.0));
	painter->drawMesh(IndexMap["RightUpperHand"], modelView, light, camera, 1);

	// RightLowerHand
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightLowerHand->getHeight() + bias, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftLowerHand->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["RightLowerHand"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerHand->getHeight(), 0.0));
	painter->drawMesh(IndexMap["RightLowerHand"], modelView, light, camera, 1);

	// **** 画左大腿和左小腿 **** 
	// LeftUpperLeg
	modelView = mstack.pop();
	modelView = glm::translate(modelView, glm::vec3(0.0, bias / 2, 0.0));
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(Body->getLength() * 13.0f / 45.0f, -LeftUpperLeg->getHeight(), 0.0));
	// 为了得到正确的旋转，需要先将腿往下移，绕原点旋转，再恢复原位
	modelView = glm::translate(modelView, glm::vec3(0.0, LeftUpperLeg->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["LeftUpperLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftUpperLeg->getHeight(), 0.0));
	painter->drawMesh(IndexMap["LeftUpperLeg"], modelView, light, camera, 1);
	// LeftLowerLeg
	modelView = glm::translate(modelView, glm::vec3(0.0, -LeftLowerLeg->getHeight() + bias / 2, 0.0));
	painter->drawMesh(IndexMap["LeftLowerLeg"], modelView, light, camera, 1);

	// **** 画右大腿和右小腿 ****
	// RightUpperLeg
	modelView = mstack.pop();
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(-Body->getLength() * 13.0f / 45.0f, -RightUpperLeg->getHeight(), 0.0));
	// 理由同上
	modelView = glm::translate(modelView, glm::vec3(0.0, RightUpperLeg->getHeight(), 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["RightUpperLeg"]]), glm::vec3(1.0, 0.0, 0.0));
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightUpperLeg->getHeight(), 0.0));
	painter->drawMesh(IndexMap["RightUpperLeg"], modelView, light, camera, 1);
	// RightLowerLeg
	modelView = glm::translate(modelView, glm::vec3(0.0, -RightLowerLeg->getHeight() + bias / 2, 0.0));
	painter->drawMesh(IndexMap["RightLowerLeg"], modelView, light, camera, 1);
}

void drawAmbulance()
{
	MatrixStack mstack;
	glm::mat4 modelView = AmbulanceBody->getModelMatrix();
	// Ambulance Body
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["AmbulanceBody"]]), glm::vec3(0.0f, 1.0f, 0.0f));
	modelView = glm::translate(modelView, glm::vec3(0.0f, AmbulanceFrontWheels->getHeight() * 0.2, 0.0f));
	painter->drawMesh(IndexMap["AmbulanceBody"], modelView, light, camera, 1);

	modelView = glm::translate(modelView, glm::vec3(0.0f, -AmbulanceFrontWheels->getHeight() * 0.2, 0.0f));
	// Ambulance Front Wheels
	// 前轮的转动
	mstack.push(modelView);
	modelView = glm::translate(modelView, glm::vec3(0.0f, AmbulanceFrontWheels->getHeight() * 0.5, AmbulanceFrontWheels->getWidth() * 21 / 10));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["AmbulanceFrontWheels"]]), glm::vec3(1.0f, 0.0f, 0.0f));
	modelView = glm::translate(modelView, glm::vec3(0.0f, -AmbulanceFrontWheels->getHeight() * 0.5, -AmbulanceFrontWheels->getWidth() * 21 / 10));
	painter->drawMesh(IndexMap["AmbulanceFrontWheels"], modelView, light, camera, 1);

	// Ambulance Back Wheels	
	modelView = mstack.pop();;
	// 后轮的转动
	modelView = glm::translate(modelView, glm::vec3(0.0f, AmbulanceBackWheels->getHeight() * 0.5, -AmbulanceBackWheels->getWidth() * 19 / 10));
	modelView = glm::rotate(modelView, glm::radians(Theta[IndexMap["AmbulanceBackWheels"]]), glm::vec3(1.0f, 0.0f, 0.0f));
	modelView = glm::translate(modelView, glm::vec3(0.0f, -AmbulanceBackWheels->getHeight() * 0.5, AmbulanceBackWheels->getWidth() * 19 / 10));
	painter->drawMesh(IndexMap["AmbulanceBackWheels"], modelView, light, camera, 1);

}

void display()
{
// #ifdef __APPLE__ // 解决 macOS 10.15 显示画面缩小问题
// 	glViewport(0, 0, WIDTH * 2, HEIGHT * 2);
// #endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelView = glm::mat4(1.0);
	
	modelView = light->getModelMatrix();
	painter->drawMesh(0, modelView, light, camera, 0);

	modelView = BackGround->getModelMatrix();
	modelView = glm::scale(modelView, glm::vec3(2.0f, 2.0f, 2.0f));
	painter->drawMesh(1, modelView, light, camera, 0);

	modelView = land->getModelMatrix();
	painter->drawMesh(2, modelView, light, camera, 0);

	chr_sword->setScale(Scale[2]);
	modelView = chr_sword->getModelMatrix();
	modelView = glm::translate(modelView, glm::vec3(1.0, 0.0, 1.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[2]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(3, modelView, light, camera, 1);

	drawLuffy();
	drawAmbulance();
}

void printHelp()
{
	std::cout << "================================================" << std::endl;
	std::cout << "Use mouse to controll the direction of camera." << std::endl;
	std::cout << "================================================" << std::endl << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<

		"[Mesh]" << std::endl <<
		"W:	Control the current mesh to move forward" << std::endl <<
		"S:	Control the current mesh to move backward" << std::endl <<
		"A:	Control the current mesh to turn left" << std::endl <<
		"D:	Control the current mesh to turn right" << std::endl <<
		"SPACE: Switch to the next mesh or the camera" << std::endl <<
		"LEFT: To watch the mesh from the left of the mesh" <<

		std::endl <<
		"[Camera]" << std::endl <<
		"W:	Control the camera to move forward" << std::endl <<
		"S:	Control the camera to move backward" << std::endl <<
		"A:	Control the camera to move left" << std::endl <<
		"D:	Control the camera to move right" << std::endl <<
		"SPACE: Switch to the next mesh" << std::endl;
		
}


// 键盘响应函数
std::map<std::string, bool> KeyMap;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (action == GLFW_PRESS || action == GLFW_REPEAT || action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_SPACE:
			if (action == GLFW_PRESS)
			{
				CurCtrlMesh = (CurCtrlMesh + 1) % NumCtrlMesh;
				CurMeshName = CtrlMeshMap[CurCtrlMesh];
				if (CurMeshName != "camera")
				{
					//调整相机角度到当前控制的Mesh的后上方
					GLfloat angle = Theta[IndexMap[CurMeshName]];
					glm::vec3 trans;
					if (CurMeshName == "Body") trans = Body->getTranslation();
					else if (CurMeshName == "AmbulanceBody") trans = AmbulanceBody->getTranslation();
					glm::vec3 dir = glm::normalize(glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle))));
					camera->pitch = 0.0f;
					camera->yaw = -angle;  //注意updatecamera里为cos(90 + yaw)，sin(angle) = cos(90 - angle);
					camera->upAngle = 15.00f;
					camera->eye = glm::vec4(trans - 2.0f * dir + glm::vec3(0.0f, 1.0f, 0.0f), 1.0);
				}
				else
				{
					//否则给个弹出效果提示用户切换到了相机
					GLfloat angle = Theta[IndexMap["AmbulanceBody"]];
					glm::vec3 dir = glm::normalize(glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle))));
					camera->eye += glm::vec4(-dir + glm::vec3(0.0f, 0.1f, 0.0f), 1.0);
				}
				std::cout << CurMeshName << std::endl;
			}
			break;
		case GLFW_KEY_ESCAPE: 
			KeyMap["ESCAPE"] = true; 
			break;
		// WASD 改变方向
		case GLFW_KEY_W:
		case GLFW_KEY_S:
		case GLFW_KEY_A:
		case GLFW_KEY_D:
			if (action == GLFW_PRESS)
			{
				if (CurMeshName != "camera")
				{
					camera->pitch = 0.0f;
					camera->yaw = -Theta[IndexMap[CurMeshName]];  //注意updatecamera里为cos(90 + yaw)，sin(angle) = cos(90 - angle);
					camera->upAngle = 15.00f;
				}
				if (key == GLFW_KEY_W) KeyMap["W"] = true;
				else if (key == GLFW_KEY_S) KeyMap["S"] = true;
				else if (key == GLFW_KEY_A) KeyMap["A"] = true;
				else if (key == GLFW_KEY_D) KeyMap["D"] = true;
			}
			else if (action == GLFW_RELEASE)
			{
				if (key == GLFW_KEY_W) KeyMap["W"] = false;
				else if (key == GLFW_KEY_S) KeyMap["S"] = false;
				else if (key == GLFW_KEY_A) KeyMap["A"] = false;
				else if (key == GLFW_KEY_D) KeyMap["D"] = false;
			}
			break;
		case GLFW_KEY_LEFT:
			if (action == GLFW_PRESS) KeyMap["LEFT"] = true;
			else if (action == GLFW_RELEASE) KeyMap["LEFT"] = false;
			break;
		// 按 8 加速
		case GLFW_KEY_8:
			// 相机最高不过 8.0f，物体最高不过 200.0f；
			if (CurMeshName == "camera") camera->cameraSpeedBase = std::min(camera->cameraSpeedBase + 0.5f, 8.0f);
			else
			{
				moveSpeedBase = std::min(moveSpeedBase + 10.0f, 200.0f);
				rotateSpeedBase = std::min(rotateSpeedBase + 10.0f, 200.0f);
			}
			break;
		// 按 9 减速
		case GLFW_KEY_9:
			// 相机最低不下 1.0f，物体最低不下 50.0f
			if (CurMeshName == "camera") camera->cameraSpeedBase = std::max(camera->cameraSpeedBase - 0.5f, 1.0f);
			else
			{
				moveSpeedBase = std::max(moveSpeedBase - 10.0f, 50.0f);
				rotateSpeedBase = std::max(rotateSpeedBase - 10.0f, 50.0f);
			}
			break;
		}
	}
}

void process_key_input(GLFWwindow *window)
{
	// 如果是当前Mesh是路飞，在全部键松开缓慢恢复站立姿势
	if (CurMeshName == "Body" && !KeyMap["W"] && !KeyMap["S"] && !KeyMap["A"] && !KeyMap["D"])
	{
		for (int i = IndexMap["Body"]; i < IndexMap["AmbulanceBody"]; i++)
			if (i != IndexMap["Body"] && fabs(Theta[i]) > 0.1)
			{
				float s = Theta[i] / fabs(Theta[i]);
				Theta[i] -= 2 * s * rotateSpeed;
			}
	}

	if (KeyMap["ESCAPE"]) exit(EXIT_SUCCESS);
	// 如果控制的不是相机就进行处理
	else if (CtrlMeshMap[CurCtrlMesh] != "camera" && (KeyMap["W"] || KeyMap["S"] || KeyMap["A"] || KeyMap["D"]))
	{
		moveanimation();

		auto dist = 0.05 * moveSpeed;
		glm::vec3 trans;
		// 获取对应物体的T矩阵
		if (CurMeshName == "Body") trans = Body->getTranslation();
		else if (CurMeshName == "AmbulanceBody") trans = AmbulanceBody->getTranslation();
		// 摄像机需要旋转的角度（与人物一致）
		GLfloat angle = Theta[IndexMap[CurMeshName]];
		// 获取人物面向的方向
		glm::vec3 dir = glm::normalize(glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle))));
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		// 如果按住左键，则摄像机设置于物体左边，并看向物体，便于观察物体动作
		if (KeyMap["LEFT"])
		{
			camera->eye = glm::vec4(trans - 2.0f * glm::normalize(glm::cross(dir, up)) + glm::vec3(0.0f, 0.5f, 0.0f), 1.0f);
			camera->yaw = -angle + 90.0f;
		}
		// 否则摄像机设置于人物后上方
		else
		{
			camera->eye = glm::vec4(trans - 2.0f * dir + glm::vec3(0.0f, 1.0f, 0.0f), 1.0);
			camera->yaw = -angle;
		}
		// W/S控制前进后退，A/D控制左右转向
		if (KeyMap["W"])
			trans += glm::vec3(dist * sin(glm::radians(angle)), 0.0, dist * cos(glm::radians(angle)));
		if (KeyMap["S"])
			trans -= glm::vec3(dist * sin(glm::radians(angle)), 0.0, dist * cos(glm::radians(angle)));
		if (KeyMap["A"])
		{
			camera->yaw = -angle;  // 转弯时强制锁定视角
			if (KeyMap["LEFT"]) camera->yaw += 90.0f;
			// 让当前物体的第0层转向
			Theta[IndexMap[CurMeshName]] += rotateSpeed;
			if (Theta[IndexMap[CurMeshName]] > 360.0f) Theta[IndexMap[CurMeshName]] -= 360.0f;
		}
		if (KeyMap["D"])
		{
			camera->yaw = -angle;  // 转弯时强制锁定视角
			if (KeyMap["LEFT"]) camera->yaw += 90.0f;
			// 让当前物体的第0层转向
			Theta[IndexMap[CurMeshName]] -= rotateSpeed;
			if (Theta[IndexMap[CurMeshName]] < 0.0f) Theta[IndexMap[CurMeshName]] += 360.0f;
		}
		// 记得保存T矩阵
		// 空气墙
		trans.x = std::min(25.0f, trans.x);
		trans.x = std::max(-25.0f, trans.x);
		trans.z = std::min(25.0f, trans.z);
		trans.z = std::max(-25.0f, trans.z);
		if (CurMeshName == "Body")
			Body->setTranslation(trans);
		else if (CurMeshName == "AmbulanceBody")
			AmbulanceBody->setTranslation(trans);
	}
	// 如果控制的是相机，则将按键传到相机
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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "2019152051_许琪乐_期末大作业", NULL, NULL);
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
		auto currentFrame = glfwGetTime();
		camera->caldeltaTime(currentFrame);
		calMoveSpeed(currentFrame);
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
