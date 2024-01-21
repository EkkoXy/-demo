#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Angel.h"
#include "Model.h"
class Model;
class Camera
{
public:
	Camera();
    Camera(glm::vec4 eye,glm::vec4 look,glm::vec4 up);
	~Camera();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix( bool isOrtho );
    glm::vec4 getLook() const{return look;}
	glm::mat4 lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up);

	glm::mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);

	glm::mat4 perspective(const GLfloat fov, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar);

	glm::mat4 frustum(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);

	// 每次更改相机参数后更新一下相关的数值
	virtual void updateCamera();
	// 处理相机的键盘操作
    virtual void keyboard(int key, int action, int mode);
    void mousemove(int x, int y,int width,int height);
    // 修改相机
    void setCamera(glm::vec4 eye,glm::vec4 look);
    void setCamera(glm::vec4 eye);
	// 模视矩阵
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;



	// 相机位置参数
	float radius = 4.0;
	float rotateAngle = 0.0;
	float upAngle = 30.0;
	glm::vec4 eye = glm::vec4(5.0, 3.0, 2.0, 1.0);
    glm::vec4 look = glm::vec4(0.0,0.0,0.0,0.0);
	glm::vec4 at = glm::vec4(0.0, 0.0, 0.0, 1.0);
	glm::vec4 up;

	// 投影参数
	float zNear = 0.1;
	float zFar = 100.0;
	// 透视投影参数
	float fov = 45.0;
	float aspect = 1.0;
	// 正交投影参数
	float scale = 1.5;

};

class FirstPerspective: public Camera{
public:
    FirstPerspective() {updateCamera();}
    const float g = 0.98;// 重力加速度
    float currentA = 0.0;// 当前加速度
    float currentVy = 0.0;// 当前y方向速度
    void keyboard(int key, int action, int mode) override;
    FirstPerspective(glm::vec4 eye,glm::vec4 look,glm::vec4 up);
    void updateCamera() override;
    void updateEye();// 根据当前的速度更新eye
    void mousemove2(int x, int y,int width,int height);
    glm::vec4 getEye() const{return eye;}
};
#endif