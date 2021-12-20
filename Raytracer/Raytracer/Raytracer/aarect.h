#pragma once

#include "hittable.h"

class xy_rect : public hittable
{
public:
    xy_rect() {}

    xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, std::shared_ptr<material> mat)
        : m_x0(_x0), m_x1(_x1), m_y0(_y0), m_y1(_y1), m_k(_k), m_mat_ptr(mat)
    {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(point3(m_x0, m_y0, m_k - 0.0001), point3(m_x1, m_y1, m_k + 0.0001));
        return true;
    }

public:
    std::shared_ptr<material> m_mat_ptr;
    double m_x0, m_x1, m_y0, m_y1, m_k;
};

bool xy_rect::hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const
{
    auto t = (m_k - r_in.origin().z()) / r_in.dir().z();
    if (t < t_min || t > t_max)
        return false;

    auto x = r_in.origin().x() + t * r_in.dir().x();
    auto y = r_in.origin().y() + t * r_in.dir().y();

    if (x < m_x0 || x > m_x1 || y < m_y0 || y > m_y1)
        return false;

    hit_rec.m_u = (x - m_x0) / (m_x1 - m_x0);
    hit_rec.m_v = (y - m_y0) / (m_y1 - m_y0);
    hit_rec.m_t = t;

    auto outward_normal = vec3(0, 0, 1);
    hit_rec.set_face_normal(r_in, outward_normal);
    hit_rec.m_mat_ptr = m_mat_ptr;
    hit_rec.m_point = r_in.at(t);

    return true;
}

class xz_rect : public hittable
{
public:
    xz_rect() {}

    xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, std::shared_ptr<material> mat)
        : m_x0(_x0), m_x1(_x1), m_z0(_z0), m_z1(_z1), m_k(_k), m_mat_ptr(mat)
    {};

    virtual bool hit(const ray& r_in, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(point3(m_x0, m_k - 0.0001, m_z0), point3(m_x1, m_k + 0.0001, m_z1));
        return true;
    }

public:
    std::shared_ptr<material> m_mat_ptr;
    double m_x0, m_x1, m_z0, m_z1, m_k;
};

bool xz_rect::hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const
{
    auto t = (m_k - r_in.origin().y()) / r_in.dir().y();
    if (t < t_min || t > t_max)
        return false;

    auto x = r_in.origin().x() + t * r_in.dir().x();
    auto z = r_in.origin().z() + t * r_in.dir().z();

    if (x < m_x0 || x > m_x1 || z < m_z0 || z > m_z1)
        return false;

    hit_rec.m_u = (x - m_x0) / (m_x1 - m_x0);
    hit_rec.m_v = (z - m_z0) / (m_z1 - m_z0);
    hit_rec.m_t = t;

    auto outward_normal = vec3(0, 1, 0);
    hit_rec.set_face_normal(r_in, outward_normal);
    hit_rec.m_mat_ptr = m_mat_ptr;
    hit_rec.m_point = r_in.at(t);

    return true;
}

class yz_rect : public hittable
{
public:
    yz_rect() {}

    yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, std::shared_ptr<material> mat)
        : m_y0(_y0), m_y1(_y1), m_z0(_z0), m_z1(_z1), m_k(_k), m_mat_ptr(mat)
    {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(point3(m_k - 0.0001, m_y0, m_z0), point3(m_k + 0.0001, m_y1, m_z1));
        return true;
    }

public:
    std::shared_ptr<material> m_mat_ptr;
    double m_y0, m_y1, m_z0, m_z1, m_k;
};

bool yz_rect::hit(const ray& r_in, double t_min, double t_max, hit_record& hit_rec) const
{
    auto t = (m_k - r_in.origin().x()) / r_in.dir().x();
    if (t < t_min || t > t_max)
        return false;

    auto y = r_in.origin().y() + t * r_in.dir().y();
    auto z = r_in.origin().z() + t * r_in.dir().z();

    if (y < m_y0 || y > m_y1 || z < m_z0 || z > m_z1)
        return false;

    hit_rec.m_u = (y - m_y0) / (m_y1 - m_y0);
    hit_rec.m_v = (z - m_z0) / (m_z1 - m_z0);
    hit_rec.m_t = t;

    auto outward_normal = vec3(1, 0, 0);
    hit_rec.set_face_normal(r_in, outward_normal);
    hit_rec.m_mat_ptr = m_mat_ptr;
    hit_rec.m_point = r_in.at(t);

    return true;
}