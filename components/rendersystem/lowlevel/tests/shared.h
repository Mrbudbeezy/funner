#ifndef RENDER_LOW_LEVEL_TESTS_SHARED_HEADER
#define RENDER_LOW_LEVEL_TESTS_SHARED_HEADER

#include <stdio.h>

#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <common/exception.h>

#include <render/low_level/system.h>
#include <render/low_level/device.h>

using namespace render::low_level;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4250)
#endif

//������ ������� ���������
class ObjectImpl: virtual public IObject, public xtl::reference_counter
{
  public:
    IPropertyList* GetProperties () { return 0; }
  
    void AddRef () { addref (this); }  
    void Release () { release (this); }
};

//�������� ���������� ������
class TestOutput: public IOutput, public ObjectImpl
{
  public:
    TestOutput ()
    {
      test_mode.width        = 100;
      test_mode.height       = 100;
      test_mode.color_bits   = 24;
      test_mode.refresh_rate = 100;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� / �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName        () { return "TestOutput"; }
    const char* GetDescription () { return "Test output class"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetModesCount () { return 1; }
    void   GetModeDesc   (size_t, OutputModeDesc& mode_desc)
    {
      mode_desc = test_mode;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �����-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCurrentMode (const OutputModeDesc& mode_desc)
    {
      test_mode = mode_desc;
    }

    void GetCurrentMode (OutputModeDesc& mode_desc)
    {
      mode_desc = test_mode;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetGammaRamp (const Color3f table [256]) { }
    void GetGammaRamp (Color3f table [256]) { }

  private:
    OutputModeDesc test_mode;
};

//�������� �������
class TestDriver: public IDriver, public ObjectImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "TestDriver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   GetOutputsCount () { return 1; }
    IOutput* GetOutput       (size_t index) { return &output; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (const SwapChainDesc&)
    {
      RaiseNotImplemented ("TestDriver::CreateSwapChain");
      return 0;
    }

    ISwapChain* CreateSwapChain (IOutput*, const SwapChainDesc&)
    {
      RaiseNotImplemented ("TestDriver::CreateSwapChain");
      return 0;
    }    

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (ISwapChain*, const char*)
    {
      RaiseNotImplemented ("TestDriver::CreateDevice");
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ���������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////   
    void SetDebugLog (const LogFunction& in_log)
    {
      log_fn = in_log;
    }
    
    const LogFunction& GetDebugLog ()
    {
      return log_fn;
    }
    
  private:
    LogFunction log_fn;
    TestOutput  output;
};

#endif
