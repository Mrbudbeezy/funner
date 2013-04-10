#include <stl/hash_map>
#include <stl/string>

#include <xtl/intrusive_ptr.h>
#include <xtl/common_exceptions.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

#include <common/singleton.h>
#include <common/component.h>
#include <common/strlib.h>

using namespace render::low_level;
using namespace common;

/*
    ���������
*/

namespace
{

const char*  DRIVER_COMPONENTS_MASK = "render.low_level.*"; //����� ��� ����������� �������������� ��������� ���������
const size_t DRIVER_RESERVED_SIZE   = 8;                    //������������� ���������� ���������

}

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
///�����������
    DriverManagerImpl ()
    {
      drivers.reserve (DRIVER_RESERVED_SIZE);
    }

///����������� ��������
    void RegisterDriver (const char* name, IDriver* driver)
    {
      static const char* METHOD_NAME = "render::low_level::DriverManager::RegisterDriver";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!driver)
        throw xtl::make_null_argument_exception (METHOD_NAME, "driver");
        
      int index = FindDriverIndex (name);

      if (index != -1)
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Driver with this name has been already registered");
        
      drivers.push_back (DriverDesc (name, driver));
    }

///������ ����������� ��������
    void UnregisterDriver (const char* name)
    {
      if (!name)
        return;
        
      int index = FindDriverIndex (name);
      
      if (index == -1)
        return;
        
      drivers.erase (drivers.begin () + index);
    }

///������ ����������� ���� ���������
    void UnregisterAllDrivers ()
    {
      drivers.clear ();
    }
    
///���������� ���������
    size_t DriversCount ()
    {
      return drivers.size ();
    }

///��� ��������
    const char* DriverName (size_t index)
    {
      if (index >= drivers.size ())
        throw xtl::make_range_exception ("render::low_level::DriverManager::DriverName", "index", index, drivers.size ());
        
      return drivers [index].name.c_str ();
    }

///�������
    IDriver* Driver (size_t index)
    {
      if (index >= drivers.size ())
        throw xtl::make_range_exception ("render::low_level::DriverManager::Driver", "index", index, drivers.size ());      

      return drivers [index].driver.get ();
    }

///����� �������� �� �����
    IDriver* FindDriver (const char* name)
    {
      if (!name)
        return 0;
        
      LoadDefaultDrivers ();
        
      int index = FindDriverIndex (name);

      if (index == -1)
        return 0;
        
      return get_pointer (drivers [index].driver);
    }

///�������� ��������
    IAdapter* CreateAdapter (const char* driver_name, const char* adapter_name, const char* path, const char* init_string)
    {
      static const char* METHOD_NAME = "render::low_level::DriverManagerImpl::CreateAdapter";

        //�������� ������������ ����������

      if (!driver_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "driver_name");

        //�������� ��������� "�� ���������"
        
      LoadDefaultDrivers ();
      
        //����� ��������
        
      int index = FindDriverIndex (driver_name);
      
      if (index == -1)
        throw xtl::make_argument_exception (METHOD_NAME, "driver_name", driver_name, "Driver not registered");
        
        //�������� ��������

      return drivers [index].driver->CreateAdapter (adapter_name, path, init_string);
    }

///�������� ������� ������
    ISwapChain* CreateSwapChain
     (const char*          driver_mask,     //����� ����� ��������
      const char*          adapter_mask,    //����� ����� ��������
      const SwapChainDesc& swap_chain_desc) //���������� ������� ������
    {
        //����� ��������
        
      AdapterArray adapters;        

      DriverPtr driver = GetDriver (driver_mask, adapter_mask, adapters);

        //�������� SwapChain � ���������� ���������

      return driver->CreateSwapChain (adapters.size (), &adapters [0], swap_chain_desc);
    }

///�������� ���������� ���������
    void CreateSwapChainAndDevice
     (const char*               driver_mask,     //����� ����� ��������
      const char*               adapter_mask,    //����� ����� ��������
      const SwapChainDesc&      swap_chain_desc, //���������� ������� ������
      const char*               init_string,     //������ �������������
      xtl::com_ptr<ISwapChain>& out_swap_chain,  //�������������� ������� ������
      xtl::com_ptr<IDevice>&    out_device,      //�������������� ���������� ���������
      xtl::com_ptr<IDriver>&    out_driver)      //�������������� �������
    {
        //�������� ������������ ����������

      if (!init_string)
        init_string = "";

        //����� ��������
        
      AdapterArray adapters;

      DriverPtr driver = GetDriver (driver_mask, adapter_mask, adapters);

        //��������� ������������ ��������

      DriverCaps caps;

      memset (&caps, 0, sizeof (caps));

      driver->GetCaps (caps);

      if (caps.can_create_swap_chain_without_device)
      {
          //�������� SwapChain � ���������� ���������

        xtl::com_ptr<ISwapChain> swap_chain (driver->CreateSwapChain (adapters.size (), &adapters [0], swap_chain_desc), false);
        xtl::com_ptr<IDevice>    device (driver->CreateDevice (get_pointer (swap_chain), init_string), false);

        out_swap_chain = swap_chain;
        out_device     = device;
      }
      else
      {
          //�������� SwapChain � ���������� ���������

        xtl::com_ptr<IDevice>    device (driver->CreateDevice (adapters.size (), &adapters [0], init_string), false);
        xtl::com_ptr<ISwapChain> swap_chain (driver->CreateSwapChain (device.get (), swap_chain_desc), false);

        out_swap_chain = swap_chain;
        out_device     = device;
      }

      out_driver = driver;
    }

  private:
    typedef stl::vector<IAdapter*> AdapterArray;
    
