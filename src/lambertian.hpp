#pragma once

#include "material.hpp"

class lambertian_t : public material_t
{
public:
    lambertian_t( const color_t &a ) : albedo( a ) { }

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
        auto scatter_direction = rec.normal + rng.random_unit_vector();

        // Catch degenerate scatter direction
        if( near_zero( scatter_direction ) )
            scatter_direction = rec.normal;

        scattered = ray_t( rec.p, scatter_direction );
        attenuation = albedo;
        return true;
    }

private:
    color_t albedo;
};
