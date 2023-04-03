#pragma once

#include <ostream>

class vec3_t
{
public:
    double x{ 0.0 };
    double y{ 0.0 };
    double z{ 0.0 };
};

// Aliases

using point3_t = vec3_t;
using color_t = vec3_t;

std::ostream &operator<<( std::ostream &out, const vec3_t &v );
void write_color( std::ostream &out, const color_t &pixel_color, int samples_per_pixel );

vec3_t operator+( const vec3_t &a, const vec3_t &b );
vec3_t &operator+=( vec3_t &a, const vec3_t &b );
vec3_t operator-( const vec3_t &a, const vec3_t &b );
vec3_t operator-( const vec3_t &v );
vec3_t operator*( const vec3_t &a, const vec3_t &b );
vec3_t operator*( const vec3_t &v, const double t );
vec3_t operator*( const double t, const vec3_t &v );
vec3_t operator/( const vec3_t &v, const double t );

double length( const vec3_t &v );
double length_squared( const vec3_t &v );

bool near_zero( const vec3_t &v );

double dot( const vec3_t &a, const vec3_t &b );
vec3_t cross( const vec3_t &a, const vec3_t &b );

vec3_t reflect( const vec3_t &v, const vec3_t &n );
vec3_t refract( const vec3_t &uv, const vec3_t &n, double etai_over_etat );

vec3_t unit_vector( const vec3_t &v );
