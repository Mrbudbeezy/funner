#include "shared.h"

namespace syslib
{

//��������� ����������
void raise (const char* source)
{
  throw xtl::format_not_supported_exception (source, "No threads support for default platform");
}

}
