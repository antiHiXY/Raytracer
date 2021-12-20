#pragma once

#include "constants.h"
#include "hittable.h"
#include "hittable_objects.h"

#include <algorithm>

inline bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis);

bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);

class bvh_node : public hittable
{
public:
    bvh_node() {}

    bvh_node(const hittable_objects& list, double time0, double time1)
        : bvh_node(list.get_m_objects(), 0, list.get_m_objects().size(), time0, time1)
    {}

    bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, size_t start, size_t end, double time0, double time1);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

private:
    std::shared_ptr<hittable> m_left;
    std::shared_ptr<hittable> m_right;
    aabb m_box;
};

bool bvh_node::bounding_box(double time0, double time1, aabb& output_box) const
{
    output_box = m_box;
    return true;
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& hit_rec) const
{
    if (!m_box.hit(r, t_min, t_max))
        return false;

    const bool hit_left = m_left->hit(r, t_min, t_max, hit_rec);
    const bool hit_right = m_right->hit(r, t_min, hit_left ? hit_rec.m_t : t_max, hit_rec);

    return hit_left || hit_right;
}

bvh_node::bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, size_t start, size_t end, double time0, double time1)
{
    auto objects = src_objects;

    int axis = random_int(0, 2);
    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1)
    {
        m_left = m_right = objects[start];
    }
    else if (object_span == 2)
    {
        if (comparator(objects[start], objects[start + 1]))
        {
            m_left = objects[start];
            m_right = objects[start + 1];
        }
        else
        {
            m_left = objects[start + 1];
            m_right = objects[start];
        }
    }
    else
    {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span / 2;
        m_left = std::make_shared<bvh_node>(objects, start, mid, time0, time1);
        m_right = std::make_shared<bvh_node>(objects, mid, end, time0, time1);
    }

    aabb box_left, box_right;

    if (!m_left->bounding_box(time0, time1, box_left) || !m_right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    m_box = surrounding_box(box_left, box_right);
}

inline bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis)
{
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min()[axis] < box_b.min()[axis];
}


bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b)
{
    return box_compare(a, b, 0);
}

bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b)
{
    return box_compare(a, b, 1);
}

bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b)
{
    return box_compare(a, b, 2);
}
