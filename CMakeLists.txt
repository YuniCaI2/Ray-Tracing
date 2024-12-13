cmake_minimum_required(VERSION 3.29)
project(RayTrace)

set(CMAKE_CXX_STANDARD 20)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(RayTrace main.cpp
        Triangle.cpp
        BoundingBox.cpp
        OBJ_Loader.h
        BVH.cpp
        Model.cpp
        Model.h
        Ray.cpp
        Ray.h
        Texture.cpp
        Texture.h
        Triangle.h
        Renderer.cpp
        Renderer.h
        Scene.cpp
        Scene.h
        Intersection.h
        Light.h)



target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
