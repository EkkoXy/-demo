//
// Created by Administrator on 2023-12-05.
//
#include <glm/gtc/quaternion.hpp>
#include "Bone.h"

Bone::Bone(const std::string &name, int ID, const aiNodeAnim *channel) {
    this->name = name;
    this->ID = ID;
    this->LocalTransform = glm::mat4(1.0f);
    this->numPositions = channel->mNumPositionKeys;
    // 将aiVectorKey转换为KeyPosition
    for(int i = 0; i < numPositions; i++) {
        aiVectorKey key = channel->mPositionKeys[i];
        KeyPosition kp;
        kp.position = glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
        kp.timeStamp = key.mTime;
        keyPositions.push_back(kp);
    }
    this->numRotations = channel->mNumRotationKeys;
    // 将aiQuatKey转换为KeyRotation
    for(int i = 0; i < numRotations; i++) {
        aiQuatKey key = channel->mRotationKeys[i];
        KeyRotation kr;
        kr.orientation = glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
        kr.timeStamp = key.mTime;
        keyRotations.push_back(kr);
    }
    this->numScalings = channel->mNumScalingKeys;
    // 将aiVectorKey转换为KeyScale
    for(int i = 0; i < numScalings; i++) {
        aiVectorKey key = channel->mScalingKeys[i];
        KeyScale ks;
        ks.scale = glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
        ks.timeStamp = key.mTime;
        keyScales.push_back(ks);
    }
}

void Bone::Update(float animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    LocalTransform = translation * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime) {
    for(int i = 0; i < numPositions - 1; i++) {
        if(animationTime < keyPositions[i + 1].timeStamp) {
            return i;
        }
    }
    //如果动画时间大于最后一个关键帧的时间，返回倒数第二个关键帧的索引
    return numPositions - 2;
}

int Bone::GetRotationIndex(float animationTime) {
    for(int i = 0; i < numRotations - 1; i++) {
        if(animationTime < keyRotations[i + 1].timeStamp) {
            return i;
        }
    }
    //如果动画时间大于最后一个关键帧的时间，返回倒数第二个关键帧的索引
    return numPositions - 2;
}

int Bone::GetScaleIndex(float animationTime) {
    for(int i = 0; i < numScalings - 1; i++) {
        if(animationTime < keyScales[i + 1].timeStamp) {
            return i;
        }
    }
    //如果动画时间大于最后一个关键帧的时间，返回倒数第二个关键帧的索引
    return numPositions - 2;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    // 如果只有一个关键帧，直接返回
    if(numPositions == 1) {
        return glm::translate(glm::mat4(1.0f), keyPositions[0].position);
    }
    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(keyPositions[p0Index].timeStamp, keyPositions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(keyPositions[p0Index].position, keyPositions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if(numRotations == 1) {
        auto rotation = glm::normalize(keyRotations[0].orientation);
        // 将四元数转换为旋转矩阵
        return glm::mat4_cast(rotation);
    }
    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(keyRotations[p0Index].timeStamp, keyRotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(keyRotations[p0Index].orientation, keyRotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::mat4_cast(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime) {
    if(numScalings == 1) {
        return glm::scale(glm::mat4(1.0f), keyScales[0].scale);
    }
    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(keyScales[p0Index].timeStamp, keyScales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(keyScales[p0Index].scale, keyScales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

