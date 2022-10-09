![](sample.png)

# Simple raytracer in C++

A basic implementation of [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) by Peter Shirley in C++

I wrote it in C++, Rust and Go as a learning exercise and to compare the languages.

## Dependencies

C++17 compiler

## Compile and Run

```sh
mkdir build
cmake -B build .
cmake --build build --config Release
```

Run

```sh
time ./build/raytracer simple > test.ppm
```
