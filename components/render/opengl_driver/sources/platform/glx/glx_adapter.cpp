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

typedef stl::vector<Output::Pointer> OutputArray;

struct Adapter::Impl
{
  Log           log;     //�������� ������ OpenGL
  OutputArray   outputs; //������������������ ���������� ������
  Library       library; //���������� ����� ����� OpenGL
  
///�����������
  Impl ()  
  {
    //���������� � ��������
    Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();
    
    DisplayLock lock (display);

    outputs.reserve (OUTPUT_ARRAY_RESERVE);
    
    size_t screens_count = XScreenCount (display);
    
    for (size_t i=0; i<screens_count; i++)
      outputs.push_back (Output::Pointer (new Output (display, i), false));
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
    ������ ���������� ������
*/

Output::Pointer Adapter::GetOutput (const void* window_handle)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Adapter::GetOutput(const void*)");
}

/*
    ����������
*/

ILibrary& Adapter::GetLibrary ()
{
  return impl->library;
}
