//
// Created by Administrator on 2023-12-05.
//

#ifndef FOOLER_ANIMATION_H
#define FOOLER_ANIMATION_H
# include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Model.h"
#include "Bone.h"
class Bone;
struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};
class Animation
{
public:
    Animation() = default;
    Animation(const std::string& animationPath, Model* model,int index);//构造函数
    ~Animation()= default;
    Bone* FindBone(const std::string& name);//查找骨骼
    inline float GetTicksPerSecond() { return TicksPerSecond; }
    inline float GetDuration() { return Duration;}
    inline const AssimpNodeData& GetRootNode() { return RootNode; }
    inline const std::map<std::string,BoneInfo>& GetBoneIDMap() { return BoneInfoMap; }
    inline const std::vector<Bone>& GetBones() { return Bones; }
private:
    void ReadMissingBones(const aiAnimation* animation, Model& model);//读取缺失的骨骼
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);//读取层次数据
    float Duration;//持续时间
    int TicksPerSecond; //每秒的帧数
    std::vector<Bone> Bones;//骨骼
    AssimpNodeData RootNode;//根节点
    std::map<std::string, BoneInfo> BoneInfoMap;//骨骼信息映射
};
#endif //FOOLER_ANIMATION_H
