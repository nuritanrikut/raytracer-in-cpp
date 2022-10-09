#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <limits>
#include <iomanip>

#include "vec3.hpp"
#include "hittable_list.hpp"
#include "lambertian.hpp"
#include "metal.hpp"
#include "dielectric.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "utils.hpp"

hittable_list_t simple_scene()
{
    hittable_list_t world;

    const auto ground_material = std::make_shared<lambertian_t>( color_t{ 0.5, 0.5, 0.5 } );
    world.add( std::make_shared<sphere_t>( point3_t{ 0.0, -1000.0, 0.0 }, 1000, ground_material ) );

    const auto material1 = std::make_shared<dielectric_t>( 1.5 );
    world.add( std::make_shared<sphere_t>( point3_t{ 0, 1, 0 }, 1.0, material1 ) );

    const auto material2 = std::make_shared<lambertian_t>( color_t{ 0.4, 0.2, 0.1 } );
    world.add( std::make_shared<sphere_t>( point3_t{ -4, 1, 0 }, 1.0, material2 ) );

    const auto material3 = std::make_shared<metal_t>( color_t{ 0.7, 0.6, 0.5 }, 0.0 );
    world.add( std::make_shared<sphere_t>( point3_t{ 4, 1, 0 }, 1.0, material3 ) );

    return world;
}

hittable_list_t random_scene( random_number_generator_t &rng )
{
    hittable_list_t world;

    const auto ground_material = std::make_shared<lambertian_t>( color_t{ 0.5, 0.5, 0.5 } );
    world.add( std::make_shared<sphere_t>( point3_t{ 0.0, -1000.0, 0.0 }, 1000, ground_material ) );

    const auto world_center = point3_t{ 4, 0.2, 0 };
    const auto radius = 0.2;

    for( int a = -11; a < 11; a++ )
    {
        for( int b = -11; b < 11; b++ )
        {
            const point3_t sphere_center{ a + 0.9 * rng.random_double(), 0.2, b + 0.9 * rng.random_double() };

            if( length( sphere_center - world_center ) > 0.9 )
            {
                const auto choose_mat = rng.random_double();
                std::shared_ptr<material_t> material;
                if( choose_mat < 0.8 )
                {
                    // diffuse
                    const auto albedo = rng.random_vec3();
                    material = std::make_shared<lambertian_t>( albedo );
                }
                else if( choose_mat < 0.95 )
                {
                    // metal_t
                    const auto albedo = rng.random_vec3_range( 0.5, 1 );
                    const auto fuzz = rng.random_range( 0, 0.5 );
                    material = std::make_shared<metal_t>( albedo, fuzz );
                }
                else
                {
                    // glass
                    material = std::make_shared<dielectric_t>( 1.5 );
                }
                world.add( std::make_shared<sphere_t>( sphere_center, radius, material ) );
            }
        }
    }

    const auto material1 = std::make_shared<dielectric_t>( 1.5 );
    world.add( std::make_shared<sphere_t>( point3_t{ 0, 1, 0 }, 1.0, material1 ) );

    const auto material2 = std::make_shared<lambertian_t>( color_t{ 0.4, 0.2, 0.1 } );
    world.add( std::make_shared<sphere_t>( point3_t{ -4, 1, 0 }, 1.0, material2 ) );

    const auto material3 = std::make_shared<metal_t>( color_t{ 0.7, 0.6, 0.5 }, 0.0 );
    world.add( std::make_shared<sphere_t>( point3_t{ 4, 1, 0 }, 1.0, material3 ) );

    return world;
}

color_t ray_color( int i, int j, const ray_t &r, const hittable_t &world, int depth, random_number_generator_t &rng )
{
    if( depth <= 0 )
        return color_t{ 1.0, 1.0, 1.0 };

    hit_record_t rec;
    if( world.hit( r, 0.001, infinity, rec ) )
    {
        ray_t scattered;
        color_t attenuation;
        if( rec.material->scatter( r, rec, rng, attenuation, scattered ) )
        {
            auto recursed_color = ray_color( i, j, scattered, world, depth - 1, rng );
            // std::cerr << "> Scatter " << i << ' ' << j << " dir=" << r.direction() << " color= " << attenuation << ", "
            //           << recursed_color << ", " << ( attenuation * recursed_color ) << std::endl;
            return attenuation * recursed_color;
        }

        // std::cerr << "> Diffuse " << i << ' ' << j << " = " << rec.material->diffuse() << std::endl;
        return rec.material->diffuse();
    }

    const vec3_t unit_direction = unit_vector( r.direction() );
    const auto t = 0.5 * ( unit_direction.y + 1.0 );
    auto sky = ( 1.0 - t ) * color_t{ 1.0, 1.0, 1.0 } + t * color_t{ 0.5, 0.7, 1.0 };

    // std::cerr << "> Sky " << i << ' ' << j << " =" << sky << std::endl;

    return sky;
}

int main( int argc, char **argv )
{
    // Image
    constexpr double aspect_ratio = 16.0 / 10.0;
    constexpr int image_width = 1920;
    constexpr int image_height = static_cast<int>( image_width / aspect_ratio );
    constexpr int samples_per_pixel_x = 16;
    constexpr int samples_per_pixel_y = 16;
    constexpr int max_depth = 50;

    std::cerr << "Rendering " << image_width << 'x' << image_height << " image with " << samples_per_pixel_x << 'x'
              << samples_per_pixel_y << " samples per pixel" << std::endl;

    random_number_generator_t rng;
    rng.random_double();

    // World
    hittable_list_t world;
    if( argc > 1 && std::string( argv[1] ) != "simple" )
    {
        std::cerr << "Loading random scene" << std::endl;
        world = random_scene( rng );
    }
    else
    {
        std::cerr << "Loading simple scene" << std::endl;
        world = simple_scene();
    }

    // camera_t
    const point3_t lookfrom{ 13.0, 2.0, 3.0 };
    const point3_t lookat{ 0.0, 0.0, 0.0 };
    const vec3_t vup{ 0.0, 1.0, 0.0 };
    const double vfov = 20.0; // vertical field-of-view in degrees
    const auto dist_to_focus = 10.0;
    const auto aperture = 0.1;

    camera_t cam{ lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus };

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for( int j = image_height - 1; j >= 0; j-- )
    {
        std::cerr << "Scanlines remaining: " << j << std::endl;
        for( int i = 0; i < image_width; i++ )
        {
            color_t pixel_color{ 0.0, 0.0, 0.0 };
            int s = 0;
            // #pragma omp parallel for shared( pixel_color ) private( s )
            for( s = 0; s < samples_per_pixel_x * samples_per_pixel_y; ++s )
            {
                const auto y = double( s / samples_per_pixel_y ) / samples_per_pixel_y - 0.5;
                const auto x = double( s % samples_per_pixel_y ) / samples_per_pixel_x - 0.5;
                const auto u = ( i + x ) / ( image_width - 1 );
                const auto v = ( j + y ) / ( image_height - 1 );
                const ray_t r = cam.get_ray( rng, u, v );
                pixel_color = pixel_color + ray_color( i, j, r, world, max_depth, rng );
            }

            // std::cerr << "> Pixel " << i << ' ' << j << " color " << pixel_color << std::endl;

            write_color( std::cout, pixel_color, samples_per_pixel_x * samples_per_pixel_y );
        }
    }

    std::cerr << "\nDone." << std::endl;

    return EXIT_SUCCESS;
}
