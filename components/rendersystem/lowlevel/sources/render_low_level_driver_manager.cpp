#include <render/low_level/driver.h>

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

class DriverManagerImpl
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
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool CreateSwapChainAndDevice (const char*          driver_mask,     //����� ����� ��������
                                   const SwapChainDesc& swap_chain_desc, //���������� ������� ������
                                   const char*          init_string,     //������ �������������
                                   ISwapChain*&         out_swap_chain,  //�������������� ������� ������
                                   IDevice*&            out_device);     //�������������� ���������� ���������

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

void DriverManagerImpl::RegisterDriver (const char* name, IDriver* driver)
{
  if (!name)
    RaiseNullArgument ("render::low_level::DriverManager::RegisterDriver", "name");

  if (!driver)
    RaiseNullArgument ("render::low_level::DriverManager::RegisterDriver", "driver");

  DriverMap::iterator iter = drivers.find (name);

  if (iter != drivers.end ())
    RaiseInvalidArgument ("render::low_level::DriverManager::RegisterDriver", "name", name,
                          "Driver with this name has been already registered");

  drivers.insert_pair (name, driver);
}

void DriverManagerImpl::UnregisterDriver (const char* name)
{
  if (!name)
    return;
    
  drivers.erase (name);
}

/*
    ����� �������� �� �����
*/

IDriver* DriverManagerImpl::FindDriver (const char* name)
{
  if (!name)
    return 0;
    
  DriverMap::iterator iter = drivers.find (name);
  
  return iter != drivers.end () ? get_pointer (iter->second) : 0;
}

/*
    �������� ������� ������
*/

ISwapChain* DriverManagerImpl::CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc)
{
  if (!driver_mask)
    driver_mask = "*";
    
    //����� ��������

  for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->first.c_str (), driver_mask))
      return iter->second->CreateSwapChain (swap_chain_desc); 

  return 0;
}

/*
    �������� ���������� ���������
*/

bool DriverManagerImpl::CreateSwapChainAndDevice
 (const char*          driver_mask,      //����� ����� ��������
  const SwapChainDesc& swap_chain_desc,  //���������� ������� ������
  const char*          init_string,      //������ �������������
  ISwapChain*&         out_swap_chain,   //�������������� ������� ������
  IDevice*&            out_device)       //�������������� ���������� ���������
{
  if (!driver_mask)
    driver_mask = "*";
    
  if (!init_string)
    init_string = "";
    
    //����� ��������
    
  for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->first.c_str (), driver_mask))
    {
      DriverPtr driver = iter->second;

        //�������� SwapChain � ���������� ���������

      xtl::com_ptr<ISwapChain> swap_chain (driver->CreateSwapChain (swap_chain_desc), false);
      
      IDevice* device = driver->CreateDevice (get_pointer (swap_chain), init_string);
      
      swap_chain->AddRef ();

      out_swap_chain = get_pointer (swap_chain);
      out_device     = device;

      return true;
    }
    
  out_swap_chain = 0;
  out_device     = 0;

  return false;
}

/*
    ������ ��� ����������� � ������� ���������� ��������������� ���������� ���������
*/

typedef Singleton<DriverManagerImpl> DriverManagerSingleton;

void DriverManager::RegisterDriver (const char* name, IDriver* driver)
{
  DriverManagerSingleton::Instance ().RegisterDriver (name, driver);
}

void DriverManager::UnregisterDriver (const char* name)
{
  DriverManagerSingleton::Instance ().UnregisterDriver (name);
}

IDriver* DriverManager::FindDriver (const char* name)
{
  return DriverManagerSingleton::Instance ().FindDriver (name);
}

ISwapChain* DriverManager::CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc)
{
  return DriverManagerSingleton::Instance ().CreateSwapChain (driver_mask, swap_chain_desc);
}

bool DriverManager::CreateSwapChainAndDevice
 (const char*          driver_mask,
  const SwapChainDesc& swap_chain_desc,
  const char*          init_string,
  ISwapChain*&         out_swap_chain,
  IDevice*&            out_device)
{
  return DriverManagerSingleton::Instance ().CreateSwapChainAndDevice (driver_mask, swap_chain_desc, init_string, out_swap_chain, out_device);
}
