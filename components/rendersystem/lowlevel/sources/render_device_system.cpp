#include <render/low_level/system.h>

#include <common/singleton.h>
#include <common/strlib.h>
#include <common/exception.h>

#include <stl/hash_map>
#include <stl/string>

#include <xtl/intrusive_ptr.h>

using namespace render::low_level;
using namespace common;

namespace render
{

namespace low_level
{

/*
    �������� ���������� ������� ���������� ��������������� ���������� ���������
*/

class RenderSystemImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void RegisterDriver   (const char* name, IDriver* driver);
    void UnregisterDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDriver* FindDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char*          driver_mask,       //����� ����� ��������
                           const SwapChainDesc& swap_chain,        //���������� ������� ������
                           const char*          init_string = ""); //������ �������������

  private:
    typedef xtl::com_ptr<IDriver>      DriverPtr;
    typedef stl::hash_map<stl::string, DriverPtr> DriverMap;
    
  private:
    DriverMap drivers; //����� ���������
};

}

}

/*
    ����������� ���������
*/

void RenderSystemImpl::RegisterDriver (const char* name, IDriver* driver)
{
  if (!name)
    RaiseNullArgument ("render::low_level::RenderSystem::RegisterDriver", "name");

  if (!driver)
    RaiseNullArgument ("render::low_level::RenderSystem::RegisterDriver", "driver");

  DriverMap::iterator iter = drivers.find (name);

  if (iter != drivers.end ())
    RaiseInvalidArgument ("render::low_level::RenderSystem::RegisterDriver", "name", name,
                          "Driver with this name has been already registered");

  drivers.insert_pair (name, driver);
}

void RenderSystemImpl::UnregisterDriver (const char* name)
{
  if (!name)
    return;
    
  drivers.erase (name);
}

/*
    ����� �������� �� �����
*/

IDriver* RenderSystemImpl::FindDriver (const char* name)
{
  if (!name)
    return 0;
    
  DriverMap::iterator iter = drivers.find (name);
  
  return iter != drivers.end () ? get_pointer (iter->second) : 0;
}

/*
    �������� ���������� ���������
*/

IDevice* RenderSystemImpl::CreateDevice
 (const char*          driver_mask,      //����� ����� ��������
  const SwapChainDesc& swap_chain_desc,  //���������� ������� ������
  const char*          init_string)      //������ �������������
{
  if (!driver_mask)
    return 0;
    
  if (!init_string)
    init_string = "";
    
    //����� ��������
    
  for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->first.c_str (), driver_mask))
    {
      DriverPtr driver = iter->second;

        //�������� SwapChain � ���������� ���������

      xtl::com_ptr<ISwapChain> swap_chain (driver->CreateSwapChain (swap_chain_desc), false);

      return driver->CreateDevice (get_pointer (swap_chain), init_string);
    }

  return 0;
}

/*
    ������ ��� ����������� � ������� ���������� ��������������� ���������� ���������
*/

typedef Singleton<RenderSystemImpl> RenderSystemSingleton;

void RenderSystem::RegisterDriver (const char* name, IDriver* driver)
{
  RenderSystemSingleton::Instance ().RegisterDriver (name, driver);
}

void RenderSystem::UnregisterDriver (const char* name)
{
  RenderSystemSingleton::Instance ().UnregisterDriver (name);
}

IDriver* RenderSystem::FindDriver (const char* name)
{
  return RenderSystemSingleton::Instance ().FindDriver (name);
}

IDevice* RenderSystem::CreateDevice (const char* driver_mask, const SwapChainDesc& swap_chain, const char* init_string)
{
  return RenderSystemSingleton::Instance ().CreateDevice (driver_mask, swap_chain, init_string);
}
