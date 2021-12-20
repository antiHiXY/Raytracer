#pragma once
#include "constants.h"

class camera
{
public:
    camera(point3 lookfrom, point3 lookat, vec3 vup, double vfov, double aspect_ratio, double aperture, double focus_dist)
    {
        const double theta = degrees_to_radians(vfov);
        const double h = tan(theta / 2);
        const double viewport_height = 2.0 * h;
        const double viewport_width = aspect_ratio * viewport_height;

        const vec3 w = unit_vector(lookfrom - lookat);
        const vec3 u = unit_vector(cross(vup, w));
        const vec3 v = cross(w, u);

        m_origin = lookfrom;
        m_horizontal = focus_dist * viewport_width * u;
        m_vertical = focus_dist * viewport_height * v;
        m_lower_left_corner = m_origin - m_horizontal / 2 - m_vertical / 2 - focus_dist * w;

        m_lens_radius = aperture / 2;
    }

    ray get_ray(double s, double t) const
    {
        const vec3 rd = m_lens_radius * random_in_unit_disk();
        const vec3 offset = m_u * rd.x() + m_v * rd.y();

        return ray(m_origin + offset, m_lower_left_corner + s * m_horizontal + t * m_vertical - m_origin - offset);
    }

private:
    point3 m_origin;
    point3 m_lower_left_corner;
    vec3 m_horizontal;
    vec3 m_vertical;
    vec3 m_u, m_v, m_w;
    double m_lens_radius;
};