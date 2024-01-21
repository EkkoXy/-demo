//
// Created by Administrator on 2023-12-05.
//
#include "Animator.h"

Animator::Animator(Animation *animation) {
    CurrentAnimation = animation;
    CurrentTime = 0.0f;
    FinalBoneMatrices.reserve(100);
    for(int i = 0; i < 100; i++) {
        FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::UpdateAnimationRepeat(float dt) {
    DeltaTime = dt;
    if(CurrentAnimation) {
        CurrentTime += CurrentAnimation->GetTicksPerSecond() * DeltaTime;
        CurrentTime = fmod(CurrentTime, CurrentAnimation->GetDuration() );
        CalculateBoneTransform(&CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}



void Animator::CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    Bone *bone = CurrentAnimation->FindBone(nodeName);
    if(bone) {
        bone->Update(CurrentTime);
        nodeTransform = bone->getLocalTransform();
    }
    glm::mat4 globalTransformation = parentTransform * nodeTransform;
    auto boneInfoMap = CurrentAnimation->GetBoneIDMap();
    if(boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        FinalBoneMatrices[index] = globalTransformation * offset;
    }
    for(int i = 0; i < node->childrenCount; i++) {
        CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}

bool Animator::UpdateAnimationNoRepeat(float dt) {
    if(CurrentTime >= CurrentAnimation->GetDuration()) {
        CurrentTime = CurrentAnimation->GetDuration();
        return false;
    }
    DeltaTime = dt;
    if(CurrentAnimation) {
        CurrentTime += CurrentAnimation->GetTicksPerSecond() * DeltaTime;
        if(CurrentTime >= CurrentAnimation->GetDuration()) {
            CurrentTime = 0;
            return false;
        }
        CurrentTime = fmod(CurrentTime, CurrentAnimation->GetDuration() );
        CalculateBoneTransform(&CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
    return true;
}

void Animator::UpdateAnimationRepeat(float dt, int startFrame, int endFrame) {
    DeltaTime = dt;
    const float duration = endFrame - startFrame;
    if(CurrentAnimation) {
        if(CurrentTime != 0){
            CurrentTime -= startFrame;
            CurrentTime += CurrentAnimation->GetTicksPerSecond() * DeltaTime;
            CurrentTime = fmod(CurrentTime, duration ) + startFrame;
        }
        else
        {
            CurrentTime = startFrame;
        }
        CalculateBoneTransform(&CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

bool Animator::UpdateAnimationNoRepeat(float dt, int startFrame, int endFrame) {
    DeltaTime = dt;
    const float duration = endFrame - startFrame;
    if(CurrentAnimation) {
        if(CurrentTime != 0){
            CurrentTime -= startFrame;
            CurrentTime += CurrentAnimation->GetTicksPerSecond() * DeltaTime;
            if(CurrentTime >= duration){
                CurrentTime = 0;
                return false;
            }
            CurrentTime = fmod(CurrentTime, duration ) + startFrame;
        }
        else
        {
            CurrentTime = startFrame;
        }
        CalculateBoneTransform(&CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
    return true;
}

