#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "constant_env.h"
#include "constants.h"
#include "hittable_objects.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"

#include <iostream>
#include <fstream>

color ray_color(const ray& r_in, const color& background, const hittable& world, int depth)
{
    hit_record hit_rec;

    if (depth <= 0)
        return color(0.f, 0.f, 0.f);

    if (!world.hit(r_in, 0.001, INF, hit_rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = hit_rec.m_mat_ptr->emitted(hit_rec.m_u, hit_rec.m_v, hit_rec.m_point);

    if (!hit_rec.m_mat_ptr->scatter(r_in, hit_rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

hittable_objects materials_scene()
{
    hittable_objects world;

    auto material_ground = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));

    auto material_left = std::make_shared<lambertian>(color(0.7, 0.2, 0.5));
    auto material_center = std::make_shared<dielectric>(1.5);
    auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2));

    world.add(std::make_shared<sphere>(point3(0.f, -100, 0.f), 100.0, material_ground));

    world.add(std::make_shared<sphere>(point3(0.f, 1.0, 0.f), 1.0, material_center));
    world.add(std::make_shared<sphere>(point3(-4.0, 1.0, 0.5), 1.0, material_left));
    world.add(std::make_shared<sphere>(point3(4.0, 1.0, -0.5), 1.0, material_right));

    return world;
}

hittable_objects cornell_box_with_smokes()
{
    hittable_objects objects;

    auto red   = std::make_shared<lambertian>(color(.65, .05, .05));
    auto white = std::make_shared<lambertian>(color(.73, .73, .73));
    auto green = std::make_shared<lambertian>(color(.12, .45, .15));
    auto light = std::make_shared<diffuse_light>(color(15, 15, 15));

    std::shared_ptr<hittable> box1 = std::make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
         box1 = std::make_shared<rotate_y>(box1, 15);
         box1 = std::make_shared<translate>(box1, vec3(265, 0, 295));

    std::shared_ptr<hittable> box2 = std::make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
         box2 = std::make_shared<rotate_y>(box2, -18);
         box2 = std::make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<constant_env>(box1, 0.01, color(0, 0, 0)));
    objects.add(std::make_shared<constant_env>(box2, 0.01, color(1, 1, 1)));

    return objects;
}

hittable_objects final_scene()
{
    hittable_objects boxes;
    auto white = std::make_shared<lambertian>(color(.73, .73, .73));

    auto ground = std::make_shared<lambertian>(color(0.9, 0.13, 0.23));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes.add(std::make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_objects objects;

    objects.add(std::make_shared<bvh_node>(boxes, 0, 1));

    auto light = std::make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(std::make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    std::shared_ptr<hittable> box1 = std::make_shared<box>(point3(0, 0, 0), point3(100, 200, 100), white);
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, vec3(100, 150, 105));

    objects.add(std::make_shared<sphere>(point3(260, 150, 45), 50, std::make_shared<lambertian>(color(0.2, 0.8, 0.1))));
    objects.add(std::make_shared<sphere>(point3(0, 150, 145), 50, std::make_shared<metal>(color(0.8, 0.8, 0.4))));
    objects.add(std::make_shared<sphere>(point3(360, 150, 145), 70, std::make_shared<dielectric>(1.5)));
    objects.add(std::make_shared<constant_env>(box1, 0.01, color(0.5, 0.5, 0.5)));

    return objects;
}

int main()
{

    // Image
    double aspect_ratio = 3.0 / 2.0;
    int image_width = 800;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    int samples_per_pixel = 50;
    const int max_depth = 50;

    //World
    hittable_objects world;

    //Camera
    point3 lookfrom;
    point3 lookat;
    const vec3 vup(0, 1, 0);
    double vfov = 40.0;
    const double dist_to_focus = 10.0;
    double aperture = 0.0;
    color background(0, 0, 0);

    int num = 0;
    std::cout << "Choose type of scene:" << std::endl;
    std::cout << "  0 - materials:" << std::endl;
    std::cout << "  1 - cornell box with smokes:" << std::endl;
    std::cout << "  2 - final scene:" << std::endl;
    std::cin >> num;

    switch (num)
    {
        case 0:
            world = materials_scene();
            image_width = 800;
            image_height = static_cast<int>(image_width / aspect_ratio);
            samples_per_pixel = 100;
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        case 1:
            world = cornell_box_with_smokes();
            aspect_ratio = 1.0;
            image_width = 800;
            image_height = static_cast<int>(image_width / aspect_ratio);
            samples_per_pixel = 200;
            background = color(0, 0, 0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        case 2:
            world = final_scene();
            aspect_ratio = 1.0;
            image_width = 800;
            image_height = static_cast<int>(image_width / aspect_ratio);
            samples_per_pixel = 1000;
            background = color(0, 0, 0);
            lookfrom = point3(478, 278, -600);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        default:
            break;
    }
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

    // Render
    //std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    std::ofstream ofs("test.ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P3" << std::endl << image_width << ' ' << image_height << std::endl << "255" << std::endl;

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(ofs, pixel_color, samples_per_pixel);

        }
    }
    std::cerr << "\nDone.\n";
    ofs.close();

    return EXIT_SUCCESS;
}
