#ifndef _MESH_PAINTER_H_
#define _MESH_PAINTER_H_

#include "TriMesh.h"
#include "Angel.h"
#include "Model.h"
#include "Camera.h"
#include "Animator.h"

#include <vector>


struct openGLObject
{
	// 顶点数组对象
	GLuint vao;
	// 顶点缓存对象
	GLuint vbo;


	// 着色器文件
	std::string vshader;
	std::string fshader;
	// 着色器变量
	GLuint pLocation;
	GLuint cLocation;
	GLuint nLocation;
	GLuint tLocation;
    GLuint boneIDLocation;// 骨骼id
    GLuint boneWeightLocation;// 骨骼权重

	// 投影变换变量
    GLuint shadowMatrixLocation;
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;
    GLuint finalBoneMatricesLocation;
	// 阴影变量
	GLuint shadowLocation;
    GLuint textureLocation;

    static GLuint program;
    static GLuint program2;
};
class Model;
class Animator;
class Camera;
class Sun;
class MeshPainter
{

public:
    MeshPainter(Model *model);
    MeshPainter();
    ~MeshPainter();
    std::vector<std::string> getMeshNames();// 获取物体名称
    std::vector<TriMesh *> getMeshes();// 获取物体
    std::vector<openGLObject> getOpenGLObj();// 获取物体的opengl对象
    void bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera);// 绑定光照和材质
    void bindObjectAndData(TriMesh *mesh, openGLObject &object, const std::string &vshader,
                           const std::string &fshader);// 绑定物体和数据
    void addMesh( TriMesh* mesh, const std::string &name, const std::string &vshader, const std::string &fshader );// 添加物体
    // 绘制物体
    void drawMeshWithAnimator(TriMesh* mesh, Animator *animator,openGLObject &object, Light *light, Camera* camera);
    void drawMesh(TriMesh* mesh, openGLObject &object, Light *light, Camera* camera);
    // 绘制模型
    void drawModelWithAnimator(Animator *animator,Light *light, Camera* camera);
    void drawModel(Light *light, Camera* camera);
    void drawModelWithoutShadow(Light *light, Camera* camera);
    void cleanMeshes();// 清除物体
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string file_path);// 加载材质纹理
    unsigned int CubeTexture(std::vector<std::string> faces);// 加载天空盒纹理
    void drawSkyBox(Camera* camera, openGLObject &object,GLuint texture);// 绘制天空盒
    int BindSkyBox(std::vector<std::string> faces);// 绑定天空盒
private:
    void drawShadow(TriMesh *mesh, openGLObject &object, Light *light, Camera *camera);// 绘制阴影
    Model *model;// 模型
    std::vector<std::string> mesh_names;// 物体名称
    std::vector<TriMesh *> meshes;// 物体
    std::vector<openGLObject> opengl_objects;// 物体的opengl对象
    static GLuint TextureFromFile(const char* path, const std::string& directory);// 从文件中加载纹理
};

#endif