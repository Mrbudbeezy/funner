#include "shared.h"

//������ ��������� ������ ������� � ������
stl::string read_shader (const char* file_name)
{
  common::InputFile file (file_name);

  stl::string buffer (file.Size (), ' ');

  file.Read (&buffer [0], file.Size ());

  return buffer;
}
