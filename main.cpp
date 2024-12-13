#include <iostream>

#include "Renderer.h"
#include "Scene.h"
#include "Model.h"
#include "Light.h"

int main()
{
    Scene scene(1280, 960);

    std::string TexturePath1 = "/Users/yunicai/Code_file/code/GAMES101-Homework/Assignment3/Assignment3/Code/models/spot/spot_texture.png";
    std::string ModelPath1 = "/Users/yunicai/Code_file/code/GAMES101-Homework/Assignment3/Assignment3/Code/models/spot/spot_triangulated_good.obj";
    std::string ModelPath2 = "/Users/yunicai/Code_file/code/GAMES101-Homework/Assignment3/Assignment3/Code/models/bunny/bunny.obj";
    std::string ModelPath3 = "/Users/yunicai/Code_file/code/GAMES101-Homework/Assignment3/Assignment3/Code/models/rock/rock.obj";
    std::string ModelPath7 = "/Users/yunicai/Model/blue-archive-sunohara-kokona/cocona.obj";
    std::string TexturePath3 = "/Users/yunicai/Code_file/code/GAMES101-Homework/Assignment3/Assignment3/Code/models/rock/rock.png";
    std::string TexturePath2 = "/Users/yunicai/Code_file/code/computer_graphic_homework/Raster/image/img.png";
    Model model1;
    Texture texture1(TexturePath1);
    model1.LoadModel(ModelPath1);
    model1.SetTexture(texture1);
    model1.SetRotation(Eigen::Vector3f(0.f, 1.f, 0.f),30);
    model1.SetPosition(0, 0, -5.f);
    model1.SetScale(Eigen::Vector3f(4, 4, 4));
    model1.GetModelMatrix();


    Model model3;
    Texture texture3(TexturePath3);
    model3.LoadModel(ModelPath3);
    model3.SetTexture(texture3);
    model3.SetRotation(Eigen::Vector3f(0.f, 1.f, 0.f),30);
    model3.SetPosition(-10.f, -10.f, -6.f);
    model3.SetScale(Eigen::Vector3f(4, 4, 4));
    model3.GetModelMatrix();

    Model model2;
    Texture texture2(TexturePath2);
    model2.LoadModel(ModelPath2);
    model2.SetTexture(texture3);
    model2.SetRotation(Eigen::Vector3f(0.f, 1.f, 0.f),0);
    model2.SetScale(Eigen::Vector3f(20, 20, 20));
    model2.SetPosition(0.f, -2.f, -5.f);
    model2.SetTexture(texture2);
    model2.GetModelMatrix();

    Model model4;
    Texture texture4(TexturePath3);
    model4.LoadModel(ModelPath3);
    model4.SetTexture(texture3);
    model4.SetRotation(Eigen::Vector3f(0.f, 1.f, 0.f),30);
    model4.SetPosition(10.f, -10.f, -5.f);
    model4.SetScale(Eigen::Vector3f(4, 4, 4));
    model4.GetModelMatrix();

    Model model5;
    model5.LoadModel(ModelPath3);
    model5.SetTexture(texture3);
    model5.SetRotation(Eigen::Vector3f(0.f, 1.f, 0.f),30);
    model5.SetPosition(0.f, -10.f, 0.f);
    model5.SetScale(Eigen::Vector3f(4, 4, 4));
    model5.GetModelMatrix();

    Model model6;
    model6.LoadModel(ModelPath3);
    model6.SetTexture(texture3);
    model6.SetRotation(Eigen::Vector3f(0.f, -5.f, 0.f),30);
    model6.SetPosition(0.f, -10.f, -10.f);
    model6.SetScale(Eigen::Vector3f(4, 4, 4));
    model6.GetModelMatrix();

    Model model7;
    model7.LoadModel(ModelPath7);
    model7.SetTexture(texture2);
    model7.SetRotation(Eigen::Vector3f(0.f, 1.f, 0.f),-45);
    model7.SetPosition(-4.f, -3.f, 0.f);
    model7.SetScale(Eigen::Vector3f(8, 8, 8));
    model7.GetModelMatrix();




    Light light1(Eigen::Vector3f(-20.f, 70.f, 20.f), Eigen::Vector3f(1.0f, 1.0f, 1.0f));
    Light light2(Eigen::Vector3f(20.f, 70.f, 20.f), Eigen::Vector3f(1.0f, 1.0f, 1.0f));

    std::vector<Model*> models;
    std::vector<Light*> lights;
    models.push_back(&model1);
    models.push_back(&model2);
    models.push_back(&model3);
    models.push_back(&model4);
    models.push_back(&model5);
    models.push_back(&model6);
    models.push_back(&model7);
    lights.push_back(&light1);
    lights.push_back(&light2);
    scene.fov = 90;
    scene.AddModel(models);
    scene.AddLight(lights);
    scene.maxDepth = 2;
    scene.BuildBVH();

    Renderer r(scene);


    return 0;

}
