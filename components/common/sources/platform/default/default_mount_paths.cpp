#include "shared.h"

using namespace common;

void DefaultPlatform::MountSystemPaths ()
{
}

namespace
{

/*
    ���������, �������������� ����������� ����� ������������ �������� �����
*/

class Component
{
  public:
    Component ()
    {            
      Platform::MountSystemPaths ();
    }
};

}

extern "C"
{

ComponentRegistrator<Component> StandardFilePathsMount ("common.file_systems.StandardFilePathsMount");

}
