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
TriMesh* LeftLowerLeg = new TriMesh();
TriMesh* RightLowerLeg = new TriMesh();

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter *painter = new MeshPainter();

// ����������պ�ɾ�����Ǵ������������
std::vector<TriMesh *> meshList;


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

// ���Ƶ���
void base(glm::mat4 modelView)
{
    // ����������������壬ƽ��������λ��
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, BASE_HEIGHT / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH));

	// ���ƣ���������ֻ��һ�����������ݣ������������ֱ��ָ������painter�д洢�ĵ�0��������
	painter->drawMesh(0, modelView * instance, light, camera, 1);
	
}


// ���ƴ��
void upper_arm(glm::mat4 modelView)
{
	// @TODO: �ο������Ļ��ƣ��ڴ���Ӵ�����ƴ��
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, (BASE_HEIGHT + UPPER_ARM_HEIGHT) / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));
	// ���ƣ���������ֻ��һ�����������ݣ������������ֱ��ָ������painter�д洢�ĵ�0��������
	painter->drawMesh(0, modelView * instance, light, camera, 1);

}

// ����С��
void lower_arm(glm::mat4 modelView)
{
	// @TODO: �ο������Ļ��ƣ��ڴ���Ӵ������С��
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, (BASE_HEIGHT + UPPER_ARM_HEIGHT + LOWER_ARM_HEIGHT) / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

	// ���ƣ���������ֻ��һ�����������ݣ������������ֱ��ָ������painter�д洢�ĵ�0��������
	painter->drawMesh(0, modelView * instance, light, camera, 1);
}

void drawMonu9(glm::mat4 modelView)
{
	painter->drawMesh(1, modelView, light, camera, 1);
}
void init()
{
	std::string vshader, cfshader, tfshader;
	// ��ȡ��ɫ����ʹ��
	vshader = "shaders/vshader.glsl";
	cfshader = "shaders/cfshader.glsl";
	tfshader = "shaders/tfshader.glsl";

	// ���ù�Դλ��
	light->readObj("assets/Myobj/sun/sun.obj");
	light->setTranslation(glm::vec3(5.0, 10.0, 10.0));
	light->setScale(glm::vec3(1, 1, 1));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // ������
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // ������
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // ���淴��
	painter->addMesh(light, "light", "assets/Myobj/sun/sun.png", vshader, tfshader);
	meshList.push_back(light);

	//���������۵Ļ���������
	cube->setNormalize(false);
	cube->generateCube();
	cube->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	cube->setRotation(glm::vec3(0.0, 0.0, 0.0));
	cube->setScale(glm::vec3(1.0, 1.0, 1.0));
	// �ӵ�painter��
	// ָ����������ɫ������Ϊ��������ͼƬ���Ծʹ������ַ�����ȥ��
	painter->addMesh(cube, "Cube", "", vshader, cfshader);
	meshList.push_back(cube);

	//�ݵ�
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

	
	chr_sword->setNormalize(true);
	chr_sword->readObj("assets/chr_sword/chr_sword.obj");
	chr_sword->setTranslation(glm::vec3(1.0, 0.0, 1.0));
	Scale[2] = glm::vec3(1, 1, 1);
	chr_sword->setScale(Scale[2]);
	chr_sword->setAmbient(glm::vec4(0.3, 0.3, 0.3, 1.0)); // ������
	chr_sword->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // ������
	chr_sword->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // ���淴��
	chr_sword->setShininess(1.0); //�߹�ϵ��
	painter->addMesh(chr_sword, "chr_sword", "assets/chr_sword/chr_sword.png", vshader, tfshader);
	meshList.push_back(chr_sword);

	LeftLowerLeg->setNormalize(true);
	LeftLowerLeg->readObj("assets/Myobj/Luffy/LeftLowerLeg.obj");
	LeftLowerLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	LeftLowerLeg->setScale(glm::vec3(1, 1, 1));
	LeftLowerLeg->setAmbient(glm::vec4(0.3, 0.3, 0.3, 1.0)); // ������
	LeftLowerLeg->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // ������
	LeftLowerLeg->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // ���淴��
	LeftLowerLeg->setShininess(1.0); //�߹�ϵ��
	painter->addMesh(LeftLowerLeg, "LeftLowerLeg", "assets/Myobj/Luffy/LeftLowerLeg.png", vshader, tfshader);
	meshList.push_back(LeftLowerLeg);

	RightLowerLeg->setNormalize(true);
	RightLowerLeg->readObj("assets/Myobj/Luffy/RightLowerLeg.obj");
	RightLowerLeg->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	RightLowerLeg->setScale(glm::vec3(1, 1, 1));
	RightLowerLeg->setAmbient(glm::vec4(0.3, 0.3, 0.3, 1.0)); // ������
	RightLowerLeg->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // ������
	RightLowerLeg->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // ���淴��
	RightLowerLeg->setShininess(1.0); //�߹�ϵ��
	painter->addMesh(RightLowerLeg, "RightLowerLeg", "assets/Myobj/Luffy/RightLowerLeg.png", vshader, tfshader);
	meshList.push_back(RightLowerLeg);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display()
{
// #ifdef __APPLE__ // ��� macOS 10.15 ��ʾ������С����
// 	glViewport(0, 0, WIDTH * 2, HEIGHT * 2);
// #endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���Ƶ��� 
	glm::mat4 modelView = glm::mat4(1.0);
	//modelView = glm::translate(modelView, glm::vec3(0.0, 0.0, 0.0));// ��΢����һ�£��û��������������ԭ��
	//modelView = glm::rotate(modelView, glm::radians(Theta[Base]), glm::vec3(0.0, 1.0, 0.0));// ������ת����
	//base(modelView); // ���Ȼ��Ƶ���

 //   // @TODO: �ڴ���Ӵ������������е�ֱۻ���
	//// ��۱任����
	//modelView = glm::rotate(modelView, glm::radians(Theta[UpperArm]), glm::vec3(0.0, 1.0, 0.0));// ������ת����
	//// ���ƴ��
	//upper_arm(modelView);

	//// С�۱任����
	//modelView = glm::rotate(modelView, glm::radians(Theta[LowerArm]), glm::vec3(0.0, 1.0, 0.0));// ������ת����
	//// ����С��	
	//lower_arm(modelView);

	modelView = light->getModelMatrix();
	painter->drawMesh(0, modelView, light, camera, 0);

	modelView = land->getModelMatrix();
	painter->drawMesh(2, modelView, light, camera, 0);

	chr_sword->setScale(Scale[2]);
	modelView = chr_sword->getModelMatrix();
	modelView = glm::rotate(modelView, glm::radians(myTheta[2]), glm::vec3(0.0, 1.0, 0.0));
	painter->drawMesh(3, modelView, light, camera, 1);

	modelView = LeftLowerLeg->getModelMatrix();
	painter->drawMesh(4, modelView, light, camera, 1);

	modelView = glm::translate(modelView, glm::vec3(1.0, 0.0, 0.0));
	painter->drawMesh(5, modelView, light, camera, 1);
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		exit(EXIT_SUCCESS);
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
	GLFWwindow* window = glfwCreateWindow(1600, 900, "2019152051_������_��ĩ����ҵ", NULL, NULL);
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
		camera->caldeltaTime(glfwGetTime());
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
