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
    random_number_generator_t(int32_t s, int32_t d, int32_t m);

    random_number_generator_t clone() const;

    double random_double();
    double random_range( double min, double max );

    vec3_t random_vec3();
    vec3_t random_vec3_range( double min, double max );

    vec3_t random_in_unit_sphere();
    vec3_t random_unit_vector();
    vec3_t random_in_unit_disk();

private:
    int32_t next();

private:
    int32_t state;
    int32_t div;
    int32_t mod;
};

double clamp( double v, double min, double max );

double degrees_to_radians( double degrees );
