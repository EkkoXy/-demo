//
// Created by Administrator on 2023-12-09.
//

#ifndef FOOLER_OBJECT_H
#define FOOLER_OBJECT_H

#include "Model.h"
class Model;
#include "MeshPainter.h"
class MeshPainter;
#include "Animation.h"
class Animation;
#include "Animator.h"
class Animator;
class FirstPerspective;
class Object{
public:
    Object() {
        model = new Model();
        painter = new MeshPainter(model);
    }
    MeshPainter *painter;
    Model *model;
    std::map<std::string,Animation*> animations;
    std::map<std::string,Animator*> animators;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    void buildModel(const std::string &filename, const std::string &vshader, const std::string &fshader,bool isbound) ;
    void generateAnimator(const std::string &filename,std::string name,int index) ;
};
class Monster:public Object{
public:
    static FirstPerspective *firstcamera;
    Monster()   {
        model = new Model();
        painter = new MeshPainter(model);
    }
    bool wakeup = false;
    bool do_wakeup = false;
    bool attack = false;
    bool do_attack = false;
    bool run = false;
    bool do_run = false;
    float distance = 0.0;
    const float wakeup_distance = 6.0;
    const float attack_distance = 1.0;
    const float run_distance = 4.0;
    float v;
    void update(float dt);// 怪物逻辑具体实现
    void start();
};
#endif //FOOLER_OBJECT_H
