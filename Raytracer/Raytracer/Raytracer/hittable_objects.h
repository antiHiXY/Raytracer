#pragma once
#include "hittable.h"

#include <memory>
#include <vector>

class hittable_objects : public hittable
{
public:
    hittable_objects() {};
    hittable_objects(std::shared_ptr<hittable> object) { add(object); }

    void add(std::shared_ptr<hittable> object) { m_objects.push_back(object); }
    void clear() { m_objects.clear(); }
    std::vector<std::shared_ptr<hittable>> get_m_objects() const
    {
        return m_objects;
    }

    virtual bool hit(const ray& ray, double t_min, double t_max, hit_record& hit_rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

private:
    std::vector<std::shared_ptr<hittable>> m_objects;
};

bool hittable_objects::hit(const ray& ray, double t_min, double t_max, hit_record& hit_rec) const
{
    hit_record tmp_hit_rec;
    bool hit_smth = false;
    double closest_so_far = t_max;

    for (const auto& obj : m_objects)
    {
        if (obj->hit(ray, t_min, closest_so_far, tmp_hit_rec))
        {
            hit_smth = true;
            closest_so_far = tmp_hit_rec.m_t;
            hit_rec = tmp_hit_rec;
        }
    }
    return hit_smth;
}

bool hittable_objects::bounding_box(double time0, double time1, aabb& output_box) const
{
    if (m_objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : m_objects)
    {
        if (!object->bounding_box(time0, time1, temp_box)) 
            return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}
