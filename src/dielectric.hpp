#pragma once

#include <fmt/format.h>

#include "material.hpp"
#include "utils.hpp"

class dielectric_t : public material_t
{
public:
    dielectric_t( double index_of_refraction ) : ir( index_of_refraction ) { }

    virtual color_t diffuse() const override
    {
        return color_t{ 1.0, 1.0, 1.0 };
    }

    virtual bool scatter( const ray_t &r_in,
                          const hit_record_t &rec,
                          random_number_generator_t &rng,
                          color_t &attenuation,
                          ray_t &scattered ) const override
    {
        attenuation = color_t{ 1.0, 1.0, 1.0 };
        const double refraction_ratio = rec.front_face ? ( 1.0 / ir ) : ir;

        const vec3_t unit_direction = unit_vector( r_in.direction() );
        const double d = dot( -unit_direction, rec.normal );
        const double cos_theta = fmin( d, 1.0 );
        const double sin_theta = sqrt( 1.0 - cos_theta * cos_theta );

        const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        const double t = rng.random_double();
        const double refl = reflectance( cos_theta, refraction_ratio );
        const bool should_reflect = refl > t;

        // std::cerr << fmt::format( "> reflectance unit_direction( {:+.8}, {:+.8}, {:+.8} ) rec.normal( {:+.8}, {:+.8}, {:+.8} ) d {:+.8} cos_theta {:+.8} refraction_ratio {:+.8} refl {:+.8} t {:+.8}",
        //                           (-unit_direction).x,
        //                           (-unit_direction).y,
        //                           (-unit_direction).z,
        //                           rec.normal.x,
        //                           rec.normal.y,
        //                           rec.normal.z,
        //                           d,
        //                           cos_theta,
        //                           refraction_ratio,
        //                           refl,
        //                           t )
        //           << std::endl;

        vec3_t direction;

        if( cannot_refract || should_reflect )
            direction = reflect( unit_direction, rec.normal );
        else
            direction = refract( unit_direction, rec.normal, refraction_ratio );

        scattered = ray_t( rec.p, direction );
        return true;
    }

    static double reflectance( double cosine, double ref_idx )
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = ( 1.0 - ref_idx ) / ( 1.0 + ref_idx );
        r0 = r0 * r0;
        return r0 + ( 1.0 - r0 ) * pow( ( 1.0 - cosine ), 5.0 );
    }

private:
    double ir; // Index of Refraction
};
