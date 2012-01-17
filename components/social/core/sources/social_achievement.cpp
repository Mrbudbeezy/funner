#include "shared.h"

using namespace social;

/*
   ���������� ����������
*/

struct Achievement::Impl : public xtl::reference_counter
{
  stl::string           id;                      //�������������
  stl::string           title;                   //��������
  bool                  hidden;                  //�������� �� �������
  double                progress;                //��������
  common::PropertyMap   properties;              //������ ��������
  const void*           handle;                  //�������������� ����������
  ReleaseHandleFunction handle_release_function; //�������, ���������� ��� ������������ �����������

  Impl ()
    : hidden (false)
    , progress (0)
    , handle (0)
    {}

  ~Impl ()
  {
    handle_release_function (handle);
  }
};

/*
   ����������� / ���������� / �����������
*/

Achievement::Achievement ()
  : impl (new Impl)
  {}

Achievement::Achievement (const Achievement& source)
  : impl (source.impl)
{
  addref (impl);
}

Achievement::~Achievement ()
{
  release (impl);
}

Achievement& Achievement::operator = (const Achievement& source)
{
  Achievement (source).Swap (*this);

  return *this;
}

/*
   �������������
*/

const char* Achievement::Id () const
{
  return impl->id.c_str ();
}

void Achievement::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("social::Achievement::SetId", "id");

  impl->id = id;
}

/*
   ��������
*/

const char* Achievement::Title () const
{
  return impl->title.c_str ();
}

void Achievement::SetTitle (const char* title)
{
  if (!title)
    throw xtl::make_null_argument_exception ("social::Achievement::SetTitle", "title");

  impl->title = title;
}

/*
   �������� �� ������� ��� ������������
*/

bool Achievement::IsHidden () const
{
  return impl->hidden;
}

void Achievement::SetHidden (bool hidden)
{
  impl->hidden = hidden;
}

/*
   �������� ����������
*/

double Achievement::Progress () const
{
  return impl->progress;
}

void Achievement::SetProgress (double progress)
{
  impl->progress = progress;
}

/*
   ������ ��������
*/

const common::PropertyMap& Achievement::Properties () const
{
  return impl->properties;
}

common::PropertyMap& Achievement::Properties ()
{
  return impl->properties;
}

void Achievement::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   ���������/��������� ��������������� �����������
*/

const void* Achievement::Handle () const
{
  return impl->handle;
}

void Achievement::SetHandle (const void* handle, const ReleaseHandleFunction& release_function)
{
  impl->handle_release_function (impl->handle);

  impl->handle = handle;
  impl->handle_release_function = release_function;
}

/*
   �����
*/

void Achievement::Swap (Achievement& source)
{
  stl::swap (impl, source.impl);
}

namespace social
{

/*
   �����
*/

void swap (Achievement& achievement1, Achievement& achievement2)
{
  achievement1.Swap (achievement2);
}

}
