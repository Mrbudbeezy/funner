#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::macosx;

/*
    �������� ���������� ��������
*/

struct Adapter::Impl
{
  Log           log;            //��������
  OutputManager output_manager; //�������� ��������� ������

  Impl () {}
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
    exception.touch ("render::low_level::opengl::macosx::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
}

/*
   ��� �������� / �������� / ���� � ������
*/

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetName ()
{
  return "MacOSXAdapter";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::macosx::Adapter";
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
