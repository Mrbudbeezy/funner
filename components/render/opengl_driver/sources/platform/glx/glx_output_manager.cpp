#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_OUTPUT_ARRAY_RESERVE = 8; //������������� ���������� ��������� ������

/*
    �������� ����������� �����
*/

typedef xtl::com_ptr<Output>   OutputPtr;
typedef stl::vector<OutputPtr> OutputArray;

}

/*
    �������� ���������� OutputManager
*/

struct OutputManager::Impl: public xtl::reference_counter
{
  OutputArray  outputs;  //���������� ������  
  static Impl* instance; //���������� ��������� ���������� ��������� ��������� ������
  
  Impl ()
  {
    //���������� � ��������
    
    Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();
    
    size_t screens_count = 0;
    
    {
      DisplayLock lock (display);

      screens_count = XScreenCount (display);
    }
    
    outputs.reserve (screens_count);
    
    //���������� ������� ��������� ������
    
    for (size_t screen_number=0; screen_number<screens_count; screen_number++)
      outputs.push_back (Output::Pointer (new Output (display, screen_number), false));

    instance = this;
  }
  
  ~Impl ()
  {
    instance = 0;
  }
};

OutputManager::Impl* OutputManager::Impl::instance = 0;

/*
    ����������� / ����������
*/

OutputManager::OutputManager ()
  : impl (OutputManager::Impl::instance)
{
  if (!impl) impl = new Impl;
  else       addref (impl);
}

OutputManager::~OutputManager ()
{
  release (impl);
}

/*
    ������������ ��������� ��������� ������
*/

size_t OutputManager::GetOutputsCount () const
{
  return impl->outputs.size ();
}

Output* OutputManager::GetOutput (size_t index) const
{
  if (index >= impl->outputs.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::glx::Adapter::GetOutput", "index", index, impl->outputs.size ());

  return get_pointer (impl->outputs [index]);
}

/*
    ����� ���������� ������, ���������� �������� � ����� �����������
*/

Output* OutputManager::FindContainingOutput (Window window) const
{
  return 0;
}
