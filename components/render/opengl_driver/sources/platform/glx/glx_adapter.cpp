#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

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

typedef stl::vector<Output*> OutputArray;

struct Adapter::Impl
{
  Log         log;     //�������� ������ OpenGL
  OutputArray outputs; //������������������ ���������� ������
  Library     library; //���������� ����� ����� OpenGL
  
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
    exception.touch ("render::low_level::opengl::glx::Adapter::Adapter");
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
  return "GLX";
}

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::glx::Adapter";
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
    throw xtl::make_range_exception ("render::low_level::opengl::glx::Adapter::GetOutput", "index", index, impl->outputs.size ());

  return impl->outputs [index];
}

/*
    ����������� ��������� ������
*/

void Adapter::RegisterOutput (Output* output)
{
  if (!output)
    return;

  impl->outputs.push_back (output);
}

void Adapter::UnregisterOutput (Output* output)
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

/*
    ������ ���������� ������
*/

Output::Pointer Adapter::GetOutput (const void* window_handle)
{
  try
  {
    if (!window_handle)
      throw xtl::make_null_argument_exception ("", "window_handle");
      
      //����� ���������� ������ ����� ��� ���������
      
    for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
      if ((*iter)->GetWindow ().Handle () == window_handle)
        return *iter;

      //�������� ������ ���������� ������

    return Output::Pointer (new Output (this, window_handle), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Adapter::GetOutput");
    throw;
  }  
}

/*
    ����������
*/

ILibrary& Adapter::GetLibrary ()
{
  return impl->library;
}
