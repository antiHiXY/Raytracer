#pragma once
#include "aabb.h"
#include "constants.h"
#include "ray.h"

class material;

struct hit_record
{
    inline void set_face_normal(const ray& r_in, const vec3& outward_normal)
    {
        m_front_face = dot(r_in.dir(), outward_normal) < 0.f;
        m_normal = m_front_face ? outward_normal : -outward_normal;
    }

    point3 m_point;
    vec3   m_normal;
    std::shared_ptr<material> m_mat_ptr;
    double m_t;
    double m_u;
    double m_v;
    bool m_front_face;
};

class hittable
{
public:
    virtual bool hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const = 0;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
};

class translate : public hittable
{
public:
    translate(std::shared_ptr<hittable> p, const vec3& displacement)
        : m_ptr(p)
        , m_offset(displacement)
    {}

    virtual bool hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

private:
    std::shared_ptr<hittable> m_ptr;
    vec3 m_offset;
};

bool translate::hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const
{
    ray moved_r(r_in.origin() - m_offset, r_in.dir(), r_in.time());
    if (!m_ptr->hit(moved_r, t_min, t_max, hit_rec))
        return false;

    hit_rec.m_point += m_offset;
    hit_rec.set_face_normal(moved_r, hit_rec.m_normal);

    return true;
}

bool translate::bounding_box(double time0, double time1, aabb& output_box) const
{
    if (!m_ptr->bounding_box(time0, time1, output_box))
        return false;

    output_box = aabb(output_box.min() + m_offset, output_box.max() + m_offset);

    return true;
}

class rotate_y : public hittable
{
public:
    rotate_y(std::shared_ptr<hittable> p, double angle);

    virtual bool hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = m_bbox;
        return m_hasbox;
    }

private:
    std::shared_ptr<hittable> m_ptr;
    double m_sin_theta;
    double m_cos_theta;
    bool m_hasbox;
    aabb m_bbox;
};

rotate_y::rotate_y(std::shared_ptr<hittable> p, double angle) : m_ptr(p)
{
    const double radians = degrees_to_radians(angle);
    m_sin_theta = sin(radians);
    m_cos_theta = cos(radians);
    m_hasbox = m_ptr->bounding_box(0, 1, m_bbox);

    point3 min(INF, INF, INF);
    point3 max(-INF, -INF, -INF);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                auto x = i * m_bbox.max().x() + (1 - i) * m_bbox.min().x();
                auto y = j * m_bbox.max().y() + (1 - j) * m_bbox.min().y();
                auto z = k * m_bbox.max().z() + (1 - k) * m_bbox.min().z();

                auto newx = m_cos_theta * x + m_sin_theta * z;
                auto newz = -m_sin_theta * x + m_cos_theta * z;

                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++)
                {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    m_bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const
{
    auto origin = r_in.origin();
    auto direction = r_in.dir();

    origin[0] = m_cos_theta * r_in.origin()[0] - m_sin_theta * r_in.origin()[2];
    origin[2] = m_sin_theta * r_in.origin()[0] + m_cos_theta * r_in.origin()[2];

    direction[0] = m_cos_theta * r_in.dir()[0] - m_sin_theta * r_in.dir()[2];
    direction[2] = m_sin_theta * r_in.dir()[0] + m_cos_theta * r_in.dir()[2];

    ray rotated_r(origin, direction, r_in.time());

    if (!m_ptr->hit(rotated_r, t_min, t_max, hit_rec))
        return false;

    auto p = hit_rec.m_point;
    auto normal = hit_rec.m_normal;

    p[0] = m_cos_theta * hit_rec.m_point[0] + m_sin_theta * hit_rec.m_point[2];
    p[2] = -m_sin_theta * hit_rec.m_point[0] + m_cos_theta * hit_rec.m_point[2];

    normal[0] = m_cos_theta * hit_rec.m_normal[0] + m_sin_theta * hit_rec.m_normal[2];
    normal[2] = -m_sin_theta * hit_rec.m_normal[0] + m_cos_theta * hit_rec.m_normal[2];

    hit_rec.m_point = p;
    hit_rec.set_face_normal(rotated_r, normal);

    return true;
}
