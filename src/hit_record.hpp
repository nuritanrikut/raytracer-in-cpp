#pragma once

#include <memory>
#include "ray.hpp"
#include "vec3.hpp"

class material_t;

class hit_record_t
{
public:
    point3_t p;
    vec3_t normal;
    std::shared_ptr<material_t> material;
    double t;
    bool front_face;

    void set_face_normal( const ray_t &r, const vec3_t &outward_normal )
    {
        front_face = dot( r.direction(), outward_normal ) < 0.0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};
