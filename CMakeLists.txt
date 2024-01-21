cmake_minimum_required(VERSION 3.26)
project(fooler)

set(CMAKE_CXX_STANDARD 17)

add_executable(fooler main.cpp
        InitShader.cpp
        glad.c
        include/Angel.h
        include/CheckError.h
        include/MeshPainter.h
        include/TriMesh.h
        include/Model.h
        include/Bone.h
        include/Animation.h
        include/Animator.h
        TriMesh.cpp
        Camera.cpp
        MeshPainter.cpp
        Model.cpp
        Bone.cpp
        Animation.cpp
        Animator.cpp
        include/Object.h
        Object.cpp
)
include_directories(include)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(fooler PRIVATE glfw)
find_package(glm CONFIG REQUIRED)
target_link_libraries(fooler PRIVATE glm::glm)
find_package(glad CONFIG REQUIRED)
target_link_libraries(fooler PRIVATE glad::glad)
find_package(assimp CONFIG REQUIRED)
target_link_libraries(fooler PRIVATE assimp::assimp)
