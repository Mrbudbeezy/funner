#include "shared.h"

int main ()
{
  printf ("Results of test_sprites:\n");

  try
  {    
      //������������� ����������

    TestApplication test;

      //������ ����������

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
