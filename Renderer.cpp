//
// Created by cai on 24-12-11.
//

#include "Renderer.h"
#include "Ray.h"

constexpr float deg2rad(float deg)
{
    return deg * M_PI / 180.0f;
}

Renderer::Renderer(const Scene &scene) {
    std::vector<Eigen::Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Eigen::Vector3f eye_pos(0, 0, 15);
    int m = 0;
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            float x = (2.0f * (i + 0.5f) / scene.width - 1.0f) * imageAspectRatio * scale;
            float y = (1.0f - 2.0f * (j + 0.5f) / scene.height) * scale;
            // TODO: Find the x and y positions of the current pixel to get the
            // direction
            //  vector that passes through it.
            // Also, don't forget to multiply both of them with the variable
            // *scale*, and x (horizontal) variable with the *imageAspectRatio*
            Eigen::Vector3f dir = Eigen::Vector3f(x,y,-1);
            dir = dir.normalized();
            framebuffer[m++] = scene.castRay(Ray(eye_pos,dir,0.0f),0);
            // Don't forget to normalize this direction!

        }
    }

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = static_cast<unsigned char>(255 * std::clamp(framebuffer[i].x(), 0.0f, 1.0f));
        color[1] = static_cast<unsigned char>(255 * std::clamp(framebuffer[i].y(), 0.0f, 1.0f));
        color[2] = static_cast<unsigned char>(255 * std::clamp(framebuffer[i].z(), 0.0f, 1.0f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);

}
