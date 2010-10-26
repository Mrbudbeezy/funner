#include "shared.h"

using namespace common;

void DefaultPlatform::MountSystemSpecificPaths ()
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
      Platform::MountSystemSpecificPaths ();
    }
};

}

extern "C"
{

ComponentRegistrator<Component> StandardFilePathsMount ("common.file_systems.StandardFilePathsMount");

}
