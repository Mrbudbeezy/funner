#include "shared.h"

using namespace math;

typedef stl::set<stl::string> MySet;

int main ()
{
  printf ("Results of wxf_writer_test:\n");

  try
  {
    int   array [] = {1,2,3,4,5,6,7,8,9,10};
    MySet set;
    
    set.insert ("world");    
    set.insert ("sunny");
    set.insert ("hello");
    
    WxfWriter writer (&dump, 2, 8);
    
    writer.WriteComment ("This file is auto generated\nDo not modify it by hand");
    writer.BeginFrame   ("test1");
    writer.WriteComment ("Scalar types serialization");
    writer.BeginFrame   ("scalar");
    writer.Write        ("int", -1);
    writer.Write        ("size_t", (unsigned int)-1);
    writer.Write        ("float", math::constf::pi);
    writer.Write        ("double", math::constd::pi);
    writer.Write        ("long_double", math::constants<long double>::pi);
    writer.Write        ("string", "Hello world");
    writer.Write        ("char", 'x');
    writer.EndFrame     ();
    writer.WriteComment ("Mathlib types serialization");
    writer.BeginFrame   ("mathlib");
    writer.Write        ("vec3f", vec3f (1.1f, 2.2f, 3.3f));
    writer.Write        ("mat4f", mat4f (1.0f));
    writer.Write        ("quatd", quatd (math::constd::pi));
    writer.EndFrame     ();
    writer.WriteComment ("Intervals serialization");
    writer.BeginFrame   ("intervals");
    writer.Write        ("int_array", xtl::make_iterator_range (sizeof (array)/sizeof (*array), array));
    writer.Write        ("stl_set", xtl::make_iterator_range (set));
    writer.EndFrame     ();
    writer.EndFrame     ();
    writer.BeginFrame   ("test2", vec4f (1.0f));
    writer.WriteComment ("This frame need to test block-comments in subframes\nDon't worry about it:)");
    writer.Write        ("bool_flag");
//    writer.EndFrame     (); //нв® б¤Ґ« ­® бЇҐжЁ «м­®!

    writer.Flush ();

    writer.Write ("wrong tag");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
