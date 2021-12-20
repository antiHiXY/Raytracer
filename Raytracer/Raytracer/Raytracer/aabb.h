#pragma once
#include "constants.h"

class aabb
{
public:
    aabb() {}
    aabb(const point3& a, const point3& b)
        : m_min(a)
        , m_max(b)
    {}

    point3 min() const { return m_min; }
    point3 max() const { return m_max; }

    bool hit(const ray& r_in, double t_min, double t_max) const
    {
        for (int a = 0; a < 3; a++)
        {
            const auto invD = 1.0f / r_in.dir()[a];
            auto t0 = (min()[a] - r_in.origin()[a]) * invD;
            auto t1 = (max()[a] - r_in.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

private:
    point3 m_min;
    point3 m_max;
};

aabb surrounding_box(aabb box0, aabb box1)
{
    point3 small(fmin(box0.min().x(), box1.min().x()),
                 fmin(box0.min().y(), box1.min().y()),
                 fmin(box0.min().z(), box1.min().z()));

    point3 big(fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));

    return aabb(small, big);
}
