#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"
#include "Model.h"
#include "Animation.h"
#include "Animator.h"
#include "Object.h"
#include <assimp/Importer.hpp>        //assimp库头文件
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <chrono>

int WIDTH = 600;
int HEIGHT = 600;
// 第一个天空盒
std::vector<std::string> first_cubemap_faces{
"./assets/environment/cubemap/first/px.png",
"./assets/environment/cubemap/first/nx.png",
"./assets/environment/cubemap/first/py.png",
"./assets/environment/cubemap/first/ny.png",
"./assets/environment/cubemap/first/pz.png",
"./assets/environment/cubemap/first/nz.png"
};
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 第三人称视角和结局相机
Camera* camera = new Camera();
// 光源1，用来稍微照亮场景，调整场景的明暗
Light* light = new Light();

// 天空盒绘制
MeshPainter* skybox_painter = new MeshPainter();

Object *monster = new Object();


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


GLuint skyboxTexture;// 天空盒纹理
void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	// 设置光源位置
	light->setTranslation(glm::vec3(5.0, 50.0, 2.0));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	light->setAttenuation(0.5, 0.001, 0.001); // 衰减系数
    // 加载模型
    monster->rotation = glm::vec3(0.0, 0.0, 0.0);
    monster->scale = glm::vec3(0.4, 0.4, 0.4);
    monster->position = glm::vec3(0.0, 0.0, 0.0);
    monster->buildModel("./assets/donghuatest/dancing_vampire.dae", "shaders/vshader.glsl", "shaders/fshader.glsl",false);
    monster->generateAnimator("./assets/donghuatest/dancing_vampire.dae", "walk", 0);
    // 加载天空盒

    skyboxTexture = skybox_painter->BindSkyBox(first_cubemap_faces);
	glClearColor(0.6, 0.6, 0.6, 1.0);
	// glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->updateCamera();// 更新相机
    // 绘制怪物
    monster->painter->drawModelWithAnimator(monster->animators["walk"],light,camera);

    // 绘制天空盒
    skybox_painter->drawSkyBox(camera,skybox_painter->getOpenGLObj()[0],skyboxTexture);

}


void printHelp()
{
	std::cout << "动画绘画展示" << std::endl;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (action == GLFW_PRESS) {
		switch (key)
		{
		    case GLFW_KEY_ESCAPE: exit(EXIT_SUCCESS); break;
		    case GLFW_KEY_H: printHelp(); break;
            case GLFW_KEY_W:break;
            case GLFW_KEY_T:
		    default:
                camera->keyboard(key, action, mode);
			break;
		}
	}
}
// 处理长按
void key_callback2(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    camera->keyboard(key, action, mode);

}
double lastX = 0.0;
double lastY = 0.0;
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->mousemove(xpos-lastX,ypos-lastY,WIDTH,HEIGHT);
    lastX = xpos;
    lastY = ypos;
}
void cleanData() {
	delete camera;
    camera = NULL;
	delete light;
	light = NULL;


	

}


int main(int argc, char** argv)
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
#pragma execution_character_set("utf-8")
    GLFWwindow *window = glfwCreateWindow(600, 600, "2021192009谢钺大作业", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    // 锁定鼠标到窗口中心
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
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

	while (!glfwWindowShouldClose(window))
	{
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        monster->animators["walk"]->UpdateAnimationRepeat(deltaTime);
        if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
            if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                camera->keyboard(GLFW_KEY_UP,GLFW_PRESS,GLFW_MOD_SHIFT);
            else
                camera->keyboard(GLFW_KEY_UP,GLFW_PRESS,0x0000);
        }
        else if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
            if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                camera->keyboard(GLFW_KEY_DOWN,GLFW_PRESS,GLFW_MOD_SHIFT);
            else
                camera->keyboard(GLFW_KEY_DOWN,GLFW_PRESS,0x0000);
        }
        if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS){
            camera->keyboard(GLFW_KEY_LEFT,GLFW_PRESS,0x0000);
        }
        else if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS){
            camera->keyboard(GLFW_KEY_RIGHT,GLFW_PRESS,0x0000);
        }
		display(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	cleanData();

	return 0;
}