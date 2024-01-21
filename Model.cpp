#include <chrono>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
# include "Model.h"
Model::Model()
{
    m_BoneCounter = 0;
    mesh_list.clear();
    texture_loaded.clear();
}
Model::~Model()
{
    for (int i = 0; i < mesh_list.size(); i++)
    {
        delete mesh_list[i];
    }
}
std::vector<TriMesh*> Model::getMeshList()
{
    return mesh_list;
}

std::vector<Texture> Model::getTextureLoaded()
{
    return texture_loaded;
}
std::map<std::string, BoneInfo> Model::getBoneInfoMap()
{
    return m_BoneInfoMap;
}
std::map<std::string, glm::mat4> Model::getBoneTransformMap()
{
    return m_BoneTransformMap;
}
// 使用assimp库读取gltf并存储到TriMesh中
void Model::readfile(const std::string& filename,MeshPainter *painter,const std::string vshader,const std::string fshader,bool isbound)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenNormals
                                                       | aiProcess_FlipUVs |aiProcess_GenBoundingBoxes|aiProcess_TransformUVCoords|
                                                        aiProcess_JoinIdenticalVertices |aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        exit(0);
    }
    // 单位矩阵
    aiMatrix4x4 identity;
    updateModelMatrix();
    processNode(scene->mRootNode, identity,
                filename,painter,vshader,fshader,scene,isbound);
    generateBoneTransformMap(scene->mRootNode, identity,scene);
}
void Model::generateBoneTransformMap(aiNode* node, aiMatrix4x4 parentTransform,const aiScene* scene)
{
    aiMatrix4x4 transform = parentTransform * node->mTransformation;
    std::string nodeName = node->mName.C_Str();
    if(m_BoneInfoMap.find(nodeName) != m_BoneInfoMap.end())
    {
        transform = parentTransform * node->mTransformation;
        glm::mat4 totalTransform = glm::mat4(1.0f);
        totalTransform[0][0] = transform.a1;
        totalTransform[0][1] = transform.b1;
        totalTransform[0][2] = transform.c1;
        totalTransform[0][3] = transform.d1;
        totalTransform[1][0] = transform.a2;
        totalTransform[1][1] = transform.b2;
        totalTransform[1][2] = transform.c2;
        totalTransform[1][3] = transform.d2;
        totalTransform[2][0] = transform.a3;
        totalTransform[2][1] = transform.b3;
        totalTransform[2][2] = transform.c3;
        totalTransform[2][3] = transform.d3;
        totalTransform[3][0] = transform.a4;
        totalTransform[3][1] = transform.b4;
        totalTransform[3][2] = transform.c4;
        totalTransform[3][3] = transform.d4;
        m_BoneTransformMap[nodeName] = totalTransform;
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        generateBoneTransformMap(node->mChildren[i], transform,scene);
    }
}

void Model::processNode(aiNode* node, aiMatrix4x4 parentTransform,const std::string& filename,MeshPainter *painter,
                          const std::string& vshader,const std::string& fshader,const aiScene* scene,bool isbound)
{
    //std::cout << node->mName.C_Str() << std::endl;
    aiMatrix4x4 transform = parentTransform * node->mTransformation;
    glm::mat4 totalTransform = glm::mat4(0.0f);
    totalTransform[0][0] = transform.a1;
    totalTransform[0][1] = transform.b1;
    totalTransform[0][2] = transform.c1;
    totalTransform[0][3] = transform.d1;
    totalTransform[1][0] = transform.a2;
    totalTransform[1][1] = transform.b2;
    totalTransform[1][2] = transform.c2;
    totalTransform[1][3] = transform.d2;
    totalTransform[2][0] = transform.a3;
    totalTransform[2][1] = transform.b3;
    totalTransform[2][2] = transform.c3;
    totalTransform[2][3] = transform.d3;
    totalTransform[3][0] = transform.a4;
    totalTransform[3][1] = transform.b4;
    totalTransform[3][2] = transform.c4;
    totalTransform[3][3] = transform.d4;
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        auto new_mesh = TriMesh::generateTriMesh_by_aiMesh(scene->mMeshes[node->mMeshes[i]],scene,this,isbound,totalTransform,filename,painter);
        mesh_list.push_back(new_mesh);
        painter->addMesh(new_mesh,"new_mesh",vshader,fshader);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], transform,filename,painter,vshader,fshader,scene,isbound);
    }
}

