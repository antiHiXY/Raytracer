#pragma once
#include "constants.h"

#include "aarect.h"
#include "hittable_objects.h"

class box : public hittable
{
public:
    box() {}
    box(const point3& p0, const point3& p1, std::shared_ptr<material> ptr);

    virtual bool hit(const ray& r_in, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(m_box_min, m_box_max);
        return true;
    }

public:
    point3 m_box_min;
    point3 m_box_max;
    hittable_objects m_sides;
};

box::box(const point3& p0, const point3& p1, std::shared_ptr<material> ptr)
{
    m_box_min = p0;
    m_box_max = p1;

    m_sides.add(std::make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    m_sides.add(std::make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

    m_sides.add(std::make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    m_sides.add(std::make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

    m_sides.add(std::make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    m_sides.add(std::make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool box::hit(const ray& r_in, double t_min, double t_max, hit_record& rec) const
{
    return m_sides.hit(r_in, t_min, t_max, rec);
}
