//
// Created by Administrator on 2023-12-05.
//

#ifndef FOOLER_ANIMATOR_H
#define FOOLER_ANIMATOR_H

#include "Animation.h"
class AssimpNodeData;
class Animation;
class Animator
{
public:
    Animator(Animation* animation);
    ~Animator() = default;
    void UpdateAnimationRepeat(float dt);//循环更新整段动画
    void UpdateAnimationRepeat(float dt,int startFrame,int endFrame);//循环更新指定帧动画
    bool UpdateAnimationNoRepeat(float dt);//更新整段动画
    bool UpdateAnimationNoRepeat(float dt,int startFrame,int endFrame);//更新指定帧动画
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);//计算骨骼变换
    std::vector<glm::mat4> GetFinalBoneMatrices(){ return FinalBoneMatrices; }
private:
    std::vector<glm::mat4> FinalBoneMatrices;//当前最终骨骼变换矩阵
    Animation* CurrentAnimation;//当前动画
    float CurrentTime;//当前时间
    float DeltaTime;
};
#endif //FOOLER_ANIMATOR_H
