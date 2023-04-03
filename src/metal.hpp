#pragma once

#include "material.hpp"

class metal_t : public material_t
{
public:
    metal_t( const color_t &a, double f ) : albedo( a ), fuzz( f < 1.0 ? f : 1.0 ) { }

    virtual color_t diffuse() const override
    {
        return albedo;
    }

    virtual bool scatter( const ray_t &r_in,
                          const hit_record_t &rec,
                          random_number_generator_t &rng,
                          color_t &attenuation,
                          ray_t &scattered ) const override
    {
        vec3_t reflected = reflect( unit_vector( r_in.direction() ), rec.normal );
        scattered = ray_t( rec.p, reflected + fuzz * rng.random_in_unit_sphere() );
        attenuation = albedo;
        return dot( scattered.direction(), rec.normal ) > 0.0;
    }

public:
    color_t albedo;
    double fuzz;
};