///����� �������� �� �����
    int FindDriverIndex (const char* name)
    {
      if (!name)
        return -1;
        
      int index = 0;
      
      for (DriverList::iterator iter=drivers.begin (), end=drivers.end (); iter!=end; ++iter, ++index)
        if (iter->name == name)
          return index;
          
      return -1;
    }

///��������� ��������
    IDriver* GetDriver (const char* driver_mask, const char* adapter_mask, AdapterArray& adapters)
    {
        //�������� ������������ ����������

      if (!driver_mask)
        driver_mask = "*";
        
      if (!adapter_mask)
        adapter_mask = "*";

        //�������� ��������� "�� ���������"
        
      LoadDefaultDrivers ();
        
        //����� ��������
        
      for (DriverList::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
        if (wcimatch (iter->name.c_str (), driver_mask))
        {
          IDriver* driver = iter->driver.get ();
          
            //����� ���������������� ���������            

          adapters.clear   ();
          adapters.reserve (driver->GetAdaptersCount ());

          for (size_t i=0, count=driver->GetAdaptersCount (); i<count; i++)
          {
            IAdapter*   adapter      = driver->GetAdapter (i);
            const char* adapter_name = "";

            if (!adapter || !(adapter_name = adapter->GetName ()))
              continue; //�������� ������������ ���������� ��������
              
            if (!wcimatch (adapter_name, adapter_mask))
              continue;
              
            adapters.push_back (adapter);
          }          
          
          if (!adapters.empty ())
            return driver;
        }

      throw xtl::format_operation_exception ("render::low_level::DriverManagerImpl::GetDriver",
        "No match driver found (driver_mask='%s', adapter_mask='%s')", driver_mask, adapter_mask);      
    }

///�������� ��������� �� ���������
    void LoadDefaultDrivers ()
    {
      static ComponentLoader loader (DRIVER_COMPONENTS_MASK);
    }

  private:
    typedef xtl::com_ptr<IDriver> DriverPtr;

    struct DriverDesc
    {
      DriverPtr   driver;
      stl::string name;
      
      DriverDesc (const char* in_name, const DriverPtr& in_driver)
        : driver (in_driver)
        , name (in_name)
      {
      }
    };
    
    typedef stl::vector<DriverDesc> DriverList;

  private:
    DriverList drivers; //����� ���������
};

}

}

/*
    ������ ��� ����������� � ������� ���������� ��������������� ���������� ���������
*/

typedef Singleton<DriverManagerImpl> DriverManagerSingleton;

void DriverManager::RegisterDriver (const char* name, IDriver* driver)
{
  DriverManagerSingleton::Instance ()->RegisterDriver (name, driver);
}

void DriverManager::UnregisterDriver (const char* name)
{
  DriverManagerSingleton::Instance ()->UnregisterDriver (name);
}

void DriverManager::UnregisterAllDrivers ()
{
  DriverManagerSingleton::Instance ()->UnregisterAllDrivers ();
}

size_t DriverManager::DriversCount ()
{
  return DriverManagerSingleton::Instance ()->DriversCount ();
}

const char* DriverManager::DriverName (size_t index)
{
  return DriverManagerSingleton::Instance ()->DriverName (index);
}

IDriver* DriverManager::Driver (size_t index)
{
  return DriverManagerSingleton::Instance ()->Driver (index);
}

IDriver* DriverManager::FindDriver (const char* name)
{
  return DriverManagerSingleton::Instance ()->FindDriver (name);
}

IAdapter* DriverManager::CreateAdapter (const char* driver_name, const char* adapter_name, const char* path, const char* init_string)
{
  return DriverManagerSingleton::Instance ()->CreateAdapter (driver_name, adapter_name, path, init_string);
}

ISwapChain* DriverManager::CreateSwapChain
 (const char*          driver_mask,
  const char*          adapter_mask,
  const SwapChainDesc& swap_chain_desc)
{
  return DriverManagerSingleton::Instance ()->CreateSwapChain (driver_mask, adapter_mask, swap_chain_desc);
}

void DriverManager::CreateSwapChainAndDevice
 (const char*               driver_mask,
  const char*               adapter_mask,
  const SwapChainDesc&      swap_chain_desc,
  const char*               init_string,
  xtl::com_ptr<ISwapChain>& out_swap_chain,
  xtl::com_ptr<IDevice>&    out_device)
{
  xtl::com_ptr<IDriver> driver;

  DriverManagerSingleton::Instance ()->CreateSwapChainAndDevice (driver_mask, adapter_mask, swap_chain_desc, init_string, out_swap_chain, out_device, driver);
}

void DriverManager::CreateSwapChainAndDevice
 (const char*               driver_mask,
  const char*               adapter_mask,
  const SwapChainDesc&      swap_chain_desc,
  const char*               init_string,
  xtl::com_ptr<ISwapChain>& out_swap_chain,
  xtl::com_ptr<IDevice>&    out_device,
  xtl::com_ptr<IDriver>&    out_driver)
{
  DriverManagerSingleton::Instance ()->CreateSwapChainAndDevice (driver_mask, adapter_mask, swap_chain_desc, init_string, out_swap_chain, out_device, out_driver);
}

void DriverManager::CreateSwapChainAndDevice
 (const char*          driver_mask,
  const char*          adapter_mask,
  const SwapChainDesc& swap_chain_desc,
  const char*          init_string,
  ISwapChain*&         out_swap_chain,
  IDevice*&            out_device)
{
  xtl::com_ptr<ISwapChain> swap_chain;
  xtl::com_ptr<IDevice>    device;
  xtl::com_ptr<IDriver>    driver;

  CreateSwapChainAndDevice (driver_mask, adapter_mask, swap_chain_desc, init_string, swap_chain, device, driver);

  swap_chain->AddRef ();
  device->AddRef ();

  out_swap_chain = &*swap_chain;
  out_device     = &*device;
}