int Model::getBoneCounter() {
    return m_BoneCounter;
}
void Model::pushBoundingBox(glm::vec3 min,glm::vec3 max) {
    bounding_box_min.push_back(min);
    bounding_box_max.push_back(max);
    bounding_box_counter++;
}
glm::vec3 Model::getRotation() {
    return rotation;
}

glm::vec3 Model::getScale() {
    return scale;
}

glm::vec3 Model::getPosition() {
    return position;
}
void Model::setRotation(glm::vec3 rotation) {
    this->rotation = rotation;
}

void Model::setScale(glm::vec3 scale) {
    this->scale = scale;
}
void Model::setPosition(glm::vec3 position) {
    this->position = position;
}

glm::vec3 Model::getBoundingBoxMin(int index) {
    return bounding_box_min[index];
}

glm::vec3 Model::getBoundingBoxMax(int index) {
    return bounding_box_max[index];
}
int Model::getBoundingBoxCounter() {
    return bounding_box_counter;
}

bool Model::CheckBoundingBox(glm::vec3 position) {
    if(position.y < 0.0f) {
        return false;
    }
    for(int i = 0; i < bounding_box_counter; i++) {
        if(position.x >= bounding_box_min[i].x && position.x <= bounding_box_max[i].x &&
           position.y >= bounding_box_min[i].y && position.y <= bounding_box_max[i].y &&
           position.z >= bounding_box_min[i].z && position.z <= bounding_box_max[i].z) {
            return false;
        }
    }
    return true;
}
std::vector<glm::vec3> Model::bounding_box_min = std::vector<glm::vec3>();
std::vector<glm::vec3> Model::bounding_box_max = std::vector<glm::vec3>();
int Model::bounding_box_counter = 0;

void Model::updateModelMatrix() {
    for(int i = 0; i < mesh_list.size(); i++) {
        mesh_list[i]->setTranslation(position);
        mesh_list[i]->setScale(scale);
        mesh_list[i]->setRotation(rotation);
    }
}

bool Model::CheckBoundingBox(glm::vec3 min, glm::vec3 max) {
    for(int i = 0; i < bounding_box_counter; i++) {
        if(min.x <= bounding_box_max[i].x && max.x >= bounding_box_min[i].x &&
           min.y <= bounding_box_max[i].y && max.y >= bounding_box_min[i].y &&
           min.z <= bounding_box_max[i].z && max.z >= bounding_box_min[i].z) {
            return false;
        }
    }
    return true;
}

void Model::setBoneCounter(int counter) {
    m_BoneCounter = counter;
}

bool Model::findBoneInfoMap(std::string key) {
    if(m_BoneInfoMap.find(key) != m_BoneInfoMap.end()) {
        return true;
    }
    return false;
}

void Model::setBoneInfoMap(std::string key, BoneInfo value) {
    m_BoneInfoMap[key] = value;
}

void Sun::setAttenuation(float _constant, float _linear, float _quadratic) {
    constant = _constant;
    linear = _linear;
    quadratic = _quadratic;
}

float Sun::getConstant() {
    return constant;
}

float Sun::getLinear() {
    return linear;
}

float Sun::getQuadratic() {
    return quadratic;
}

void Sun::setAmbient(glm::vec4 _ambient) {
    ambient = _ambient;
}
void Sun::setDiffuse(glm::vec4 _diffuse) {
    diffuse = _diffuse;
}
void Sun::setSpecular(glm::vec4 _specular) {
    specular = _specular;
}
void Sun::setShininess(float _shininess) {
    shininess = _shininess;
}
