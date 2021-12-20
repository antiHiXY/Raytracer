#pragma once
#include <cmath>
#include <iostream>

#include "constants.h"

class vec3;

using point3 = vec3;
using color = vec3;

class vec3
{
public:
    vec3() : m_xyz {0.f, 0.f, 0.f} {}
    vec3(double x, double y, double z) : m_xyz {x, y, z} {}

    double x() const { return m_xyz[0]; }
    double y() const { return m_xyz[1]; }
    double z() const { return m_xyz[2]; }

    vec3    operator -() const { return vec3(-m_xyz[0], -m_xyz[1], -m_xyz[2]); }
    double  operator [] (unsigned int i) const { return m_xyz[i]; }
    double& operator [] (unsigned int i) { return m_xyz[i]; }

    vec3& operator += (const vec3& v)
    {
        m_xyz[0] += v.m_xyz[0];
        m_xyz[1] += v.m_xyz[1];
        m_xyz[2] += v.m_xyz[2];
        return *this;
    }

    vec3& operator *= (double t)
    {
        m_xyz[0] *= t;
        m_xyz[1] *= t;
        m_xyz[2] *= t;
        return *this;
    }

    vec3& operator /= (double t)
    {
        return *this *= 1 / t;
    }

    double length() const
    {
        return std::sqrt(length_squared());
    }

    double length_squared() const
    {
        return m_xyz[0] * m_xyz[0] + m_xyz[1] * m_xyz[1] + m_xyz[2] * m_xyz[2];
    }

    inline static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    inline static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    bool near_zero() const
    {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(m_xyz[0]) < s) && (fabs(m_xyz[1]) < s) && (fabs(m_xyz[2]) < s);
    }

private:
    double m_xyz[3];
};

inline std::ostream& operator << (std::ostream& out, const vec3& v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline vec3 operator + (const vec3& u, const vec3& v)
{
    return vec3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

inline vec3 operator - (const vec3& u, const vec3& v)
{
    return vec3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

inline vec3 operator * (const vec3& u, const vec3& v)
{
    return vec3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline vec3 operator * (double t, const vec3& v)
{
    return vec3(t * v.x(), t * v.y(), t * v.z());
}

inline vec3 operator * (const vec3& v, double t)
{
    return t * v;
}

inline vec3 operator / (const vec3& v, double t)
{
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v)
{
    return   u.x() * v.x()
           + u.y() * v.y()
           + u.z() * v.z();
}

inline vec3 cross(const vec3& u, const vec3& v)
{
    return vec3(u.y() * v.z() - u.z() * v.y(),
                u.z() * v.x() - u.x() * v.z(),
                u.x() * v.y() - u.y() * v.x());
}

inline vec3 unit_vector(const vec3& v)
{
    return v / v.length();
}

vec3 random_in_unit_sphere()
{
    while (true)
    {
        const vec3 p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

vec3 random_in_hemisphere(const vec3& normal)
{
    const vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0)
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

vec3 reflect(const vec3& v, const vec3& n)
{
    return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat)
{
    const double cos_theta = fmin(dot(-uv, n), 1.0);
    const vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    const vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

vec3 random_in_unit_disk()
{
    while (true)
    {
        const vec3 p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}