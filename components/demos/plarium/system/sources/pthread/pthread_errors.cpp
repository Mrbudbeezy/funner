#include "shared.h"

namespace plarium
{

namespace system
{

//��������� ���������� � ����� ������
void pthread_raise_error (const char* source, int status)
{
  throw std::runtime_error (utility::format ("Operation '%s' failed. Reason: '%s' (function exit with code %d)\n", source, strerror (status), status));
}

}

}
