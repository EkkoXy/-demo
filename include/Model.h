//
// Created by Administrator on 2023-12-01.
//

#ifndef MODEL_H
#define MODEL_H
#include "MeshPainter.h"
#include "TriMesh.h"
class MeshPainter;
// 处理层级建模的类
class Model
{
public:
    Model();
    ~Model();
    std::vector<TriMesh*> getMeshList();
    std::vector<Texture> getTextureLoaded();
    std::map<std::string, BoneInfo> getBoneInfoMap();
    bool findBoneInfoMap(std::string key);
    void setBoneInfoMap(std::string key,BoneInfo value);
    std::map<std::string, glm::mat4> getBoneTransformMap();
    glm::vec3 getRotation();
    glm::vec3 getScale();
    glm::vec3 getPosition();
    static glm::vec3 getBoundingBoxMin(int index);
    static glm::vec3 getBoundingBoxMax(int index);
    static int getBoundingBoxCounter();
    static bool CheckBoundingBox(glm::vec3 position);// 碰撞检测函数，检测单点
    static bool CheckBoundingBox(glm::vec3 min,glm::vec3 max);// 碰撞检测函数，检测区域
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);
    void setPosition(glm::vec3 position);
    void updateModelMatrix();// 根据model的位置，旋转，缩放更新模型矩阵
    void readfile(const std::string& filename,MeshPainter *painter,std::string vshader,std::string fshader,bool isbound);// 读取模型文件
    int getBoneCounter();
    void setBoneCounter(int counter);
    void pushBoundingBox(glm::vec3 min,glm::vec3 max);// 用于碰撞检测

private:
    static std::vector<glm::vec3> bounding_box_min; // 用于碰撞检测
    static std::vector<glm::vec3> bounding_box_max; // 用于碰撞检测
    static int bounding_box_counter;//  用于碰撞检测
    std::vector<TriMesh*> mesh_list;// 模型包括的mesh
    std::vector<Texture> texture_loaded;// 模型包含的纹理
    std::map<std::string, BoneInfo> m_BoneInfoMap;// 骨骼信息
    std::map<std::string, glm::mat4>  m_BoneTransformMap;// 骨骼变换矩阵
    glm::vec3 rotation;// 模型的旋转
    glm::vec3 scale;// 模型的缩放
    glm::vec3 position;// 模型的位置
    int m_BoneCounter ;// 骨骼数量
    void generateBoneTransformMap(aiNode* node, aiMatrix4x4 parentTransform,const aiScene* scene);// 生成骨骼变换矩阵
    void processNode(aiNode* node, aiMatrix4x4 parentTransform,const std::string& filename,MeshPainter *painter,
                            const std::string& vshader,const std::string& fshader,const aiScene* scene,bool isbound);
    // 处理assimp导入的节点

};
class Sun: public Model {
public:

    void setAttenuation(float _constant, float _linear, float _quadratic);
    float getConstant();
    float getLinear();
    float getQuadratic();
    void setAmbient(glm::vec4 _ambient );
    void setDiffuse(glm::vec4 _diffuse);
    void setSpecular(glm::vec4 _specular);
    void setShininess(float _shininess);
    glm::vec4 getAmbient() {return ambient;}
    glm::vec4 getDiffuse() {return diffuse;}
    glm::vec4 getSpecular() {return specular;}
    float getShininess() {return shininess;}
protected:
    // 光源衰减系数的三个参数
    float constant; // 常数项
    float linear;	// 一次项
    float quadratic;// 二次项
    glm::vec4 ambient;	// 环境光
    glm::vec4 diffuse;	// 漫反射
    glm::vec4 specular;	// 镜面反射
    float shininess;	// 高光系数
};
#endif //MODEL_H
