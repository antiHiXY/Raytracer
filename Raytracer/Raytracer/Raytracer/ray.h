#pragma once
#include "vec3.h"


class ray
{
public:
    ray() {}
    ray(const point3& origin, const vec3& dir, double time = 0.f) 
        : m_origin(origin)
        , m_dir(dir)
        , m_tm(time)
    {}

    point3 origin() const { return m_origin; }
    vec3   dir() const { return m_dir; }
    point3 at(double t) const { return m_origin + t * m_dir; }
    double time() const { return m_tm; }

private:
    point3 m_origin;
    vec3 m_dir;
    double m_tm;
};
