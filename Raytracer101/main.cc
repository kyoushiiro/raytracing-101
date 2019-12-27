#include <iostream>
#include <fstream>
#include <float.h>

#include "random.h"
#include "vec3.h"
#include "ray.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

vec3 color(const ray& r, hittable *world, int depth) {
  hit_record rec;
  if (world->hit(r, 0.001, FLT_MAX, rec)) {
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * color(scattered, world, depth + 1);
    }
    else {
      return vec3(0, 0, 0);
    }
  }
  else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}

int main() {
  std::ofstream out("out.ppm");
  if (out.is_open()) {
    int nx = 400;
    int ny = 200;
    int ns = 20;
    out << "P3\n" << nx << " " << ny << "\n255\n";
    
    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    const int num_hittables = 4;
    hittable* list[num_hittables];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));      // first sphere
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0))); // ground
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));    // small sphere on right
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));   // small sphere on left
    hittable* world = new hittable_list(list, num_hittables);
    camera cam(vec3(-2,2,1), vec3(0,0,-1), vec3(0,1,0), 75, float(nx)/float(ny));
    
    for (int j = ny - 1; j >= 0; j--) {
      for (int i = 0; i < nx; i++) {
        vec3 col(0, 0, 0);
        // For antialiasing, sample a random ns number of pixels around the
        // actual pixel, and average these ns pixels.
        for (int s = 0; s < ns; s++) {
          float u = float(i + random_double()) / float(nx);
          float v = float(j + random_double()) / float(ny);
          ray r = cam.get_ray(u, v);
          col += color(r, world, 0);
        }
        col /= float(ns);
        col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

        int ir = int(255.99 * col[0]);
        int ig = int(255.99 * col[1]);
        int ib = int(255.99 * col[2]);
        out << ir << " " << ig << " " << ib << "\n";
      }
    }
    out.close();
  }
  else {
    std::cout << "FAILED TO OPEN out.ppm\n";
  }
} 