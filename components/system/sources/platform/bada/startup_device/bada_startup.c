#include <stdio.h>
#include <stdarg.h>

#include <FBaseSys.h>

///����� ����� � ����������
_EXPORT_ int OspMain (int argc, char *argv [])
{
  const char* env [] = {0};

  int result = main (argc, argv, env);
    
  return result;
}
