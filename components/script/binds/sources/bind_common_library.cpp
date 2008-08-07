#include "shared.h"

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
    ��������� (����� ���������)
*/

const char* COMMON_FILE_LIBRARY = "static.File";

/*
    ����������� ���������
*/

void bind_file_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_FILE_LIBRARY);

  lib.Register ("LoadString", make_invoker (implicit_cast<stl::string (*)(const char*)> (&FileSystem::LoadTextFile)));
}

}

namespace script
{

namespace binds
{

/*
    ����������� ���������� ������ ����������
*/

void bind_common_library (Environment& environment)
{
    //����������� ���������

  bind_file_library (environment);
}

}

}
