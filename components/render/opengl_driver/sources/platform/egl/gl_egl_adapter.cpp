#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
    ���������
*/

const size_t OUTPUT_ARRAY_RESERVE = 8; //������������� ���������� ��������� ������

}

/*
    �������� ���������� ��������
*/

typedef stl::vector<IOutput*> OutputArray;

struct Adapter::Impl
{
  Log         log;     //�������� ������ OpenGL
  OutputArray outputs; //������������������ ���������� ������
  
///�����������
  Impl ()
  {
    outputs.reserve (OUTPUT_ARRAY_RESERVE);
  }
};

/*
    ����������� / ����������
*/

Adapter::Adapter ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
}

/*
    ��� �������� / ���� � ������ / ��������
*/

const char* Adapter::GetName ()
{
  return "EGL";
}

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::egl::Adapter";
}

/*
    ������������ ��������� ��������� ������
*/

size_t Adapter::GetOutputsCount ()
{
  return impl->outputs.size ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  if (index >= impl->outputs.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::egl::Adapter::GetOutput", "index", index, impl->outputs.size ());

  return impl->outputs [index];
}

/*
    ����������� ��������� ������
*/

void Adapter::RegisterOutput (IOutput* output)
{
  static const char* METHOD_NAME = "render::low_level::opengl::egl::Adapter::RegisterOutput";

  if (!output)
    throw xtl::make_null_argument_exception (METHOD_NAME, "output");

  for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
    if (*iter == output)
      throw xtl::format_operation_exception (METHOD_NAME, "Output has already registered");

  impl->outputs.push_back (output);
}

void Adapter::UnregisterOutput (IOutput* output)
{
  if (!output)
    return;
    
  for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
    if (*iter == output)
    {
      impl->outputs.erase (iter);
      
      return;
    }
}
