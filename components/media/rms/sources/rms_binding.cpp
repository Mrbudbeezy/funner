#include "shared.h"

using namespace media::rms;

/*
    �������� ���������� ����������
*/

typedef stl::auto_ptr<ICustomBinding> BindingPtr;

struct Binding::Impl: public xtl::reference_counter
{
  BindingPtr           binding;           //������ ����������
  xtl::auto_connection on_delete_binding; //���������� � ��������, ����������� � ��������� �������� �������

  Impl () {}
  
  Impl (BindingPtr& in_binding) : binding (in_binding)
  {
    xtl::trackable* trackable = binding->GetTrackable ();
    
    if (trackable)
      on_delete_binding = trackable->connect_tracker (xtl::bind (&Impl::ResetBinding, this));
  }

  void ResetBinding ()
  {
    binding.release ();
    on_delete_binding.disconnect ();
  }
};

/*
    ������������ / ���������� / ������������
*/

Binding::Binding ()
  : impl (new Impl)
{
}

Binding::Binding (ICustomBinding* in_binding)
{
  BindingPtr binding (in_binding);

  if (!binding)
    throw xtl::make_null_argument_exception ("media::rms::Binding::Binding", "binding");

  impl = new Impl (binding);
}

Binding::Binding (const Binding& binding)
  : impl (binding.impl)
{
  addref (impl);
}

Binding::~Binding ()
{
  release (impl);
}

Binding& Binding::operator = (const Binding& binding)
{
  Binding (binding).Swap (*this);
  
  return *this;
}

/*
    �������� ��� ���������� ���������
*/

void Binding::Load ()
{
  if (impl->binding)
    impl->binding->Load ();
}

void Binding::Prefetch ()
{
  if (impl->binding)
    impl->binding->Prefetch ();
}

void Binding::Unload ()
{
  if (impl->binding)
    impl->binding->Unload ();
}

/*
    �����
*/

void Binding::Swap (Binding& binding)
{
  stl::swap (impl, binding.impl);
}

namespace media
{

namespace rms
{

void swap (Binding& binding1, Binding& binding2)
{
  binding1.Swap (binding2);
}

}

}
