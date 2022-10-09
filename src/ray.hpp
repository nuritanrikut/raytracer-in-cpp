#pragma once

#include "vec3.hpp"

class ray_t
{
public:
    ray_t() { }
    ray_t( const point3_t &origin, const vec3_t &direction ) : origin_( origin ), direction_( direction ) { }

    point3_t origin() const
    {
        return origin_;
    }
    vec3_t direction() const
    {
        return direction_;
    }

    point3_t at( double t ) const
    {
        return origin_ + t * direction_;
    }

private:
    point3_t origin_;
    vec3_t direction_;
};
