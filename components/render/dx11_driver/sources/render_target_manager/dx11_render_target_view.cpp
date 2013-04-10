#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    ����������� / ����������
*/

View::View (const DeviceManager& device_manager, ITexture* texture, const ViewDesc& desc)
  : DeviceObject (device_manager)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

View::~View ()
{
}

/*
    ��������� ������� ��������
*/

ITexture* View::GetTexture ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� �����������
*/

void View::GetDesc (ViewDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
