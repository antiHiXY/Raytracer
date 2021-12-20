#pragma once

#include "constants.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_env : public hittable
{
public:
    constant_env(std::shared_ptr<hittable> b, double d, std::shared_ptr<texture> a)
        : m_boundary(b)
        , m_neg_inv_density(-1 / d)
        , m_phase_function(std::make_shared<isotropic>(a))
    {}

    constant_env(std::shared_ptr<hittable> b, double d, color c)
        : m_boundary(b)
        , m_neg_inv_density(-1 / d)
        , m_phase_function(std::make_shared<isotropic>(c))
    {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        return m_boundary->bounding_box(time0, time1, output_box);
    }

public:
    std::shared_ptr<hittable> m_boundary;
    std::shared_ptr<material> m_phase_function;
    double m_neg_inv_density;
};

bool constant_env::hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const
{
    hit_record  hit_rec1, hit_rec2;

    if (!m_boundary->hit(r_in, -INF, INF, hit_rec1))
        return false;

    if (!m_boundary->hit(r_in, hit_rec1.m_t + 0.0001, INF, hit_rec2))
        return false;

    if (hit_rec1.m_t < t_min)  hit_rec1.m_t = t_min;
    if (hit_rec2.m_t > t_max)  hit_rec2.m_t = t_max;
             
    if (hit_rec1.m_t >= hit_rec2.m_t)
        return false;

    if (hit_rec1.m_t < 0)
        hit_rec1.m_t = 0;

    const auto ray_length = r_in.dir().length();
    const auto distance_inside_boundary = (hit_rec2.m_t - hit_rec1.m_t) * ray_length;
    const auto hit_distance = m_neg_inv_density * log(random_double());

    if (hit_distance > distance_inside_boundary)
        return false;

    hit_rec.m_t = hit_rec1.m_t + hit_distance / ray_length;
    hit_rec.m_point = r_in.at(hit_rec.m_t);

    hit_rec.m_normal = vec3(1, 0, 0);  // arbitrary
    hit_rec.m_front_face = true;     // also arbitrary
    hit_rec.m_mat_ptr = m_phase_function;

    return true;
}