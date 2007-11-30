#include "shared.h"

using namespace render::low_level::opengl;

/*
    ������, ����������� �������� ��� ��������
*/

Trackable::Trackable ()
{
}

Trackable::~Trackable ()
{
  try
  {
    destroy_signal ();
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    ����������� ����������� ������� �������� �������
*/

xtl::connection Trackable::RegisterDestroyHandler (xtl::slot<void ()>& slot)
{
  return destroy_signal.connect (slot);
}
