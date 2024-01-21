//
// Created by Administrator on 2023-12-09.
//

#include "Object.h"
void Object::buildModel(const std::string &filename, const std::string &vshader,
                        const std::string &fshader,bool isbound)
{
    model->setPosition(position);
    model->setRotation(rotation);
    model->setScale(scale);
    model->readfile(filename, painter, vshader, fshader, isbound);
}
void Object::generateAnimator(const std::string &filename,std::string name,int index) {
    auto *animation = new Animation(filename, model, index);
    animations[name] = animation;
    auto *animator = new Animator(animation);
    animators[name] = animator;
}
FirstPerspective *Monster::firstcamera = new FirstPerspective();
void Monster::update(float dt) {
    // 怪物逻辑具体实现
    glm::vec3 eye = firstcamera->getEye();
    glm::vec3 position = model->getPosition();
    distance = glm::distance(position, eye);// 计算怪物和玩家的距离
    // 如果怪物没有被唤醒或者唤醒动画还没有播放
    if (!wakeup||!do_wakeup) {
        // 如果动画还没有播放，说明怪物还没有被唤醒
        if (!do_wakeup) {
            // 如果怪物和玩家的距离小于唤醒距离或者玩家捡到太阳，那么怪物被唤醒
            if (distance < wakeup_distance || wakeup) {
                do_wakeup = true;
                wakeup = false;
                return;
            }
        } else {
            // 播放完动画后才算被唤醒
            if (!animators["walk"]->UpdateAnimationNoRepeat(dt, 641, 814)) {
                wakeup = true;
                return;
            }
        }
    }
    // 如果怪物已经被唤醒
    else {
        // 如果没有进入攻击状态
        if (!do_run) {
            // 检测是否要进入攻击状态
            if (distance < run_distance || run) {
                do_run = true;
                return;
            }
            // 不用则播放警戒动画
            animators["walk"]->UpdateAnimationRepeat(dt, 190, 450);
        }
        // 进入攻击状态
        else {
            // 如果怪物和玩家的距离大于攻击距离，那么怪物向玩家移动
            if(!do_attack && distance >= attack_distance) {
                // 根据position和eye的位置调整rotation，即一直追踪玩家，面向玩家奔跑
                glm::vec3 direction = glm::vec3(eye.x - position.x, 0.0, eye.z - position.z);
                direction = glm::normalize(direction);
                float angle = glm::acos(glm::dot(glm::vec3(0.0, 0.0, -1.0), direction));
                if (direction.x < 0.0) {
                    angle = -angle;
                }
                // angle转化为度数
                angle = angle * 180.0 / glm::pi<float>();
                model->setRotation(glm::vec3(0.0, 180.0-angle, 0.0));
                position.x += direction.x * v;
                position.z += direction.z * v;
                model->setPosition(position);
                animators["walk"]->UpdateAnimationRepeat(dt, 815, 830);
            }
            // 如果怪物和玩家的距离小于攻击距离，那么怪物攻击玩家
            else {
                do_attack = true;
                if(!animators["walk"]->UpdateAnimationNoRepeat(dt, 982, 1013)) {
                    // 攻击动画结束后再次检测距离，看切换到跑步还是攻击
                    do_attack = false;
                }
            }
        }
    }
}
void Monster::start() {
    wakeup = true;
    run = true;
}