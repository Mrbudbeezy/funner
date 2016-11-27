// Тестирование swap ()
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

void init_set (multiset<int>& ms)
{
  multiset<int> tmp;
  for (int i=1; i<4; i++) tmp.insert (i);
  tmp.swap (ms);
}

int main () 
{
  printf ("Results of set11m_test:\n");

  multiset <int> s1, s2, s3;

  s1.insert (10);
  s1.insert (20);
  s1.insert (10);
  s2.insert (100);
  s2.insert (100);
  s3.insert (300);

  print ("s1 =", s1.begin (), s1.end ());
  print ("s2 =", s2.begin (), s2.end ());
  print ("s3 =", s3.begin (), s3.end ());

  s1.swap (s2);
  printf ("Result of 's1.swap (s2)':\n");
  print ("s1 =", s1.begin (), s1.end ());
  print ("s2 =", s2.begin (), s2.end ());

  swap (s1, s3);
  printf ("Result of 'swap (s1, s3)':\n");
  print ("s1 =", s1.begin (), s1.end ());
  print ("s3 =", s3.begin (), s3.end ());

  init_set (s3);
  printf ("Result of 'init_set (s3)':\n");
  print ("s3 =", s3.begin (), s3.end ());

  return 0;
}
