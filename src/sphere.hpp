#pragma once

#include <memory>
#include <cmath>

#include "material.hpp"
#include "hittable.hpp"

class sphere_t : public hittable_t
{
public:
    sphere_t() { }
    sphere_t( point3_t center, double radius, std::shared_ptr<material_t> m )
        : center_( center ),
          radius_( radius ),
          material_( m )
    {
    }

    virtual bool hit( const ray_t &r, double t_min, double t_max, hit_record_t &rec ) const override
    {
        const vec3_t oc = r.origin() - center_;
        const auto a = length_squared( r.direction() );
        const auto half_b = dot( oc, r.direction() );
        const auto c = length_squared( oc ) - radius_ * radius_;

        const auto discriminant = half_b * half_b - a * c;
        if( discriminant < 0 )
            return false;
        const auto sqrtd = sqrt( discriminant );

        // Find the nearest root that lies in the acceptable range.
        auto t = ( -half_b - sqrtd ) / a;
        if( t < t_min || t_max < t )
        {
            t = ( -half_b + sqrtd ) / a;
            if( t < t_min || t_max < t )
                return false;
        }

        rec.t = t;
        rec.p = r.at( rec.t );
        const vec3_t outward_normal = ( rec.p - center_ ) / radius_;
        rec.set_face_normal( r, outward_normal );
        rec.material = material_;

        return true;
    }

private:
    point3_t center_;
    double radius_;
    std::shared_ptr<material_t> material_;
};
