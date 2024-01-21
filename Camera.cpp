#include "Camera.h"

Camera::Camera() { updateCamera(); };
Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix()
{
	return this->lookAt(eye, at, up);
}

glm::mat4 Camera::getProjectionMatrix(bool isOrtho)
{
	if (isOrtho) {
		return this->ortho(-scale, scale, -scale, scale, this->zNear, this->zFar);
	}
	else {
		return this->perspective(fov, aspect, this->zNear, this->zFar);
	}
}

glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	// use glm.
	glm::vec3 eye_3 = eye;
	glm::vec3 at_3 = at;
	glm::vec3 up_3 = up;

	glm::mat4 view = glm::lookAt(eye_3, at_3, up_3);

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
    if(look == glm::vec4(0.0, 0.0, 0.0, 0.0)){
        look = eye - at;
        //规范化为1
        look = glm::normalize(look);
    }
    at = eye - look;
    // 根据look的方向计算此时的rotateAngle
    if (look.x == 0.0){
        if (look.z > 0) rotateAngle = 90.0;
        else rotateAngle = -90.0;
    }
    else rotateAngle = atan2(look.z, look.x) * 180 / M_PI;
    // 根据look的方向计算此时的upAngle
    upAngle = atan2(look.y , sqrt(look.x * look.x + look.z * look.z)) * 180 / M_PI;
	up = glm::vec4(0.0, 1.0, 0.0, 0.0);
    // 根据upAngle的正负来确定up的方向
	if (upAngle > 90) up.y = 1;
	else if (upAngle < -90) up.y = -1;
}


void Camera::keyboard(int key, int action, int mode)
{
	// 用键盘的上下左右键控制相机在x-z平面的移动，按下shift+上下键会在y轴上移动
    // 灵敏度
    float sensitivity = 0.01;
    if(key == GLFW_KEY_UP && action == GLFW_PRESS && mode == 0x0000){
        eye.x -= sensitivity * cos(rotateAngle * M_PI / 180.0);
        eye.z -= sensitivity * sin(rotateAngle * M_PI / 180.0);
    }
    else if(key == GLFW_KEY_UP && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT){
        eye.y += sensitivity;
    }
    else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && mode == 0x0000){
        eye.x += sensitivity * cos(rotateAngle * M_PI / 180.0);
        eye.z += sensitivity * sin(rotateAngle * M_PI / 180.0);
    }
    else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT){
        eye.y -= sensitivity;
    }
    else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && mode == 0x0000){
        eye.x -= sensitivity * cos((rotateAngle - 90) * M_PI / 180.0);
        eye.z -= sensitivity * sin((rotateAngle - 90) * M_PI / 180.0);
    }
    else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && mode == 0x0000){
        eye.x -= sensitivity * cos((rotateAngle + 90) * M_PI / 180.0);
        eye.z -= sensitivity * sin((rotateAngle + 90) * M_PI / 180.0);
    }
    // 按下空格重置为初始状态，eye在(0, 1.5, 1.0)，at在(0, 0, 0)，up在(0, 1, 0)
    else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS && mode == 0x0000){
        eye = glm::vec4(4.0, 0.0, 0.0, 1.0);
        look = glm::vec4(0.0,0.0,0.0,0.0);
        at = glm::vec4(0.0, 0.0, 0.0, 1.0);
    }

}
void FirstPerspective::keyboard(int key, int action, int mode)
{
    //std::cout<<key<<std::endl;
    // 用键盘的上下左右键控制相机在x-z平面的移动，按下shift+上下键会在y轴上移动
    // 灵敏度
    float sensitivity = 0.1;
    glm::vec4 tmp = eye;
    if(key == GLFW_KEY_W && action == GLFW_PRESS && mode == 0x0000){
        eye.x -= sensitivity * cos(rotateAngle * M_PI / 180.0);
        eye.z -= sensitivity * sin(rotateAngle * M_PI / 180.0);
    }
    else if(key == GLFW_KEY_UP && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT){
        eye.y += sensitivity;
    }
    else if(key == GLFW_KEY_S && action == GLFW_PRESS && mode == 0x0000){
        eye.x += sensitivity * cos(rotateAngle * M_PI / 180.0);
        eye.z += sensitivity * sin(rotateAngle * M_PI / 180.0);
    }
    else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT){
        eye.y -= sensitivity;
    }
    else if(key == GLFW_KEY_A && action == GLFW_PRESS && mode == 0x0000){
        eye.x -= sensitivity * cos((rotateAngle - 90) * M_PI / 180.0);
        eye.z -= sensitivity * sin((rotateAngle - 90) * M_PI / 180.0);
    }
    else if(key == GLFW_KEY_D && action == GLFW_PRESS && mode == 0x0000){
        eye.x -= sensitivity * cos((rotateAngle + 90) * M_PI / 180.0);
        eye.z -= sensitivity * sin((rotateAngle + 90) * M_PI / 180.0);
    }
        // 按下空格重置为初始状态，eye在(0, 1.5, 1.0)，at在(0, 0, 0)，up在(0, 1, 0)
    else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS && mode == 0x0000){
        currentVy = 7.0;
    }
    if(!Model::CheckBoundingBox(eye-glm::vec4(0.0,0.2,0.0,0.0))){
        eye = tmp;
    }

}

