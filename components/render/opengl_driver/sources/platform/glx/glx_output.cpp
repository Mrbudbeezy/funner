#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    ���������
*/

const size_t OUTPUT_MAX_NAME_SIZE = 128; //������������ ������ ����� ���������� ������

}

/*
    �������� ���������� ���������� ������
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef stl::vector<OutputModeDesc> OutputModeArray;

struct Output::Impl
{
  Log               log;                         //�������� ������������ ��������
  OutputModeArray   modes;                       //������ ������ ����������
  Display*          display;                     //���������� � ��������
  int               screen_number;               //����� ������ �������
  char              name [OUTPUT_MAX_NAME_SIZE]; //��� ������� ������
  
///�����������
  Impl (Display* in_display, const int in_screen_number)
    : display (in_display)
    , screen_number (in_screen_number)
  {
    DisplayLock lock (display);

    *name = 0;
    
    int sizes_count = 0;
    int rates_count = 0;
    
    XRRScreenSize *sizes = XRRSizes (display, screen_number, &sizes_count);
    short         *rates = 0;
    
    for (int sizeID=0; sizeID < sizes_count; sizeID++)
    {
      rates = XRRRates (display, screen_number, sizeID, &rates_count);
      
      for (int rateID=0; rateID < rates_count; rateID++)
      {
        OutputModeDesc mode_desc;
        
        mode_desc.width        = sizes [sizeID].width;
        mode_desc.height       = sizes [sizeID].height;
        mode_desc.color_bits   = 0;
        mode_desc.refresh_rate = rates [rateID];
        
        modes.push_back (mode_desc);
      }
    }
  }
  
///����������
  ~Impl ()
  {
    try
    {

    }
    catch (...)
    {
    }
  }

/*
    ����������� / ����������
*/

Output::Output (Display* display, const int output_number)
{
  try
  {
    if (!display)
      throw xtl::make_null_argument_exception ("", "display");
      
    if (output_number < 0)
      throw xtl::make_null_argument_exception ("", "output_number");

    impl = new Impl (display, output_number);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Output::Output");
    throw;
  }
}

Output::~Output ()
{

}

/*
    ��������� �����
*/

const char* Output::GetName ()
{
  return impl->name;
}

/*
    ��������� ������ �����-�������
*/

size_t Output::GetModesCount ()
{
  return impl->modes.size ();
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  if (mode_index >= impl->modes.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::glx::Output::GetModeDesc", "mode_index", mode_index, impl->modes.size ());
    
  mode_desc = impl->modes [mode_index];
}

/*
    ��������� �������� �����-������
*/

void Output::SetCurrentMode (const OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Output::SetCurrentMode");
}

void Output::GetCurrentMode (OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Output::GetCurrentMode");
}

/*
    ���������� �����-����������
*/

void Output::SetGammaRamp (const Color3f [256])
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::glx::Output::SetGammaRamp", "Gamma ramp not supported in EGL");
}

void Output::GetGammaRamp (Color3f [256])
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::glx::Output::GetGammaRamp", "Gamma ramp not supported in EGL");
}
