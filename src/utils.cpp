#include <iostream>
#include <iomanip>

#include "utils.hpp"

random_number_generator_t::random_number_generator_t() : div( 1000 ), mod( 1000000 ), state( 675248 )
{
    srand( 1 );
}

unsigned long random_number_generator_t::next()
{
    return state = state * state / div % mod;
}

double random_number_generator_t::random_double()
{
    // use this version if you want actual PRNG
    // return rand() / ( RAND_MAX + 1.0 );

    // use this version if you want repeatable sequence of numbers
    auto t1 = next();
    auto t2 = next();
    return ( t1 * mod + t2 ) / double( mod ) / double( mod );
}

double random_number_generator_t::random_range( double min, double max )
{
    return min + ( max - min ) * random_double();
}

vec3_t random_number_generator_t::random_vec3()
{
    return vec3_t{ random_double(), random_double(), random_double() };
}

vec3_t random_number_generator_t::random_vec3_range( double min, double max )
{
    return vec3_t{ random_range( min, max ), random_range( min, max ), random_range( min, max ) };
}

vec3_t random_number_generator_t::random_in_unit_sphere()
{
    while( true )
    {
        auto p = random_vec3_range( -1.0, 1.0 );
        if( length_squared( p ) >= 1.0 )
            continue;
        return p;
    }
}

vec3_t random_number_generator_t::random_unit_vector()
{
    return unit_vector( random_in_unit_sphere() );
}

vec3_t random_number_generator_t::random_in_unit_disk()
{
    while( true )
    {
        auto p = vec3_t{ random_range( -1.0, 1.0 ), random_range( -1.0, 1.0 ), 0.0 };
        if( length_squared( p ) >= 1.0 )
            continue;
        return p;
    }
}

double clamp( double v, double min, double max )
{
    if( v < min )
        return min;
    if( v > max )
        return max;
    return v;
}

double degrees_to_radians( double degrees )
{
    return degrees * pi / 180.0;
}
