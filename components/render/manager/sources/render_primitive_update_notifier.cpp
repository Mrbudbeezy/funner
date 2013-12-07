#include "shared.h"

using namespace render::manager;

/*
    ���������
*/

const size_t LISTENER_ARRAY_RESERVE_SIZE = 4; //������������� ���������� ����������

/*
    ����������� / ����������
*/

PrimitiveUpdateNotifier::PrimitiveUpdateNotifier ()
{
  listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
}

PrimitiveUpdateNotifier::~PrimitiveUpdateNotifier ()
{
}

/*
    ����������� ����������
*/

void PrimitiveUpdateNotifier::Attach (IPrimitiveUpdateListener* listener)
{
  listeners.push_back (listener);
}

void PrimitiveUpdateNotifier::Detach (IPrimitiveUpdateListener* listener)
{
  listeners.erase (stl::remove (listeners.begin (), listeners.end (), listener), listeners.end ());
}

/*
    ����������
*/

void PrimitiveUpdateNotifier::UpdateRendererPrimitives (size_t group_index, size_t first_primitive_index, size_t primitives_count)
{
  for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    (*iter)->UpdateRendererPrimitives (group_index, first_primitive_index, primitives_count);
}

