#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    �������� ���������� ��������
*/

typedef stl::vector<Output::Pointer> OutputArray;

struct Adapter::Impl
{
  Log           log;               //�������� ������ OpenGL
  OutputManager output_manager;    //�������� ��������� ������
  Library       library;           //���������� ����� ����� OpenGL
  stl::string   name;              //��� ��������
  
///�����������
  Impl ()  
    : name ("GLX")
  {
  }  
};

/*
    ����������� / ����������
*/

Adapter::Adapter ()
{
  try
  {
    //�������� ����������
    
    impl = new Impl;
    
    impl->log.Printf ("...adapter successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Adapter::Adapter");
    
    throw;
  }
}

Adapter::~Adapter ()
{
  Log log = impl->log;

  log.Printf ("Destroy adapter '%s' (path='%s')", impl->name.c_str (), impl->library->GetName ());

  impl = 0;

  log.Printf ("...adapter successfully destroyed");
}

/*
    ��� �������� / ���� � ������ / ��������
*/

const char* Adapter::GetName ()
{
  return impl->name.c_str ();
}

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::glx::Adapter::Adapter";
}

/*
    ������������ ��������� ��������� ������
*/

size_t Adapter::GetOutputsCount ()
{
  return impl->output_manager.GetOutputsCount ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  return impl->output_manager.GetOutput (index);
}

/*
    ������ ���������� ������
*/

Output::Pointer Adapter::GetOutput (Window window)
{
  return impl->output_manager.FindContainingOutput (window);
}

/*
    ����������
*/

ILibrary& Adapter::GetLibrary ()
{
  return impl->library;
}
