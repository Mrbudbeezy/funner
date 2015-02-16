#include "shared.h"

const char* f (float x, const char* y, int z, unsigned int w)
{
  printf ("f(%g,%s,%d,%u)\n", x, y, z, w);

  return "all ok";
}

int main ()
{
  printf ("Results of invoker1_test:\n");

  try
  {
    Invoker f1 = make_invoker (&f);
    MyStack stack;

    stack.PushSymbol ("dummy");
    stack.Push (3.0f);
    stack.Push ("Hello world");
    stack.Push ((ptrdiff_t)12);
    stack.Push ((ptrdiff_t)100);

    f1 (stack);

    printf ("stack (size=%u):\n", stack.Size ());

    stl::string buffer;

    for (unsigned int i=0; i<stack.Size (); i++)
    {
      to_string (buffer, stack.GetVariant (i));

      printf ("  item[%u]: '%s'\n", i, buffer.c_str ());
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
