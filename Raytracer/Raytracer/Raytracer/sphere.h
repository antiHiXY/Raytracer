#pragma once
#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
    sphere() = delete;
    sphere(point3 center, double radius, std::shared_ptr<material> m) 
        : m_center(center)
        , m_radius(radius)
        , m_mat_ptr(m)
    {};

    virtual bool hit(const ray& ray, double t_min, double t_max, hit_record& hit_rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

private:
    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + PI;

        u = phi / (2 * PI);
        v = theta / PI;
    }

private:
    point3 m_center;
    double m_radius;
    std::shared_ptr<material> m_mat_ptr;
};

bool sphere::hit(const ray& ray, double t_min, double t_max, hit_record& hit_rec) const
{
    const vec3 oc = ray.origin() - m_center;
    const double a = ray.dir().length_squared();
    const double half_b = dot(oc, ray.dir());
    const double c = oc.length_squared() - m_radius * m_radius;
    const double discriminant = half_b * half_b - a * c;
    if (discriminant < 0.f)
        return false;
    else
    {
        const double discrim_sqrt = sqrt(discriminant);
        double root = (-half_b - discrim_sqrt) / a;
        if (root < t_min || root > t_max)
        {
            root = (-half_b + discrim_sqrt) / a;
            if (root < t_min || root > t_max)
                return false;
        }
        hit_rec.m_t = root;
        hit_rec.m_point = ray.at(hit_rec.m_t);
        const vec3 outward_normal = (hit_rec.m_point - m_center) / m_radius;
        hit_rec.set_face_normal(ray, outward_normal);
        get_sphere_uv(outward_normal, hit_rec.m_u, hit_rec.m_v);
        hit_rec.m_mat_ptr = m_mat_ptr;
        return true;
    }
}

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const
{
    output_box = aabb(m_center - vec3(m_radius, m_radius, m_radius), m_center + vec3(m_radius, m_radius, m_radius));
    return true;
}
