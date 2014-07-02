#include "shared.h"

using namespace render::manager;

/*
    ������������
*/

BoxAreaImpl::BoxAreaImpl ()
{
}

BoxAreaImpl::BoxAreaImpl (const render::manager::Box& in_box)
  : box (in_box)
{
}
    
/*
    ������� �������
*/

void BoxAreaImpl::SetBox (const render::manager::Box& in_box)
{
  box = in_box;
}
