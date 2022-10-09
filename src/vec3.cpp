#include "vec3.hpp"

#include <cmath>
#include <iomanip>

#include "utils.hpp"

std::ostream &operator<<( std::ostream &out, const vec3_t &v )
{
    out << '(' << std::fixed << std::setprecision( 8 ) << v.x;
    out << ", " << std::fixed << std::setprecision( 8 ) << v.y;
    out << ", " << std::fixed << std::setprecision( 8 ) << v.z << ')';
    return out;
}

void write_color( std::ostream &out, const color_t &pixel_color, int samples_per_pixel )
{
    const auto scale = 1.0 / samples_per_pixel;

    const auto r = sqrt( scale * pixel_color.x );
    const auto g = sqrt( scale * pixel_color.y );
    const auto b = sqrt( scale * pixel_color.z );

    out << static_cast<int>( 256 * clamp( r, 0.0, 0.999 ) ) << ' ' << static_cast<int>( 256 * clamp( g, 0.0, 0.999 ) )
        << ' ' << static_cast<int>( 256 * clamp( b, 0.0, 0.999 ) ) << '\n';
}

vec3_t operator+( const vec3_t &a, const vec3_t &b )
{
    return vec3_t{ a.x + b.x, a.y + b.y, a.z + b.z };
}

vec3_t operator-( const vec3_t &a, const vec3_t &b )
{
    return vec3_t{ a.x - b.x, a.y - b.y, a.z - b.z };
}

vec3_t operator-( const vec3_t &v )
{
    return vec3_t{ -v.x, -v.y, -v.z };
}

vec3_t operator*( const vec3_t &a, const vec3_t &b )
{
    return vec3_t{ a.x * b.x, a.y * b.y, a.z * b.z };
}

vec3_t operator*( const vec3_t &v, const double t )
{
    return vec3_t{ v.x * t, v.y * t, v.z * t };
}

vec3_t operator*( const double t, const vec3_t &v )
{
    return vec3_t{ v.x * t, v.y * t, v.z * t };
}

vec3_t operator/( const vec3_t &v, const double t )
{
    return vec3_t{ v.x / t, v.y / t, v.z / t };
}

double length( const vec3_t &v )
{
    return std::sqrt( length_squared( v ) );
}

double length_squared( const vec3_t &v )
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

bool near_zero( const vec3_t &v )
{
    // Return true if the vector is close to zero in all dimensions.
    const auto s = 1e-8;
    return ( fabs( v.x ) < s ) && ( fabs( v.y ) < s ) && ( fabs( v.z ) < s );
}

double dot( const vec3_t &a, const vec3_t &b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t cross( const vec3_t &a, const vec3_t &b )
{
    return vec3_t{ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

vec3_t reflect( const vec3_t &v, const vec3_t &n )
{
    return v - 2.0 * dot( v, n ) * n;
}

vec3_t refract( const vec3_t &uv, const vec3_t &n, double etai_over_etat )
{
    const auto cos_theta = fmin( dot( -uv, n ), 1.0 );
    const vec3_t r_out_perp = etai_over_etat * ( uv + cos_theta * n );
    const vec3_t r_out_parallel = -sqrt( fabs( 1.0 - length_squared( r_out_perp ) ) ) * n;
    return r_out_perp + r_out_parallel;
}

vec3_t unit_vector( const vec3_t &v )
{
    return v / length( v );
}

