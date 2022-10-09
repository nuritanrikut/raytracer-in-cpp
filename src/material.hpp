#pragma once

#include <memory>

#include "vec3.hpp"
#include "ray.hpp"
#include "hit_record.hpp"
#include "utils.hpp"

class material_t
{
public:
    virtual color_t diffuse() const = 0;
    virtual bool scatter( const ray_t &r_in,
                          const hit_record_t &rec,
                          random_number_generator_t &rng,
                          color_t &attenuation,
                          ray_t &scattered ) const = 0;
};
