#ifndef BOUND_VOLUMES_TESTS_SHARED
#define BOUND_VOLUMES_TESTS_SHARED

//тестирование компиляции кода с макросами min/max
#define min
#define max

#include <cstdio>

#include <math/utility.h>

#include <bv/axis_aligned_box.h>
#include <bv/plane_list.h>
#include <bv/sphere.h>

using namespace bound_volumes;
using namespace math;

/*
    Вывод приимтивов
*/

void dump (const vec3f& v)
{
  printf ("[%.2g %.2g %.2g]", v.x, v.y, v.z);
}

void dump (const aaboxf& box)
{
  printf ("min=");
  dump (box.minimum ());
  printf (" max=");
  dump (box.maximum ());
}

void dump (const spheref& s)
{
  printf ("center=");
  dump (s.center());
  printf (" radius=%g",s.radius());
}

void dump (const plane_listf& p)
{
  printf ("planes count %u:\n", p.planes_count ());

  for (size_t i = 0, count = p.planes_count (); i < count; i++)
    printf ("  %.2f %.2f %.2f %.2f\n", p [i].a, p [i].b, p [i].c, p [i].d);
}

#endif
