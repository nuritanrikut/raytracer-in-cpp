#pragma once

#include "ray.hpp"
#include "vec3.hpp"
#include "utils.hpp"

class camera_t
{
public:
    camera_t( const point3_t &lookfrom,
              const point3_t &lookat,
              const vec3_t &vup,
              double vfov, // vertical field-of-view in degrees
              double aspect_ratio,
              double aperture,
              double focus_dist )
    {
        const auto theta = degrees_to_radians( vfov );
        const auto h = tan( theta / 2.0 );
        const auto viewport_height = 2.0 * h;
        const auto viewport_width = aspect_ratio * viewport_height;

        w = unit_vector( lookfrom - lookat );
        u = unit_vector( cross( vup, w ) );
        v = cross( w, u );

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
    }

    ray_t get_ray( random_number_generator_t &rng, double s, double t ) const
    {
        const vec3_t rd = lens_radius * rng.random_in_unit_disk();
        const vec3_t offset = u * rd.x + v * rd.y;

        return ray_t( origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset );
    }

private:
    point3_t origin;
    point3_t lower_left_corner;
    vec3_t horizontal;
    vec3_t vertical;
    vec3_t u;
    vec3_t v;
    vec3_t w;
    double lens_radius;
};
