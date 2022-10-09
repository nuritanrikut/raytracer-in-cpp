#pragma once

#include <vector>
#include <memory>

#include "hittable.hpp"

class hittable_list_t : public hittable_t
{
public:
    hittable_list_t() { }
    hittable_list_t( std::shared_ptr<hittable_t> object )
    {
        add( object );
    }

    void clear()
    {
        objects.clear();
    }
    void add( std::shared_ptr<hittable_t> object )
    {
        objects.push_back( object );
    }

    virtual bool hit( const ray_t &r, double t_min, double t_max, hit_record_t &rec ) const override
    {
        hit_record_t temp_rec;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        for( const auto &object : objects )
        {
            if( object->hit( r, t_min, closest_so_far, temp_rec ) )
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

private:
    std::vector<std::shared_ptr<hittable_t>> objects;
};
