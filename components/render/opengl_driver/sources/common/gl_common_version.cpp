#include "shared.h"

using namespace render::low_level::opengl;

/*
    ������������
*/

Version::Version ()
  : major_version (0),
    minor_version (0),
    build_number (0)    
{
}

Version::Version (const char* string)
  : major_version (0),
    minor_version (0),
    build_number (0)
{
    //�������� ������������ ����������

  if (!string)
    throw xtl::make_null_argument_exception ("render::low_level::opengl::Version::Version", "string");

    //������ ������ ������
    
  const char* dot = strchr (string, '.');
  
  if (!dot)
    return;

    //��������� �������� ������ ������    

  stl::string buffer (string, dot);  

  major_version = atoi (buffer.c_str ());
  
    //��������� �������� ������ ������
    
  string = dot + 1;    
  dot    = strchr (string, '.');

  if (!dot)
    return;    
    
  buffer.assign (string, dot);  
  
  minor_version = atoi (buffer.c_str ());
  
    //��������� ������ �����
    
  string = dot + 1;

  build_number = atoi (string);
}

/*
    �������� ������������ ������
*/

bool Version::IsValid () const
{
  static Version first_valid_version = "1.1";

  return first_valid_version < *this;
}

/*
    ���������
*/

bool Version::operator < (const Version& version) const
{
  if (major_version != version.major_version)
    return major_version < version.major_version;

  if (minor_version != version.minor_version)
    return minor_version < version.minor_version;

  return build_number < version.build_number;
}

bool Version::operator == (const Version& version) const
{
  return major_version == version.major_version && minor_version == version.minor_version && build_number == version.build_number;
}
