#pragma once

#include "ray.hpp"
#include "hit_record.hpp"

class hittable_t
{
public:
    virtual bool hit( const ray_t &r, double t_min, double t_max, hit_record_t &rec ) const = 0;
};