FirstPerspective::FirstPerspective(glm::vec4 eye, glm::vec4 look, glm::vec4 up) : Camera(eye, look, up) {
    this->eye = eye;
    this->look = look;
    this->up = up;
    currentVy = 0.0;
    updateCamera();
}

void FirstPerspective::updateCamera() {
    if(look == glm::vec4(0.0, 0.0, 0.0, 0.0)){
        look = eye - at;
        //规范化为1
        look = glm::normalize(look);
    }
    updateEye();
    at = eye - look;
    // 根据look的方向计算此时的rotateAngle
    if (look.x == 0.0){
        if (look.z > 0) rotateAngle = 90.0;
        else rotateAngle = -90.0;
    }
    else rotateAngle = atan2(look.z, look.x) * 180 / M_PI;
    // 根据look的方向计算此时的upAngle
    upAngle = atan2(look.y , sqrt(look.x * look.x + look.z * look.z)) * 180 / M_PI;
    up = glm::vec4(0.0, 1.0, 0.0, 0.0);
    // 根据upAngle的正负来确定up的方向
    if (upAngle > 90) up.y = 1;
    else if (upAngle < -90) up.y = -1;
}

void FirstPerspective::updateEye() {
    glm::vec4 tmp = eye;
    currentVy -= g * 0.5;
    eye.y += currentVy * 0.01;
    if(!Model::CheckBoundingBox(eye-glm::vec4(0.0,0.5,0.0,0.0))){
        eye = tmp;
        currentVy = 0.0;
    }
}

//鼠标朝向为look的方向
void Camera::mousemove(int x, int y,int width,int height)
{
    double dx = x;
    double dy = y;
    dx /= width / 2;
    dy /= height / 2;
    double sensitivity = 5.0;
    upAngle += sensitivity * dy;
    if(upAngle > 89) upAngle = 89;
    else if(upAngle < -89) upAngle = -89;
    rotateAngle += sensitivity * dx;
    if(rotateAngle > 360) rotateAngle -= 360;
    else if(rotateAngle < 0) rotateAngle += 360;
    // 根据两个angle计算此时的look
    look.x = cos(upAngle * M_PI / 180.0) * cos(rotateAngle * M_PI / 180.0);
    look.y = sin(upAngle * M_PI / 180.0);
    look.z = cos(upAngle * M_PI / 180.0) * sin(rotateAngle * M_PI / 180.0);
    // 规范化为1
    look = glm::normalize(look);
    //std::cout<<look.x<<" "<<look.y<<" "<<look.z<<std::endl;
}
void FirstPerspective::mousemove2(int x, int y, int width, int height) {
    double dx = x;
    double dy = y;
    dx /= width / 2;
    dy /= height / 2;
    double sensitivity = 5.0;
    upAngle += sensitivity * dy;
    if(upAngle > 89) upAngle = 89;
    else if(upAngle < -89) upAngle = -89;
    rotateAngle += sensitivity * dx;
    if(rotateAngle > 360) rotateAngle -= 360;
    else if(rotateAngle < 0) rotateAngle += 360;
    // 根据两个angle计算此时的look
    look.x = cos(upAngle * M_PI / 180.0) * cos(rotateAngle * M_PI / 180.0);
    look.y = sin(upAngle * M_PI / 180.0);
    look.z = cos(upAngle * M_PI / 180.0) * sin(rotateAngle * M_PI / 180.0);
    // 规范化为1
    look = glm::normalize(look);
    //std::cout<<look.x<<" "<<look.y<<" "<<look.z<<std::endl;
}
Camera::Camera(glm::vec4 eye, glm::vec4 look, glm::vec4 up) {
    this->eye = eye;
    this->look = look;
    this->up = up;
    updateCamera();
}

void Camera::setCamera(glm::vec4 eye, glm::vec4 look) {
    this->eye = eye;
    this->look = look;
    updateCamera();
}
void Camera::setCamera(glm::vec4 eye) {
    this->eye = eye;
    updateCamera();
}
