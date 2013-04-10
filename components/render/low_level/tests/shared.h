#ifndef RENDER_LOW_LEVEL_TESTS_SHARED_HEADER
#define RENDER_LOW_LEVEL_TESTS_SHARED_HEADER

#include <stdio.h>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/utils.h>

using namespace render::low_level;

#ifdef _MSC_VER
  #pragma warning (disable : 4250)
#endif

//������ ������� ���������
class ObjectImpl: virtual public IObject, public xtl::reference_counter, public xtl::trackable
{
  public:
    IPropertyList* GetProperties () { return 0; }
  
    void AddRef () { addref (this); }  
    void Release () { release (this); }
    unsigned int UseCount () { return use_count (); }
    
    xtl::trackable& GetTrackable () { return *this; }
    
    xtl::connection RegisterDestroyHandler (xtl::trackable::slot_type& handler)
    {
      return xtl::trackable::connect_tracker (handler);
    }

    xtl::connection RegisterDestroyHandler (const xtl::trackable::function_type& handler)
    {
      return xtl::trackable::connect_tracker (handler);
    }

    xtl::connection RegisterDestroyHandler (const xtl::trackable::function_type& handler, xtl::trackable& linked_trackable)
    {
      return xtl::trackable::connect_tracker (handler, linked_trackable);
    }
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
class TestAdapter: public IAdapter, public ObjectImpl
{
  public:
///��������� �����
    const char* GetName () { return "TestAdapter"; }
    
///��������� ����
    const char* GetPath () { return ""; }
    
///��������� ��������
    const char* GetDescription () { return "render::low_level::TestAdapter"; }
    
///���������� ��������� ������
    size_t GetOutputsCount () { return 1; }
    
///��������� ���������� ������
    IOutput* GetOutput (size_t) { return &output; }
  
  private:
    TestOutput output;
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
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetCaps (DriverCaps& caps)
    {
      caps.can_create_swap_chain_without_device = true;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t    GetAdaptersCount () { return 1; }
    IAdapter* GetAdapter       (size_t) { return &adapter; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* CreateAdapter (const char*, const char*, const char*)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateAdapter");
    }    

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (size_t adapters_count, IAdapter**, const SwapChainDesc&)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateSwapChain");
    }

    ISwapChain* CreateSwapChain (IDevice*, const SwapChainDesc&)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateSwapChain");
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (ISwapChain*, const char*)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateDevice");
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
    TestAdapter adapter;
};

#endif
