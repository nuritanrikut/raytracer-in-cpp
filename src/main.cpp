#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <limits>
#include <iomanip>
#include <functional>
#include <thread>
#include <execution>

#include "vec3.hpp"
#include "hittable_list.hpp"
#include "lambertian.hpp"
#include "metal.hpp"
#include "dielectric.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "utils.hpp"

[[nodiscard]] hittable_list_t simple_scene()
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

[[nodiscard]] hittable_list_t random_scene( random_number_generator_t &rng )
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

[[nodiscard]] color_t
ray_color( int col, int row, const ray_t &r, const hittable_t &world, int depth, random_number_generator_t &rng )
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
            auto recursed_color = ray_color( col, row, scattered, world, depth - 1, rng );
            // std::cerr << "> Scatter " << col << ' ' << row << " dir=" << r.direction() << " attenuation=" << attenuation
            //           << " recursed_color=" << recursed_color << " out=" << ( attenuation * recursed_color ) << '\n';
            return attenuation * recursed_color;
        }

        // std::cerr << "> Diffuse " << col << ' ' << row << " = " << rec.material->diffuse() << '\n';
        return rec.material->diffuse();
    }

    const vec3_t unit_direction = unit_vector( r.direction() );
    const auto t = 0.5 * ( unit_direction.y + 1.0 );
    constexpr color_t white = color_t{ 1.0, 1.0, 1.0 };
    constexpr color_t blue = color_t{ 0.5, 0.7, 1.0 };
    auto sky = white * ( 1.0 - t ) + blue * t;

    // std::cerr << "> Sky " << col << ' ' << row << " = " << sky << '\n';

    return sky;
}

struct Job
{
    color_t color;
    random_number_generator_t rng;
    int row;
    int col;
    const hittable_list_t *world;
    const camera_t *cam;
    int image_width;
    int image_height;
    int samples_per_pixel_x;
    int samples_per_pixel_y;
    int max_depth;
};

[[nodiscard]] color_t render_job( Job &job )
{
    int samples_per_pixel = job.samples_per_pixel_x * job.samples_per_pixel_y;

    color_t color = color_t{ 0.0, 0.0, 0.0 };

    for( int sample_y = 0; sample_y < job.samples_per_pixel_y; sample_y++ )
    {
        double y = double( sample_y ) / job.samples_per_pixel_y - 0.5;
        double v = ( job.row + y ) / ( job.image_height - 1 );

        for( int sample_x = 0; sample_x < job.samples_per_pixel_x; sample_x++ )
        {
            // std::cerr << "Job " << job.col << ' ' << job.row << ", sample " << sample_x << ' ' << sample_y << '\n';

            double x = double( sample_x ) / job.samples_per_pixel_x - 0.5;
            double u = ( job.col + x ) / ( job.image_width - 1 );
            const ray_t r = job.cam->get_ray( job.rng, u, v );
            color += ray_color( job.col, job.row, r, *job.world, job.max_depth, job.rng );
        }
    }

    return color;
}

void render( int image_width, int image_height, std::vector<Job> &jobs, std::vector<std::vector<color_t>> &pixel )
{
    int job_count = image_width * image_height;
    int job_index = 0;

    // #pragma omp parallel for shared( remaining_jobs, remaining_lines )
    std::for_each( std::execution::seq,
                   jobs.begin(),
                   jobs.end(),
                   [&pixel, &job_index, job_count, image_width]( Job &job )
                   {
                       if( job_index % image_width == 0 )
                       {
                           int remaining_lines = ( job_count - job_index ) / image_width;
                           std::cerr << "Lines remaining: " << remaining_lines << "    \r";
                       }
                       pixel[job.row][job.col] = render_job( job );
                       job_index++;
                   } );
    std::cerr << "Lines remaining: 0    \n";
}

int main( int argc, char **argv )
{
    // Image
    constexpr double aspect_ratio = 16.0 / 10.0;
    constexpr int image_width = 192;
    constexpr int image_height = static_cast<int>( image_width / aspect_ratio );
    constexpr int samples_per_pixel_x = 16;
    constexpr int samples_per_pixel_y = 16;
    constexpr int sample_count = samples_per_pixel_x * samples_per_pixel_y;
    constexpr int job_count = image_height * image_width;
    constexpr int max_depth = 50;

    std::cerr << "Rendering " << image_width << 'x' << image_height << " image with " << samples_per_pixel_x << 'x'
              << samples_per_pixel_y << " samples per pixel" << '\n';

    random_number_generator_t rng;
    rng.random_double();

    // World
    hittable_list_t world;
    if( argc > 1 && std::string( argv[1] ) == "simple" )
    {
        std::cerr << "Loading simple scene" << '\n';
        world = simple_scene();
    }
    else
    {
        std::cerr << "Loading random scene" << '\n';
        world = random_scene( rng );
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

    std::vector<Job> jobs;
    jobs.reserve( job_count );
    
    for( int row = 0; row < image_height; row++ )
    {
        // std::cerr << "Creating jobs for line: " << row << "    \r";

        for( int col = 0; col < image_width; col++ )
        {
            Job job;

            job.rng = rng.clone();
            job.row = row;
            job.col = col;
            job.world = &world;
            job.cam = &cam;
            job.image_width = image_width;
            job.image_height = image_height;
            job.samples_per_pixel_x = samples_per_pixel_x;
            job.samples_per_pixel_y = samples_per_pixel_y;
            job.max_depth = max_depth;

            jobs.push_back( job );
        }
    }

    // std::cerr << "\n";
    std::cerr << "Created " << job_count << " jobs\n";

    auto pixel = std::vector<std::vector<color_t>>( image_height, std::vector<color_t>( image_width, color_t{} ) );
    render( image_width, image_height, jobs, pixel );

    std::cerr << "Jobs finished\n";
    std::cerr << "Writing image\n";

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for( int row = image_height - 1; row >= 0; row-- )
    {
        for( int col = 0; col < image_width; col++ )
        {
            write_color( std::cout, pixel[row][col], samples_per_pixel_x * samples_per_pixel_y );
        }
    }

    std::cerr << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
