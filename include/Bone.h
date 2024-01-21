//
// Created by Administrator on 2023-12-05.
//

#ifndef FOOLER_BONE_H
#define FOOLER_BONE_H
# include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/detail/type_quat.hpp>
#include <vector>
#include <string>
#include <assimp/anim.h>
// 关键帧位置
struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};
// 关键帧旋转
struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};
// 关键帧缩放
struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

struct Bone
{
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);//构造函数
    void Update(float animationTime);//更新骨骼
    int GetPositionIndex(float animationTime);//获取位置索引
    int GetRotationIndex(float animationTime);//获取旋转索引
    int GetScaleIndex(float animationTime);//获取缩放索引
    std::string getName() const { return name; }//获取名字
    void setName(std::string val) { name = val; }
    int getID() const { return ID; }
    void setID(int val) { ID = val; }
    glm::mat4 getLocalTransform() const { return LocalTransform; }//获取本地变换矩阵
private:
    std::string name;//名字
    int ID;//ID
    glm::mat4 LocalTransform;//本地变换矩阵
    std::vector<KeyPosition> keyPositions;//关键帧位置
    std::vector<KeyRotation> keyRotations;//关键帧旋转
    std::vector<KeyScale> keyScales;//关键帧缩放
    int numPositions;
    int numRotations;
    int numScalings;
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);//计算缩放因子
    glm::mat4 InterpolatePosition(float animationTime);//插值计算位置
    glm::mat4 InterpolateRotation(float animationTime);//插值计算旋转
    glm::mat4 InterpolateScaling(float animationTime);//插值计算缩放
};
#endif //FOOLER_BONE_H
