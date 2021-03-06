#include "shared.h"

template <class T>
stl::string get_array_string (const T* array, size_t size)
{
  stl::string result;

  xtl::to_string (result, array, array + size, ", ");
  
  result.insert (result.begin (), '{');

  result += "}";
  
  return result;
}

#define TEST(X) try { printf ("SetProperty ('%s', %s): ", name, #X); properties.SetProperty (name, X); stl::string result; properties.GetProperty (name, result), printf ("%s\n", result.c_str ()); }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }
#define TEST_ARRAY(X,SIZE) try { printf ("SetProperty ('%s', %s): ", name, get_array_string (X, SIZE).c_str ()); properties.SetProperty (name, SIZE, X); stl::string result; properties.GetProperty (name, result), printf ("%s\n", result.c_str ()); }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void test (PropertyMap& properties, const char* name)
{
  TEST ("");
  TEST ("hello world");
  TEST ("3.14");
  TEST ("1 2.5 3 4");
  TEST ("1 2.5 4");
  TEST ("1 2 3 4 5");
  TEST ("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16");
  TEST ("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16");  
  TEST (13);
  TEST (1.5f);
  TEST (math::vec4f (1.0f, 2.0f, 3.0f, 4.0f));    
  TEST (math::mat4f (3.0f));
  
  int int_array [4] = {21, 22, 23, 24};
  float float_array [16] = {121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136};
  math::vec4f vec_array [4] = {math::vec4f (1.0f), math::vec4f (2.0f), math::vec4f (3.0f), math::vec4f (4.0f)};
  math::mat4f matrix_array [2] = {math::mat4f (1.0f), math::mat4f (2.0f)};

  TEST_ARRAY (int_array, 4);
  TEST_ARRAY (float_array, 16);
  TEST_ARRAY (vec_array, 4);
  TEST_ARRAY (matrix_array, 2);
}

int main ()
{
  printf ("Results of properties_conversion2_test:\n");
  
  try
  {
    PropertyMap properties;
    
    properties.SetProperty ("String",  "");
    properties.SetProperty ("Integer", int (0));
    properties.SetProperty ("Float",   0.0f);
    properties.SetProperty ("Vector",  math::vec4f (0.0f));
    properties.SetProperty ("Matrix",  math::mat4f (1.0f));   
    
    int int_array [4] = {21, 22, 23, 24};
    float float_array [16] = {121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136};
    math::vec4f vec_array [4] = {math::vec4f (1.0f), math::vec4f (2.0f), math::vec4f (3.0f), math::vec4f (4.0f)};
    math::mat4f matrix_array [2] = {math::mat4f (1.0f), math::mat4f (2.0f)};

    properties.SetProperty ("IntArray", 4, &int_array [0]);
    properties.SetProperty ("FloatArray", 16, &float_array [0]);
    properties.SetProperty ("VectorArray", 4, &vec_array [0]);
    properties.SetProperty ("MatrixArray", 2, &matrix_array [0]);

    for (size_t i=0; i<properties.Size (); i++)
      test (properties, properties.PropertyName (i));    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
