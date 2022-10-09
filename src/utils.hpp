#pragma once

#include <cmath>
#include <limits>

#include "vec3.hpp"

// Constants

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility Functions

class random_number_generator_t
{
public:
    random_number_generator_t();

    double random_double();
    double random_range( double min, double max );

    vec3_t random_vec3();
    vec3_t random_vec3_range( double min, double max );

    vec3_t random_in_unit_sphere();
    vec3_t random_unit_vector();
    vec3_t random_in_unit_disk();

private:
    unsigned long next();

private:
    unsigned long state;
    unsigned long div;
    unsigned long mod;
};

double clamp( double v, double min, double max );

double degrees_to_radians( double degrees );
