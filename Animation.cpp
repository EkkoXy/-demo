//
// Created by Administrator on 2023-12-05.
//
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Animation.h"

Animation::Animation(const std::string &animationPath, Model *model,int index) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    if(scene->mNumAnimations == 0) {
        Duration = 0.0f;
        TicksPerSecond = 0.0f;
        return;
    }
    aiAnimation *animation = scene->mAnimations[index];
    Duration = animation->mDuration;
    TicksPerSecond = animation->mTicksPerSecond;
    ReadHeirarchyData(RootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
}

Bone *Animation::FindBone(const std::string &name) {
auto iter = std::find_if(Bones.begin(), Bones.end(), [&](const Bone& Bone) { return Bone.getName() == name; });
    if(iter == Bones.end()) {
        return nullptr;
    } else {
        return &(*iter);
    }
}

void Animation::ReadMissingBones(const aiAnimation *animation, Model &model) {
    int num = animation->mNumChannels;//通道数
    std::map<std::string, BoneInfo> boneInfoMap = model.getBoneInfoMap();
    int boneCounter = model.getBoneCounter();
    for(int i = 0; i < num; i++) {
        aiNodeAnim *channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;
        // 如果骨骼信息映射中没有该骨骼，添加该骨骼
        if(boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName].id = boneCounter;
            boneCounter++;
        }
        Bone bone(boneName, boneInfoMap[boneName].id, channel);
        Bones.push_back(bone);
    }
    this->BoneInfoMap = boneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src) {
    assert(src);
    // 读取节点数据
    dest.name = src->mName.data;
    dest.transformation = glm::transpose(glm::make_mat4(&src->mTransformation.a1));
    dest.childrenCount = src->mNumChildren;
    // 递归读取子节点
    for(int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

