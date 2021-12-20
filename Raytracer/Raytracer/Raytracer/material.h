#pragma once
#include "constants.h"
#include "hittable.h"
#include "texture.h"

struct hit_record;

class material
{
public:
    virtual bool scatter(const ray& r_in, const hit_record& hit_rec, color& attenuation, ray& scattered) const = 0;
    virtual color emitted(double u, double v, const point3& p) const { return color(0, 0, 0); }
};

class lambertian : public material
{
public:
    lambertian(const color& a) : m_albedo(std::make_shared<solid_color>(a)) {}
    lambertian(std::shared_ptr<texture> a) : m_albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& hit_rec, color& attenuation, ray& scattered) const override
    {
        vec3 scatter_direction = hit_rec.m_normal + random_unit_vector();

        if (scatter_direction.near_zero())
            scatter_direction = hit_rec.m_normal;

        scattered = ray(hit_rec.m_point, scatter_direction, r_in.time());
        attenuation = m_albedo->value(hit_rec.m_u, hit_rec.m_v, hit_rec.m_point);
        return true;
    }

private:
    std::shared_ptr<texture> m_albedo;
};

class metal : public material
{
public:
    metal(const color& a) : m_albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& hit_rec, color& attenuation, ray& scattered) const override
    {
        vec3 reflected = reflect(unit_vector(r_in.dir()), hit_rec.m_normal);
        scattered = ray(hit_rec.m_point, reflected, r_in.time());
        attenuation = m_albedo;
        return (dot(scattered.dir(), hit_rec.m_normal) > 0);
    }

private:
    color m_albedo;
};

class dielectric : public material
{
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(const ray& r_in, const hit_record& hit_rec, color& attenuation, ray& scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        const double refraction_ratio = hit_rec.m_front_face ? (1.0 / ir) : ir;

        const vec3 unit_direction = unit_vector(r_in.dir());
        const double cos_theta = fmin(dot(-unit_direction, hit_rec.m_normal), 1.0);
        const double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, hit_rec.m_normal);
        else
            direction = refract(unit_direction, hit_rec.m_normal, refraction_ratio);

        scattered = ray(hit_rec.m_point, direction, r_in.time());
        return true;
    }

private:
    static double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

private:
    double ir;
};

class diffuse_light : public material
{
public:
    diffuse_light(std::shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override
    {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const override
    {
        return emit->value(u, v, p);
    }

public:
    std::shared_ptr<texture> emit;
};

class isotropic : public material
{
public:
    isotropic(color c) : m_albedo(std::make_shared<solid_color>(c)) {}
    isotropic(std::shared_ptr<texture> a) : m_albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& hit_rec, color& attenuation, ray& scattered
    ) const override
    {
        scattered = ray(hit_rec.m_point, random_in_unit_sphere(), r_in.time());
        attenuation = m_albedo->value(hit_rec.m_u, hit_rec.m_v, hit_rec.m_point);
        return true;
    }

public:
    std::shared_ptr<texture> m_albedo;
};