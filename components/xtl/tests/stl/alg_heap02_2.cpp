//Тестирование алгоритма push_heap (first, last, less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_heap02_2:\n");

  int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, -9 };
  const int N = sizeof (A) / sizeof (int);

  print ("A =", A, A+N);

  make_heap (A, A+N-1, abs_less);
  print ("[A, A + 9)  =", A, A+N-1);

  push_heap(A, A+N, abs_less);
  print ("[A, A + 10) =", A, A+N);

  return 0;
}

